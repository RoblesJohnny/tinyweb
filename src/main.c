#include "http.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // http_server server = http_server_init(8080, 100);
    // server.listen_and_serve(&server);

    char *stream = "GET /home HTTP/1.1\nhost: localhost:8080\n";
    http_request *request = http_parse_request(stream);
    printf("%s\n%s\n%s\n", request->method, request->uri, request->version);
    for (size_t i = 0; i < 5; i++)
    {
        printf("%s: %s\n", request->header[i].name, request->header[i].value);
    }
    
    free(request);
    return 0;
}