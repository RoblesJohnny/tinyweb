#include "http.h"
#include <stdio.h>
#include <stdlib.h>

void *home_handler(http_request *req)
{
    printf("%d %s %s\n", req->method, req->uri, req->version);
    http_response response;

    response.code = HTTP_200_OK;
    strcpy(response.header[0].name, "Content-type:");
    strcpy(response.header[0].value, "text/html");
    strcpy(response.header[1].name, "Content-Length:");
    strcpy(response.header[1].value, "30");
    strcpy(response.version, HTTP_SUPPORTED_VERSION);
    strcpy(response.body, "<html>hello, world</html>\r\n");
    http_response_send(req->additional_info.client_socket, &response);
    return NULL;
}

void *index_handler(http_request *req)
{
    printf("%d %s %s\n", req->method, req->uri, req->version);
    return NULL;
}

void *student_handler(http_request *req)
{
    printf("%d %s %s\n", req->method, req->uri, req->version);
    return NULL;
}

int main()
{
    http_server server = http_server_create(8080, 100);
    http_handle_function_add(&server, "/", home_handler, HTTP_METHOD_GET);
    http_handle_function_add(&server, "/index", index_handler, HTTP_METHOD_GET);
    http_handle_function_add(&server, "/student", student_handler, HTTP_METHOD_POST);
    server.listen_and_serve(&server);
    return 0;
}
