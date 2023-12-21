#include <stdio.h>
#include <stdlib.h>	


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

    printf("Server: %s\n", server);
    printf("File: %s\n", filename);

    return 0;
}
