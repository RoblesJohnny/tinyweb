#include "http.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    http_server server = http_server_init(8080, 100);
    server.listen_and_serve(&server);
}