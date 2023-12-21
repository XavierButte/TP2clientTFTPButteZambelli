#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


int main(int argc, char *argv[]) {
    char *server;
    char *filename;


	//function syntax reminder
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    
    server = argv[1];
    filename = argv[2];
    
    
    struct addrinfo hints, *servinfo, *p;
    int status;
    int sockfd;

    printf("Server: %s\n", server);
    printf("File: %s\n", filename);
    
    
    memset(&hints, 0, sizeof hints);           //set hints to 0
    hints.ai_family = AF_UNSPEC;               //ai_family field definition
    hints.ai_socktype = SOCK_DGRAM; 		   //definition of the socket type used

    status= getaddrinfo(server, NULL, &hints, &servinfo);
    if ( status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }
    
    
     for(p = servinfo; p != NULL; p = p->ai_next) {
        //socket creation
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("client: socket");
            continue;
        }

        break; //if socket creation is successful, exit the loop.
    }
    
        if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }
    
    
    freeaddrinfo(servinfo);          //frees space allocated by getaddrinfo()

    return 0;
}
