#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_SIZE 516
#define TFTP_PORT "69"
#define WRQ 2
#define DAT 3
#define ACK 4

void send_wrq(int sockfd, struct addrinfo *p, const char *filename, const char *mode);
void send_file(int sockfd, struct addrinfo *p, const char *filename);

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
    send_wrq(sockfd, servinfo, filename, "octet");
    send_file(sockfd, servinfo, filename);

    freeaddrinfo(result);          //frees space allocated by getaddrinfo()
    close(sockfd);
    return 0;
}



void send_wrq(int sockfd, struct addrinfo *p, const char *filename, const char *mode) {
	//construct WRQ packet
    char buffer[BUFFER_SIZE];
    //send WRQ packet
    int len = sprintf(buffer, "%c%c%s%c%s%c", 0x00, WRQ, filename, 0x00, mode, 0x00);
    sendto(sockfd, buffer, len, 0, p->ai_addr, p->ai_addrlen);
}



void send_file(int sockfd, struct addrinfo *p, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Cannot open file");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int numbytes, block_num = 0;
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;

    while (!feof(file)) {
		//read a block of data from the file
        numbytes = fread(buffer + 4, 1, BUFFER_SIZE - 4, file);
        block_num++;

        //prepare DAT packet
        buffer[0] = 0x00;
        buffer[1] = DAT;
        buffer[2] = (block_num >> 8) & 0xFF;
        buffer[3] = block_num & 0xFF;

        //send DAT packet
        sendto(sockfd, buffer, numbytes + 4, 0, p->ai_addr, p->ai_addrlen);

        //wait for ACK
        numbytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&their_addr, &addr_len);
        if (numbytes < 0) {
            perror("recvfrom");
            exit(1);
        }

        //check for ACK
        if (buffer[1] != ACK || buffer[2] != ((block_num >> 8) & 0xFF) || buffer[3] != (block_num & 0xFF)) {
            fprintf(stderr, "Invalid ACK received\n");
            exit(1);
        }
    }

    fclose(file);
}
