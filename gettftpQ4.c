#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>


#define BUFFER_SIZE 516  
#define TFTP_PORT "69"  
#define RRQ 1            
#define DAT 3            
#define ACK 4            

//function to send RRQ to the server
void send_rrq(int sockfd, struct addrinfo *p, const char *filename, const char *mode);

//function to receive the file from the server
void receive_file(int sockfd, struct addrinfo *p);

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

    //send RRQ to request a file
    send_rrq(sockfd, result, filename, "octet");
    
    printf("test1");
    //receive the requested file
    receive_file(sockfd, result);

    
    freeaddrinfo(result);          //frees space allocated by getaddrinfo()
    close(sockfd);

    return 0;
}

void send_rrq(int sockfd, struct addrinfo *p, const char *filename, const char *mode) {

    char buffer[BUFFER_SIZE];
    //construct RRQ packet
    printf("test2");

    int len = sprintf(buffer, "%c%c%s%c%s%c", 0x00, RRQ, filename, 0x00, mode, 0x00);
    printf("test3");
    //send RRQ packet
    sendto(sockfd, buffer, len, 0, p->ai_addr, p->ai_addrlen);
}

void receive_file(int sockfd, struct addrinfo *p) {
    char buffer[BUFFER_SIZE];
    struct sockaddr_storage their_addr;
    socklen_t addr_len = sizeof their_addr;
    int numbytes;
    unsigned short block_num = 0;

    while (1) {
        //receive a packet
        numbytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&their_addr, &addr_len);
        if (numbytes < 0) {
            perror("recvfrom");
            exit(1);
        }

        //parse the opcode and block number from the received packet
        unsigned short opcode = (buffer[0] << 8) | buffer[1];
        unsigned short received_block_num = (buffer[2] << 8) | buffer[3];

   
        if (opcode == DAT) {
            printf("Received block %d\n", received_block_num);
            if (received_block_num == block_num + 1) {
                block_num++;


                //send ACK for the received block
                int ack_len = sprintf(buffer, "%c%c%c%c", 0x00, ACK, buffer[2], buffer[3]);
                sendto(sockfd, buffer, ack_len, 0, (struct sockaddr *)&their_addr, addr_len);

                //if the packet is less than 516 bytes, it is last packet
                if (numbytes < 516) {
                    printf("Last block received, file transfer complete.\n");
                    break;
                }
            }
        }
    }
}
