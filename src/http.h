#ifndef _HTTP_H
#define _HTTP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

//Constants
#define MAX_HEADER 100
#define HTTP_METHOD_GET "GET"
#define HTTP_METHOD_HEAD "HEAD"
#define HTTP_SUPPORTED_VERSION "HTTP/1.1"
#define MAX_URI_SIZE 8000
#define MAX_HEADER_NAME 100
#define MAX_HEADER_VALUE 8000
#define MAX_BODY_SIZE 128000

//Types
typedef struct http_header
{
    char name[MAX_HEADER_NAME];
    char value[MAX_HEADER_VALUE];
} http_header;

typedef struct http_request {
    char method[10];
    char version[10];
    char uri[MAX_URI_SIZE];
    http_header header[MAX_HEADER];
    char body[MAX_BODY_SIZE];
} http_request;

// represents a client connected to the server
typedef struct http_client
{
    int socket;
    struct sockaddr_in address;
} http_client;

typedef struct http_server 
{
    int socket;
    int backlog;                //Connection backlog supported in the socket
    struct sockaddr_in address; // Server address information
    int (*listen_and_serve)(struct http_server *self); //Pointer to the function listen and serve
} http_server;

http_server http_server_init(int port, int backlog);
http_request *http_parse_request(const char *stream);

#endif
