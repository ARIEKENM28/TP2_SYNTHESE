#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

// Define constants
#define BUFFER_SIZE 516

// Function prototypes
void parse_arguments(int argc, char *argv[], char **host, char **file);
void get_server_address(const char *host, struct sockaddr_in *server_addr);
int create_socket();
void tftp_put(const char *host, const char *file);

int main(int argc, char *argv[]) {
    char *host;
    char *file;

    // Parse command line arguments
    parse_arguments(argc, argv, &host, &file);

    // Perform TFTP put operation
    tftp_put(host, file);

    return 0;
}

// Function to parse command line arguments
void parse_arguments(int argc, char *argv[], char **host, char **file) {
    if (argc != 3) {
        const char *usage = "Usage: puttftp <host> <file>\n";
        write(STDERR_FILENO, usage, strlen(usage));
        exit(EXIT_FAILURE);
    }
    *host = argv[1];
    *file = argv[2];
}

// Function to get the server address using getaddrinfo
void get_server_address(const char *host, struct sockaddr_in *server_addr) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(host, "69", &hints, &res);
    if (rc != 0) {
        const char *error = "getaddrinfo error\n";
        write(STDERR_FILENO, error, strlen(error));
        exit(EXIT_FAILURE);
    }

    memcpy(server_addr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
}

// Function to create a UDP socket
int create_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        const char *error = "socket error\n";
        write(STDERR_FILENO, error, strlen(error));
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

// Function to perform TFTP put operation
void tftp_put(const char *host, const char *file) {
    struct sockaddr_in server_addr;
    get_server_address(host, &server_addr);
    int sockfd = create_socket();

    char buffer[BUFFER_SIZE];
    int len = snprintf(buffer, sizeof(buffer), "\x00\x02%s\x00octet\x00", file);

    if (sendto(sockfd, buffer, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        write(STDERR_FILENO, "sendto error\n", 13);
        exit(EXIT_FAILURE);
    }
    printf("Sent WRQ for file: %s\n", file);

    FILE *fp = fopen(file, "rb");
    if (fp == NULL) {
        write(STDERR_FILENO, "file open error\n", 16);
        exit(EXIT_FAILURE);
    }

    size_t bytes_read = fread(buffer + 4, 1, BUFFER_SIZE - 4, fp);
    while (bytes_read > 0) {
        len = bytes_read + 4;
        if (sendto(sockfd, buffer, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            write(STDERR_FILENO, "sendto error\n", 13);
            exit(EXIT_FAILURE);
        }
        bytes_read = fread(buffer + 4, 1, BUFFER_SIZE - 4, fp);
    }

    fclose(fp);
    close(sockfd);

    printf("File %s uploaded successfully.\n", file);
}
