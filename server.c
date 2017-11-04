#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h> // Don't forget to compile with -lpthread

#define NORMAL  "\x1B[0m"
#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CYAN  "\x1B[36m"
#define WHITE  "\x1B[37m"

void *connection_handler(void *);

void send_string(char *str, int s_dial) {
  char buf[80];
  memset (buf, 0, 80);
  strcpy (buf, str) ;
  write (s_dial, buf, strlen (buf)) ;
  if (buf[strlen(buf) - 1] == '\n')
    buf[strlen(buf) - 1] = '\0';
  printf ("%sSent [%s] to client\n%s", BLUE, buf, NORMAL) ;
}

int main(int argc , char *argv[]) {
  int socket_desc , client_sock , c;
  struct sockaddr_in server , client;

  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
    printf("Could not create socket");

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(5000);

  if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
    perror("[ERROR] : bind failed");
    return 1;
  }

  listen(socket_desc, 3);

  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);

  pthread_t thread_id;

  while(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) {
    printf ("%sClient %d connected from %s:%d%s\n", GREEN, client_sock, inet_ntoa (client.sin_addr), ntohs (client.sin_port), NORMAL) ;

    if( pthread_create(&thread_id, NULL,  connection_handler, (void*) &client_sock) < 0) {
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
  char buf[80];
  int s_dial = *(int*)socket_desc;
  
  while (1) {
    memset(buf, 0, 80);
    read(s_dial, buf, 80) ;
    if (buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1] = '\0';
    printf("%sReceived [%s] du client%s\n", BLUE, buf, NORMAL) ;
    
    if (strequals(buf, "exit")) {
      send_string("A plus dans l'bus !", s_dial);
      printf("%sClient %d disconnected%s\n", YELLOW, s_dial, NORMAL);
      break;
    }
    else if (strequals(buf, "blague"))
      send_string("cc sava\n", s_dial);
    else
      send_string("M'en bats les couilles !\n", s_dial);
  }
} 
