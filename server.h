//
// Created by matteo on 07/11/17.
//

#ifndef MULTISERVER_SERVER_H
#define MULTISERVER_SERVER_H

#define NORMAL  "\x1B[0m"
#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CYAN  "\x1B[36m"
#define WHITE  "\x1B[37m"

#define LISTENING_PORT 5001

typedef struct s_broadcast {
    int id;
    char *str;
} t_broadcast;

void *connection_handler(void *);

#endif //MULTISERVER_SERVER_H
