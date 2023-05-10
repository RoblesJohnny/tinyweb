#include "http.h"
#include <stdio.h>
#include <stdlib.h>

void home_handler(http_request *request, http_response *response)
{
    //http_response_header_add(response, "Content-lenght:", "30");
    strcpy(response->body, "<html>hello, world</html>\r\n");
    //http_response_send(request->additional_info.client_socket, response);
}

void index_handler(http_request *request, http_response *response)
{
    
}

int main()
{
    http_server server = http_server_create(8080, 100);
    server.add_url_handler(&server, "/", home_handler, HTTP_METHOD_GET);
    server.add_url_handler(&server, "/index", index_handler, HTTP_METHOD_GET );
    server.listen_and_serve(&server);
    return 0;
}
