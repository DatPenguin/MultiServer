#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> // Don't forget to compile with -lpthread
#include <fcntl.h>
#include "server.h"

t_broadcast bc = {0};

int fd[2] = {0};

void send_string(char *str, int s_dial) {
    char buf[80];
    memset(buf, 0, 80);
    strcpy(buf, str);
    write(s_dial, buf, strlen(buf));
    if (buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = '\0';
    printf("%sSent [%s] to client %d\n%s", BLUE, buf, s_dial, NORMAL);
}

void welcome(int s_dial) {
    char buf[80];
    memset(buf, 0, 80);
    snprintf(buf, 80, "Welcome. You are Client %d\n", s_dial);
    write(s_dial, buf, strlen(buf));
    if (buf[strlen(buf) - 1] == '\n')
        buf[strlen(buf) - 1] = '\0';
    printf("%sSent [%s] to client %d\n%s", BLUE, buf, s_dial, NORMAL);
}

int main(int argc, char *argv[]) {
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
        printf("Could not create socket");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(LISTENING_PORT);

    if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("[ERROR] : bind failed");
        return 1;
    }

    listen(socket_desc, 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    pthread_t thread_id;

    while (client_sock = accept(socket_desc, (struct sockaddr *) &client, (socklen_t *) &c)) {
        printf("%sClient %d connected from %s:%d%s\n", GREEN, client_sock, inet_ntoa(client.sin_addr),
               ntohs(client.sin_port), NORMAL);

        if (pthread_create(&thread_id, NULL, connection_handler, (void *) &client_sock) < 0) {
            perror("could not create thread");
            return 1;
        }
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }
    return 0;
}

int strequals(char *str1, char *str2) {
    int i = 0;
    while (str1[i] && str2[i])
        if (str1[i] != str2[i++])
            return 0;
    return 1;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc) {
    t_broadcast l_bc = {0};
    char msg[80];

    int opened_w_pipe = 0;
    int opened_r_pipe = 0;

    char buf[80];
    int s_dial = *(int *) socket_desc;

    welcome(s_dial);
    while (1) {
        /*if (l_bc.id != bc.id) {
            l_bc = bc;
            send_string("Broadcast received :\n", s_dial);
            send_string(l_bc.str, s_dial);
        }*/

        if (opened_r_pipe) {
            memset(buf, 0, 80);
            if (read(fd[0], buf, 80) > 0) {
                if (buf[strlen(buf) - 1] == '\n')
                    buf[strlen(buf) - 1] = '\0';
                send_string("[PIPE] ", s_dial);
                send_string(buf, s_dial);
            }

            memset(buf, 0, 80);
        }

        memset(buf, 0, 80);
        read(s_dial, buf, 80);
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = '\0';
        printf("%sReceived [%s] from client %d%s\n", BLUE, buf, s_dial, NORMAL);

        if (strequals(buf, "exit")) {
            send_string("A plus dans l'bus !", s_dial);
            printf("%sClient %d disconnected%s\n", YELLOW, s_dial, NORMAL);
            if (opened_w_pipe) {
                close(fd[0]);
                close(fd[1]);
            }
            break;
        } else if (strequals(buf, "blague")) {
            send_string("cc sava\n", s_dial);
        } /*else if (strequals(buf, "debug")) {
            send_string("Local : ", s_dial);
            send_string(l_bc.str, s_dial);
            send_string("\nGlobal : ", s_dial);
            send_string(bc.str, s_dial);
        } */else if (strequals(buf, "broadcast")) {
            memset(buf, 0, 80);
            read(s_dial, buf, 80);
            send_string("Broadcasted !\n", s_dial);
            bc.str = buf;
            bc.id++;
        } else if (strequals(buf, "connect")) {
            memset(buf, 0, 80);
            read(s_dial, buf, 80);
            int a = atoi(buf);
            fd[0] = s_dial;
            fd[1] = a;
            pipe(fd);
            strcpy(msg, "Test Pipe\n");
            write(fd[1], msg, strlen(msg));
            opened_w_pipe = 1;
            fcntl(fd[0], F_SETFL, fcntl(fd[0], F_GETFL, 0) | O_NONBLOCK);
        } else if (strequals(buf, "receive")) {
            memset(buf, 0, 80);
            if (read(fd[0], buf, 80) > 0)
                opened_r_pipe = 1;
            send_string(buf, s_dial);
        } else if (strequals(buf, "send")) {
            memset(buf, 0, 80);
            read(s_dial, buf, 80);
            strcpy(msg, buf);
            write(fd[1], msg, strlen(msg));
        } else {
            send_string("Ok\n", s_dial);
        }
    }
} 
