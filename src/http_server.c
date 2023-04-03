#include "http.h"

int http_listen_and_serve(const http_server *server)
{
    
    // Listen
    if (listen(server->socket, server->backlog) < 0)
    {
        perror("http_listen_and_serve: Error listening on socket");
        exit(EXIT_FAILURE);
    };

    //Creates a client
    http_client client;

    //Accept connections and requests
    for (;;)
    {
        client.socket = accept(server->socket, (struct sockaddr *)&client.address, (socklen_t *)sizeof(struct sockaddr_in));
        if (client.socket < 0)
        {
            perror("Accept failed");
        }
        puts("Connection Accepted");

        // if (pthread_create(&thread, NULL, connection_handler, (void *)&client) < 0)
        // {
        //     perror("Could not create thread");
        //     return EXIT_FAILURE;
        // }
    }
    return EXIT_SUCCESS;
}

http_server http_server_init(int port, int backlog)
{
    // Create a http_server structure
    http_server server;

    // Create socket descriptor
    server.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server.socket < 0)
    {
        perror("http_server_init: Can not create a socket");
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
        perror("http_server_init: Can not bind a socket");
        exit(EXIT_FAILURE);
    };

    return server;
}
