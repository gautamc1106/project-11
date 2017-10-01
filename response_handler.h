#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "parse.h"
#include <time.h>

void handleRequest(char* ,int ,int , char*, char* );
void serve_get(Request* , char*, char*);
void serve_post(Request*, char*);
