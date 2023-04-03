# tinyweb
A HTTP library for educational purposes

Have you ever wonder how the HTTP protocol works? 
When someone ask you to develop an HTTP Server/API or something similar your code look like this?

  var http = require('http');

  http.createServer(function (req, res) {
    res.write('Hello World!');
    res.end(); 
  }).listen(8080);

Well if this is your case, this repo could help you. 

Tinyweb is an implementation of the HTTP/1.1 protocol that I'm doing in C. Why C? because if you **really** want to understand how something works. Yo should do it in C. 

Don't get me wrong, Thank God, the cool libraries, languages and frameworks that we have today exist. Otherwise our work would have been a lot harder. But once in a time it is good to do something for the sake of understanding.
