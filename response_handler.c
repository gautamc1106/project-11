#include "response_handler.h"

//Process Request from the client and send the client an appropriate response

char* STATUS_200 = "200_OK\n";
char* STATUS_204 = "204_NO_CONTENT\n";  
char* STATUS_404 = "404_NOT_FOUND\n";
char* STATUS_411 = "411_LENGTH_REQUIRED\n";
char* STATUS_500 = "500_INTERNAL_SERVER_ERROR\n";
char* STATUS_501 = "501_NOT_IMPLEMENTED\n";
char* STATUS_503 = "503_SERVICE_UNAVAILABLE\n";
char* STATUS_505 = "505_HTTP_VERSION_NOT_SUPPORTED\n";

void handleRequest(char* buf, int readRet, int listener, char* response, char* path) {
  Request *request = parse(buf, readRet, listener);
  //calling respond_HTTP_ver function
  // respond_HTTP_ver(request->http_version, response);
  // strcat(response, " ");
  if (!strcmp(request->http_version , "HTTP/1.1")) {
    strcpy(response, request->http_version);
    strcat(response, " ");

    if (!strcasecmp(request->http_method, "HEAD")){
      serve_get(request, response);
    } 

    if(!strcasecmp(request->http_method, "GET")){
    	serve_get(request, response);
    }
    else if (!strcasecmp(request->http_method, "POST")) {
      	serve_post(request, response);        
    }
    else {
      strcat(response, STATUS_501);  
    }
  }
  else {
    strcpy(response, request->http_version);
    strcat(response, " ");
    strcat(response, STATUS_505);
  }
}

//find the file extension, code modified from (http://stackoverflow.com/questions/5309471/getting-file-extension-in-c)
char *file_ext(const char *filename) {
  char *period = strrchr(filename, '.');
  if(!period || period == filename){
    return "";  
  }     
  return period + 1;
}

void serve_get(Request * request, char * response) {

  char header[4096];
  char body[1000000];

  char file_path[1024];

  char cwd[1024];
  getcwd(cwd,sizeof(cwd));
  strcpy(file_path,cwd);

  strcat(file_path,path);
  strcat(file_path, request->http_uri);https://adclick.g.doubleclick.net/pcs/click?xai=AKAOjsst6pF0WQLRVcsNkpLdUDOeqcpcnCHJv1rrgBMO-LXS-2fAOkPkc4AVStW6EHRU-J4EjAWwC1e8vx6abqH0hEYBqZBO8MGnw3CzCILMWNdXfy5mG9hEMQjvrOIll3ZNyD-EdY_RddI&sig=Cg0ArKJSzDJ9WlKblp9T&urlfix=1&adurl=https://adclick.g.doubleclick.net:443/pcs/click%253Fxai%253DAKAOjsuf6oFmO5ym9gmGCBh9ECNur24ZN4aMnDt4lixyxlX-w7hkvPxjj-lyqhcg8HXybM5-s7nW1iFLvyyTQs2aeK80hkn6Pvm8mZFZxaKMaacXqKTJ0NRSKKO19o7msQ%2526sig%253DCg0ArKJSzJcbUZ8eN2IB%2526urlfix%253D1%2526adurl%253Dhttp://adtrk.tw/IjM5%26url%3Dhttp://adtrk.tw/IjM5

  printf(file_path);

  //check if the file exists
  if(access(file_path, F_OK ) != -1 ) {
    strcpy(header, STATUS_200);
    strcat(header, "Server: select_server/1.0\n");
    strcat(header, "Connection: keep-alive\r\n");

    //read from file 
    int fd_in = open(file_path, O_RDONLY);
    char file_buf[1000000];
    strcpy(file_buf, " ");
    if(fd_in < 0) {
      printf("Error 501: Failed to open the file\n"); 
      strcat(response, STATUS_501);
      exit(-1);                   
    }
    //read from the file
    int content_length = read(fd_in,file_buf,sizeof(file_buf));
    printf("File Buf is!! : \n%s\n", file_buf);
    printf("************\n");

    char content_length_str[100];
    sprintf(content_length_str, "%d", content_length);
    strcat(header, "Content-length: ");
    strcat(header, content_length_str);
    strcat(header,"\r\n");

    //if its GET, read from the file buffer
    if(!strcmp(request->http_method, "GET")){
      strcpy(body, file_buf);
      // free(file_buf);
    }

    char ext[1024];

    //check if the file extension is html, image or text
    if (!strcmp(file_ext(request->http_uri), "html")) {
      strcpy(ext, "text/html");
    }
    else if (!strcmp(file_ext(request->http_uri), "png")) {
      strcpy(ext, "image/png");
    }
    else if (!strcmp(file_ext(request->http_uri), "css")) {
      strcpy(ext, "text/css");
    }
    else if (!strcmp(file_ext(request->http_uri), "jpg")) {
      strcpy(ext, "image/jpeg");
    }
    else if (!strcmp(file_ext(request->http_uri), "txt")) {
      strcpy(ext, "text/plain");
    }
    else if (!strcmp(file_ext(request->http_uri), "gif")) {
      strcpy(ext, "image/gif");
    }
    else { 
      strcpy(ext, "application/octet-stream");
    }

    strcat(header, "Content-Type: ");
    strcat(header, ext);
    strcat(header,"\r\n");

    // Get Date, Time
    char get_date_time[500];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(get_date_time, sizeof get_date_time, "%a, %d %b %Y %H:%M:%S %Z", &tm);
    strcat(header, "Date: ");
    strcat(header, get_date_time);
    strcat(header, "\r\n");

    // Get the latest time (code modified from: http://stackoverflow.com/questions/10446526/get-last-modified-time-of-file-in-linux)
    struct stat attr;
    stat(request->http_uri, &attr);
    strcat(header, "Last-Modified: ");
    strcat(header, ctime(&attr.st_mtime));
    strcat(header, "\r\n");

  }
  else{
    strcat(header, STATUS_404);
    printf("File was not found");
  }

  strcat(header, "\r\n");
  strcat(response, header);

  if(!strcmp(request->http_method, "GET")){
    strcat(response, body);
  }
}

//response when the http request is POST
void serve_post(Request * request, char * response)  {
  if(access(request->http_uri, F_OK ) != -1 ) 
    strcat(response, STATUS_200);    
  else
    strcat(response, STATUS_204);
}



