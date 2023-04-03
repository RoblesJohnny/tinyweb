#include "http.h"
#include <string.h>

//Parses a request and returns an http_request structure loaded with the information
//Warning: caller must free the "request" pointer returned by this function
http_request *http_parse_request(const char *stream)
{
	http_request *request = malloc(sizeof(http_request));
	char *tmp_stream = strdup(stream);
	char *tmp_ptr = tmp_stream; //this pointer will allow us to free the original duplicated stream.
	
	//Parse the request line
	char *line = strsep(&tmp_stream, "\n");
	char *token = NULL;
	while ((token = strsep(&line, " ")) != NULL)
	{
		//Method parsing
		if (strcmp(token, HTTP_METHOD_GET) == 0)
			strcpy(request->method, HTTP_METHOD_GET);
		if (strcmp(token, HTTP_METHOD_HEAD) == 0)
			strcpy(request->method, HTTP_METHOD_HEAD);

		// // URI parsing
		if (token != NULL)
		{
			if (token[0] == '/')
				strcpy(request->uri, token);
		}

		//Version parsing
		if (strcmp(token, HTTP_SUPPORTED_VERSION) == 0)
			strcpy(request->version, HTTP_SUPPORTED_VERSION);
	}

	//Parse the headers
	line = strsep(&tmp_stream, "\n");
	int i = 0;

	while (line != NULL && (strcmp(line, "") != 0))
	{
		token = NULL;
		
 		while((token = strsep(&line, " ")) != NULL )
		{
			if (token[strlen(token)-1] == ':')
			{
				strcpy(request->header[i].name, token);
			} else {
				strcpy(request->header[i].value, token);
			}
		}

		i++;

		line = strsep(&tmp_stream, "\n");
	}
	free(tmp_ptr);

	return request;
}
