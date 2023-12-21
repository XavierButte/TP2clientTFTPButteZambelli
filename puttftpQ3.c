#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
     if (argc != 3) {
        fprintf(stderr, "Usage: %s <server> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server = argv[1];
    char *filename = argv[2];
    char service[NI_MAXSERV];
    
    

    
    struct addrinfo hints, *result;
    int sockfd;
    memset(&hints, 0, sizeof hints);       //set hints to 0
    hints.ai_family = AF_UNSPEC;           //ai_family field definition
    hints.ai_socktype = SOCK_DGRAM;        //definition of the socket type used
	hints.ai_flags = AI_PASSIVE;           //for wildcard IP address
	hints.ai_protocol = IPPROTO_UDP;       //any pprotocole
	
	
    printf("Server: %s\n", server);
    printf("File: %s\n", filename);
   
    int status = getaddrinfo(server, TFTP_PORT, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }


        //socket creation
     sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
     if (sockfd == -1) {
         perror("error");
         exit(EXIT_FAILURE);
        }
    printf("socket :%d \n", sockfd);

    freeaddrinfo(result);          //frees space allocated by getaddrinfo()
    close(sockfd);

    return 0;
}
