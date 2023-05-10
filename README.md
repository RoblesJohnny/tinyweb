# Tinyweb
A simple HTTP/1.1 C library

## Description 
Have you ever wonder how the HTTP protocol works? 
When someone ask you to develop an HTTP Server/API or something similar your code look like this?

```javascript
  var http = require('http');

  http.createServer(function (req, res) {
    res.write('Hello World!');
    res.end(); 
  }).listen(8080);
```
Well if this is your case, this repo could help you. 

Tinyweb is an implementation of the HTTP/1.1 protocol written in C. Why C? Because if you **really** want to understand how something works, you should do it in C. 

## Pre requisites
1. clang or gcc installed

## Features
1. HTTP1.1 Support
2. Basic functions to create an http server, parse a request, create and send responses

## Limitations
1. No https
2. Not all response codes are implemented
3. No multithreading

## Examples
1. A Basic http web server in C (webserver.c)

```C
#include "http.h"

void home_handler(http_request *request, http_response *response)
{
    response.code = HTTP_200_OK;
    response->add_header(response, "Content-type:", "text/html");
    response->add_header(response, "Content-lenght:", "28");
    response->add_body(response,  "<html><h1>Home</1></html>\r\n");
}

int main()
{
  http_server server = http_server_create(8080, 100);
  server.add_url_handler(&server, "/", home_handler, HTTP_METHOD_GET);
  server.listen_and_serve(&server);
  return (0);
}
```

In this example you first create an http_server structure which will contain all the information and functions necessary to operate the server, you must specify the listening port (8080) and the connection backlog the server will allow (100 connections).

After the server creation, use the function server.add_url_handler to register a url handler in the server. In this case the "/" path is registered to be handled by the home_handler function and just for the HTTP_METHOD_GET.

Finally, server.listen_and_serve starts the server and listening in the 8080 port waiting for connections.

## Main types

### The http_server type
1. Structure
2. Fields
3. Function pointers

### The http_request type
1. Structure
2. Fields
3. Function pointers

### The http_response type
1. Structure
2. Fields
3. Function pointers
