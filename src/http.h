#ifndef _HTTP_H
#define _HTTP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Constants
#define MAX_HEADER 100
#define HTTP_METHOD_GET "GET"
#define HTTP_METHOD_HEAD "HEAD"
#define HTTP_SUPPORTED_VERSION "HTTP/1.1"
#define MAX_URI_SIZE 8000
#define MAX_HEADER_NAME 100
#define MAX_HEADER_VALUE 8000
#define MAX_BODY_SIZE 128000
#define MAX_URL_HANDLERS 100
#define MAX_METHOD_NAME_LENGTH 10
#define MAX_VERSION_LENGTH 10
#define BUFFER_SIZE 128000


/******************************************************************************************************
 * Types definition
******************************************************************************************************/

//Represents a http header in a http request
typedef struct http_header
{
    char name[MAX_HEADER_NAME];
    char value[MAX_HEADER_VALUE];
} http_header;

//Represents a http request
typedef struct http_request
{
    char method[MAX_METHOD_NAME_LENGTH]];               //http method (GET, HEAD, POST, etc)
    char version[MAX_VERSION_LENGTH];                   //http version (HTTP/1.1)
    char uri[MAX_URI_SIZE];                             //Resource identifier ("/index.html", "/"", "/home", etc)
    http_header header[MAX_HEADER];                     //Headers array
    char body[MAX_BODY_SIZE];                           //Request body
} http_request;

// represents a client connected to the server
typedef struct http_client
{
    int socket;                 // Client socket
    struct sockaddr_in address; // Address information
    socklen_t address_size;     // Address size
} http_client;

//Represents a http server 
typedef struct http_server
{
    int socket;                                                                                           // Socket descriptor assigned
    int backlog;                                                                                          // Connection backlog supported by the socket
    struct sockaddr_in address;                                                                           // Server address information
    struct 
    {
        char path[MAX_URI_SIZE];
        void *(*function)(void *args);
    } handlers[MAX_URL_HANDLERS];                                                                        //Handler functions structure and array
    
    int (*listen_and_serve)(const struct http_server *self);                                             // Pointer to the function listen_and_serve()
    void (*handle_function)(struct http_server *self, char *path, void *(*function_name)(void *args));   // Pointer to the function handle_function()
} http_server;

/******************************************************************************************************
 * Functions declaration
 ******************************************************************************************************/
http_request *http_request_parse(const char *message);
int http_listen_and_serve(const http_server *server);
http_server http_server_create(int port, int backlog);
void http_handle_function(http_server *server, char *path, void *(*function_name)(void *args));


/******************************************************************************************************
 * Functions implementation
 *******************************************************************************************************/
//Registers a handle function for an specific URL in the handlers array of a server
void http_handle_function(http_server *server, char *path, void *(*function_name)(void *args))
{
    strcpy(server->handlers[1].path, path);
    server->handlers[1].function = function_name;
}

//Listen and serve function
int http_listen_and_serve(const http_server *server)
{

    // Listen
    if (listen(server->socket, server->backlog) < 0)
    {
        perror("http_listen_and_serve: Error listening on socket");
        exit(EXIT_FAILURE);
    };

    // Creates a client
    http_client client;

    // Accept connections and requests
    for (;;)
    {
        client.socket = accept(server->socket, (struct sockaddr *)&client.address, &client.address_size);
        if (client.socket < 0)
        {
            perror("Accept failed");
        }

        // Read from the socket
        char buffer[BUFFER_SIZE];
        int valread = read(client.socket, buffer, BUFFER_SIZE);
        if (valread < 0)
        {
            perror("webserver (read)");
        }

        http_request *request = http_request_parse(buffer);

        //Just for testing purposes
        // printf("%s\n%s\n%s\n", request->method, request->uri, request->version);
        // for (size_t i = 0; i < 5; i++)
        // {
        //     printf("%s: %s\n", request->header[i].name, request->header[i].value);
        // }

        // IMPORTANT>Request handling code here
        if (strcmp(request->uri, server->handlers[1].path) == 0)
        {   
            void *args = NULL;
            void *(*call_func)(void *args) = server->handlers[1].function;
            call_func(args);
        }
        

        free(request);
        close(client.socket);
    }
    return EXIT_SUCCESS;
}

// Creates an http_server object to handle the connections to our server
http_server http_server_create(int port, int backlog)
{
    // Create a http_server structure
    http_server server;

    // Create socket descriptor
    server.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server.socket < 0)
    {
        perror("http_server_create: Can not create a socket");
        exit(EXIT_FAILURE);
    }

    server.address.sin_family = AF_INET;
    server.address.sin_addr.s_addr = INADDR_ANY;
    server.address.sin_port = htons(port);
    server.backlog = backlog;
    server.listen_and_serve = http_listen_and_serve;
    server.handle_function = http_handle_function;

    // Binding
    if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0)
    {
        perror("http_server_create: Can not bind a socket");
        exit(EXIT_FAILURE);
    };

    return server;
}

// Parses a request and returns an http_request structure loaded with the information
// Warning: caller must free the "request" pointer returned by this function
http_request *http_request_parse(const char *message)
{
    http_request *request = malloc(sizeof(http_request));
    char *tmp_stream = strdup(message);
    char *tmp_ptr = tmp_stream; // this pointer will allow us to free the original duplicated message.

    // Parse the request line
    char *line = strsep(&tmp_stream, "\n");
    char *token = NULL;
    while ((token = strsep(&line, " ")) != NULL)
    {
        // Method parsing
        if (strcmp(token, HTTP_METHOD_GET) == 0)
            strcpy(request->method, HTTP_METHOD_GET);
        if (strcmp(token, HTTP_METHOD_HEAD) == 0)
            strcpy(request->method, HTTP_METHOD_HEAD);

        // // URI parsing
        if (token != NULL)
        {
            if (token[0] == '/')
                strcpy(request->uri, token);
        }

        // Version parsing
        if (strcmp(token, HTTP_SUPPORTED_VERSION) == 0)
            strcpy(request->version, HTTP_SUPPORTED_VERSION);
    }

    // Parse the headers
    line = strsep(&tmp_stream, "\n");
    int i = 0;

    while (line != NULL && (strcmp(line, "") != 0))
    {
        token = NULL;

        while ((token = strsep(&line, " ")) != NULL)
        {
            if (token[strlen(token) - 1] == ':')
            {
                strcpy(request->header[i].name, token);
            }
            else
            {
                strcpy(request->header[i].value, token);
            }
        }

        i++;

        line = strsep(&tmp_stream, "\n");
    }
    free(tmp_ptr);

    return request;
}

#endif
