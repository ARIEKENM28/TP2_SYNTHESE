#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "tftp.h"

// Helper function to get server address
void get_server_address(const char *host, struct sockaddr_in *server_addr) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(host, "69", &hints, &res);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(EXIT_FAILURE);
    }

    memcpy(server_addr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
}

// Helper function to create a socket
int create_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

// Function to send RRQ (Read Request)
void send_rrq(int sockfd, struct sockaddr_in *server_addr, const char *file) {
    char buffer[512];
    int len = snprintf(buffer, sizeof(buffer), "\x00\x01%s\x00octet\x00", file);

    if (sendto(sockfd, buffer, len, 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
}

// Function to receive a file in multiple packets
void receive_file_multi(int sockfd, struct sockaddr_in *server_addr, const char *file) {
    char buffer[516];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    ssize_t nbytes;
    FILE *fp = fopen(file, "wb");

    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        nbytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from_addr, &from_len);
        if (nbytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        fwrite(buffer + 4, 1, nbytes - 4, fp);

        char ack[4] = {0x00, 0x04, buffer[2], buffer[3]};
        if (sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&from_addr, from_len) == -1) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        if (nbytes < 516) {
            break;
        }
    }

    fclose(fp);
}

// Main function for tftp_get
void tftp_get(const char *host, const char *file) {
    struct sockaddr_in server_addr;
    get_server_address(host, &server_addr);
    int sockfd = create_socket();

    send_rrq(sockfd, &server_addr, file);

    // For simplicity, we call receive_file_multi to handle both single and multiple packets
    receive_file_multi(sockfd, &server_addr, file);

    close(sockfd);
}
