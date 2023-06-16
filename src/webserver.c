#include "http.h"
#include <stdio.h>
#include <stdlib.h>

void home_handler(http_request *request, http_response *response)
{
    response->add_header(response, "Content-type:", "text/html");
    response->add_body(response,  "<html><h1>Home</1></html>\r\n");
}

void index_handler(http_request *request, http_response *response)
{
    response->add_body(response, "<html><h1>Index</h1></html>");
}

int main()
{
    http_server server = http_server_create(8080, 100);
    server.add_url_handler(&server, "/", home_handler, HTTP_METHOD_GET);
    server.add_url_handler(&server, "/index", index_handler, HTTP_METHOD_GET);
    server.listen_and_serve(&server);
    return 0;
}
