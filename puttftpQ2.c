#include <stdio.h>
#include <stdlib.h>	
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


	status= getaddrinfo(server, NULL, &hints, &servinfo);
    if ( status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }
    
    
    freeaddrinfo(servinfo);          //frees space allocated by getaddrinfo()


    return 0;
}
