void receive_file(int sockfd, struct sockaddr_in *server_addr, const char *file) {
    char buffer[516];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    ssize_t nbytes;

    nbytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from_addr, &from_len);
    if (nbytes == -1) {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(file, "wb");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fwrite(buffer + 4, 1, nbytes - 4, fp);
    fclose(fp);

    char ack[4] = {0x00, 0x04, buffer[2], buffer[3]};
    if (sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&from_addr, from_len) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
}
