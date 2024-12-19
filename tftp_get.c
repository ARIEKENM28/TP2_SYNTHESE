void tftp_get(const char *host, const char *file) {
    struct sockaddr_in server_addr;
    get_server_address(host, &server_addr);
    int sockfd = create_socket();

    send_rrq(sockfd, &server_addr, file);

    // For simplicity, we call receive_file_multi to handle both single and multiple packets
    receive_file_multi(sockfd, &server_addr, file);

    close(sockfd);
}
