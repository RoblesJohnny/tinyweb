#ifndef _HTTP_H
#define _HTTP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

//Constants
#define MAX_HEADER 100

//Types
typedef struct http_header
{
    char *name;
    char *value;
} http_header;

typedef struct http_request {
    char *method;
    char *version;
    char *uri;
    http_header header[MAX_HEADER];
} http_request;

typedef struct http_server 
{
    int socket;
    int backlog;                //Connection backlog supported in the socket
    struct sockaddr_in address; // Server address information
    int (*listen_and_serve)(struct http_server *self); //Pointer to the function listen and serve
} http_server;

http_server http_server_init(int port, int backlog);

#endif