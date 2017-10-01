#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <unistd.h>
#include "response_handler.h"

#define BUF_SIZE 8192

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char* argv[])
{
    int HTTP_PORT = atoi(argv[1]);
    char *path = argv[2];

    printf(path);

    //int sock, client_sock;
    //ssize_t readret;
    socklen_t addrlen;
    struct sockaddr_in addr, remoteaddr;
    char buf[BUF_SIZE];

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    //struct sockaddr_storage remoteaddr; // client address

    int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

    //int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j;

	//struct addrinfo hints, *ai, *p;

    fprintf(stdout, "----- HTTP Server -----\n");

    /* all networked programs must create a socket */
    if (( listener = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
    }

	// get us a socket and bind it
	addr.sin_family = AF_INET;
    addr.sin_port = htons(HTTP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* servers bind sockets to ports---notify the OS they accept connections */
    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)))
    {
        //close_socket(listener);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }


    if (listen(listener, 5))
    {
        //close_socket(listener);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }

    // add the listener to the master set
    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
					newfd = accept(listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
							inet_ntop(remoteaddr.sin_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        int readRet = read(listener,buf,8192);
                        Request *request = parse(buf,readRet,listener);
                        //Just printing everything
                        printf("Http Method %s\n",request->http_method);

                        char *response = malloc(200000);
                        handleRequest(buf,readRet, listener, response, path);

                        strcat(response, "\r\n");
                        printf("Printing Resposne before sending it to client:\n");
                        printf(response);

                        //sending the http response to the client
                        if (send(i, response, strlen(response), 0) == -1) {
                          printf("Error sending\n");

                        }else{
                          printf("SENT RESPONSE!");
                        }
                        //free the allocated memory for response
                        free(response);
                        free(request);

                    }
                }
            }
        }
    }
 return 0;
}

                        //When request is over
                        




