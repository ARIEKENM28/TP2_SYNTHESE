#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define BUFFER_SIZE 516
#define DATA_SIZE 512

void erreur(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}
//Extraction des informations du serveur et du fichier
void get_request_info(int argc, char *argv[], const char **server_name, const char **server_port, const char **filename) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_name> <server_port> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    *server_name = argv[1];
    *server_port = argv[2];
    *filename = argv[3];
}
//Obtention de l'adresse du serveur
struct addrinfo* get_server_address(const char *server_name, const char *server_port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int status = getaddrinfo(server_name, server_port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    return res;
}
//Création du socket
int create_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) erreur("Erreur lors de la création du socket");
    return sockfd;
}
//Création et envoi de la requête RRQ pour télécharger le fichier suivi de l'attente de la réponse initiale d'acquittement ACK
void send_rrq(int sockfd, struct addrinfo *server_info, const char *filename, const char *mode) {
    char rrq[BUFFER_SIZE];
    int rrq_len = 2 + strlen(filename) + 1 + strlen(mode) + 1;
    rrq[0] = 0x00;
    rrq[1] = 0x01;
    strcpy(rrq + 2, filename);
    strcpy(rrq + 2 + strlen(filename) + 1, mode);

    if (sendto(sockfd, rrq, rrq_len, 0, server_info->ai_addr, server_info->ai_addrlen) < 0)
        erreur("Erreur lors de l'envoi du RRQ");

    printf("Requête RRQ envoyée pour le fichier : %s\n", filename);
}

void receive_file(int sockfd, const char *filename) {
    char buffer[BUFFER_SIZE];
    FILE *output_file = fopen(filename, "wb");
    if (!output_file) erreur("Erreur lors de la création du fichier local");

    struct sockaddr_in sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    int block_number = 0;

    while (1) {
        int received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender_addr, &sender_len);
        if (received < 0) erreur("Erreur lors de la réception des données");

        uint16_t opcode = ntohs(*(uint16_t *)buffer);
        if (opcode == 3) {
            uint16_t received_block = ntohs(*(uint16_t *)(buffer + 2));
            if (received_block != block_number + 1) {
                fprintf(stderr, "Bloc inattendu reçu : %d\n", received_block);
                break;
            }

            fwrite(buffer + 4, 1, received - 4, output_file);

            char ack[4] = {0x00, 0x04, buffer[2], buffer[3]};
            if (sendto(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&sender_addr, sender_len) < 0)
                erreur("Erreur lors de l'envoi de l'ACK");

            printf("Bloc %d reçu et confirmé.\n", received_block);
            block_number++;

            if (received < BUFFER_SIZE) {
                printf("Dernier bloc reçu. Transfert terminé.\n");
                break;
            }
        } else if (opcode == 5) {
            fprintf(stderr, "Erreur TFTP : %s\n", buffer + 4);
            break;
        }
    }

    fclose(output_file);
}

int main(int argc, char *argv[]) {
    const char *server_name;
    const char *server_port;
    const char *filename;
    const char *mode = "octet";

    get_request_info(argc, argv, &server_name, &server_port, &filename);
    struct addrinfo *server_info = get_server_address(server_name, server_port);
    int sockfd = create_socket();
    send_rrq(sockfd, server_info, filename, mode);
    receive_file(sockfd, filename);

    freeaddrinfo(server_info);
    close(sockfd);
    return 0;
}