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

//Enumeration for the http methods
typedef enum http_method
{
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE, 
    HTTP_METHOD_UNDEFINED
} http_method;

//Response codes supported
typedef enum http_response_code
{
    HTTP_200_OK=200,
    HTTP_404_NOT_FOUND=404,
    HTTP_501_NOT_IMPLEMENTED=501,
    HTTP_505_VERSION_NOT_SUPPORTED=505
} http_response_code;

// Represents an http response
typedef struct http_response
{
    char version[MAX_VERSION_LENGTH];
    http_response_code code;
    struct
    {
        char name[MAX_HEADER_NAME];
        char value[MAX_HEADER_VALUE];
    } header[MAX_HEADER];     // Headers array
    char body[MAX_BODY_SIZE];
} http_response;

// Represents an http request
typedef struct http_request
{
    http_method method;                  // http method (GET, HEAD, POST, etc)
    char version[MAX_VERSION_LENGTH];    // http version (HTTP/1.1)
    char uri[MAX_URI_SIZE];              // Resource identifier ("/index.html", "/"", "/home", etc)
    struct
    {
        char name[MAX_HEADER_NAME];
        char value[MAX_HEADER_VALUE];
    } header[MAX_HEADER];               // Headers array
    char body[MAX_BODY_SIZE];           // Request body
    struct                              // Request additional information 
    {
        int client_socket;              // Connected client socket descriptor
    } additional_info;
    
} http_request;

// Represents an http server
typedef struct http_server
{
    int socket;                 // Socket descriptor assigned
    int backlog;                // Connection backlog supported by the socket
    struct sockaddr_in address; // Server address information
    struct
    {
        char path[MAX_URI_SIZE];
        void *(*function)(http_request *req);
        http_method method;
    } url_handlers[MAX_URL_HANDLERS]; // Handler functions structure and array

    struct
    {
        int socket;                 // Client socket
        struct sockaddr_in address; // Address information
        socklen_t address_size;     // Address size
    } connected_client;             // Connected client information structure

    int (*listen_and_serve)(struct http_server *self);                                                                     // Pointer to the function listen_and_serve()
} http_server;

//Type definition for a pointer to a handle function for handle requests
typedef void *(*handle_function_t)(http_request *req);


/******************************************************************************************************
 * Functions declaration
 ******************************************************************************************************/
http_request *http_request_parse(const char *message);
int http_listen_and_serve(http_server *server);
http_server http_server_create(int port, int backlog);
void http_handle_function_add(http_server *server, char *path, handle_function_t function_name, http_method method);
int http_response_send(int connected_client_socket, const http_response *response);

/******************************************************************************************************
 * Functions implementation
 *******************************************************************************************************/

//Sends an http_response to the client
int http_response_send(int connected_client_socket, const http_response *response)
{
    //Creates the buffer to store the serialized response
    char buffer[BUFFER_SIZE] = "";
    char headers[100] = ""; //TODO: Create a MAX headers lenght in a response

    int i = 0;
    while (strcmp(response->header[i].name, "") != 0)
    {
        strcat(headers, response->header[i].name);
        strcat(headers, " ");
        strcat(headers, response->header[i].value);    
        strcat(headers, "\r\n");
        i++;
    }

    //Serializes the response
    if (sprintf(buffer, "%s %d\r\n%s\r\n%s", response->version, response->code, headers, response->body) < 0)
    {
        perror("Error serializing response");
    }

    //Writes to the client
    int valwrite = write(connected_client_socket, buffer, strlen(buffer));
    if (valwrite < 0)
    {
        perror("http_response_send");
    }

    return 0; 
}

// Registers a handle function for an specific URL in the url_handlers array of a server
void http_handle_function_add(http_server *server, char *path, handle_function_t function_name, http_method method)
{
    for (size_t i = 0; i < MAX_URL_HANDLERS; i++)
    {
        if (strcmp(server->url_handlers[i].path, "") == 0)
        {
            strcpy(server->url_handlers[i].path, path);
            server->url_handlers[i].function = function_name;
            server->url_handlers[i].method = method;
            break;
        }

        if (i == MAX_URL_HANDLERS)
        {
            perror("Server URL Handlers array is full");
            exit(EXIT_FAILURE);
        }
    }
}

// Listen and serve function
int http_listen_and_serve(http_server *server)
{

    // Listen
    if (listen(server->socket, server->backlog) < 0)
    {
        perror("http_listen_and_serve: Error listening on socket");
        exit(EXIT_FAILURE);
    };

    // Accept connections and requests
    for (;;)
    {
        server->connected_client.socket = accept(server->socket, (struct sockaddr *)&server->connected_client.address, &server->connected_client.address_size);
        if (server->connected_client.socket < 0)
        {
            perror("Accept failed");
        }

        // Read from the socket
        char buffer[BUFFER_SIZE];
        int valread = read(server->connected_client.socket, buffer, BUFFER_SIZE);
        if (valread < 0)
        {
            perror("webserver (read)");
        }

        http_request *request = http_request_parse(buffer);
        request->additional_info.client_socket = server->connected_client.socket;

        // //Just for testing purposes
        // printf("%s\n%s\n%s\n", request->method, request->uri, request->version);
        // for (size_t i = 0; i < 5; i++)
        // {
        //     printf("%s %s\n", request->header[i].name, request->header[i].value);
        // }

        // IMPORTANT>Request handling code here

        for (size_t i = 0; i < MAX_URL_HANDLERS; i++)
        {
            if ((strcmp(request->uri, server->url_handlers[i].path) == 0) && (request->method == server->url_handlers[i].method))
            {
                handle_function_t call_func = server->url_handlers[i].function;
                call_func(request);
                break;
            }

            if (i == (MAX_URL_HANDLERS - 1))
            {
                perror("Handler not registered");
            }
        }

        free(request);
        close(server->connected_client.socket);
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

    // Binding
    if (bind(server.socket, (struct sockaddr *)&server.address, sizeof(server.address)) < 0)
    {
        perror("http_server_create: Can not bind a socket");
        exit(EXIT_FAILURE);
    };

    // Url handlers initialization
    for (size_t i = 0; i < MAX_URL_HANDLERS; i++)
    {
        strcpy(server.url_handlers[i].path, "");
        server.url_handlers[i].function = NULL;
        server.url_handlers[i].method = HTTP_METHOD_UNDEFINED;
    }

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
        if (strcmp(token, "GET") == 0)
            request->method = HTTP_METHOD_GET;
        if (strcmp(token, "HEAD") == 0)
            request->method = HTTP_METHOD_HEAD;
        if (strcmp(token, "POST") == 0)
            request->method = HTTP_METHOD_POST;

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
