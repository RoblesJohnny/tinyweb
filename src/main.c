#include "http.h"
#include <stdio.h>
#include <stdlib.h>

void home_handler(http_request *request, http_response *response)
{
    strcpy(response->header[0].name, "Content-type:");
    strcpy(response->header[0].value, "text/html");
    strcpy(response->header[1].name, "Content-Length:");
    strcpy(response->header[1].value, "30");
    strcpy(response->body, "<html>hello, world</html>\r\n");
    //http_response_send(request->additional_info.client_socket, response);
}

void index_handler(http_request *rq, http_response *rs)
{
    
}

int main()
{
    http_server server = http_server_create(8080, 100);
    http_handle_function_add(&server, "/", home_handler, HTTP_METHOD_GET);
    http_handle_function_add(&server, "/index", index_handler, HTTP_METHOD_GET );
    server.listen_and_serve(&server);
    return 0;
}
