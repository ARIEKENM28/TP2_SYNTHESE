void send_rrq(int sockfd, struct sockaddr_in *server_addr, const char *file) {
    char buffer[512];
    int len = snprintf(buffer, sizeof(buffer), "\x00\x01%s\x00octet\x00", file);

    if (sendto(sockfd, buffer, len, 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
}
