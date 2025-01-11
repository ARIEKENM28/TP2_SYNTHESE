## TP2_SYNTHESE_INFORMATIQUE: TRANSFERT DE FICHIER EN UTILISANT TFTP(Trivial File Transfer Protocol)

### Introduction
Ce Tp implémente un client TFTP en C pour envoyer et recevoir des fichiers d'un serveur TFTP. Il comprend deux parties :l'envoi de fichiers au serveur TFTP (WRQ - Write Request), la réception de fichiers du serveur TFTP (RRQ - Read Request).

### Fonctionnalités implémentées

1. Extraction des Informations de Requête

 J'ai utilisé la fonction "void get_request_info(int argc, char *argv[], const char **server_name, const char **server_port, const char **filename)" qui permet de passer dynamiquement les informations nécessaires pour la requête, ce qui rend le programme réutilisable à chaque fois que l'on a en besoin.
 Cette fonction extrait les informations du serveur et du fichier à partir des arguments de la ligne de commande.

2. Obtention de l'Adresse du Serveur

J'ai utilisé la fonction "struct addrinfo* get_server_address(const char *server_name, const char *server_port)" car "getaddrinfo" est une méthode standard pour résoudre les noms de domaine et obtenir les informations nécessaires pour établir une connexion réseau.
Cette fonction utilise getaddrinfo pour obtenir les informations et l'adresse du serveur en utilisant le nom du serveur et le port fournis.

3. Création du Socket
J'ai utilisé la fonction "int create_socket()"  car un socket UDP est nécessaire pour échanger des paquets avec le serveur TFTP, qui utilise ce protocole pour le transfert de fichiers.
Cette fonction crée un socket UDP pour la communication avec le serveur.


4. Envoi de Requête WRQ (Write Request)

J'ai utilisé la fonction "struct sockaddr_in send_wrq(int sockfd, struct addrinfo *server_info, const char *filename, const char *mode)" car La requête WRQ (opcode 2) est utilisé pour demander l'autorisation d'écrire un fichier sur le serveur TFTP.
Cette fonction crée et envoie une requête WRQ au serveur pour envoyer un fichier, et attend l'accusé de réception initial du serveur.

5. Envoi du Fichier au Serveur
 
 J'ai utilisé la fonction "void send_file(int sockfd, struct sockaddr_in *server_addr, const char *filename)"

Description : Cette fonction lit le fichier local par blocs et envoie chaque bloc au serveur, tout en attendant l'accusé de réception (ACK) pour chaque bloc envoyé. Elle permet diviser le fichier en blocs et attendre l'ACK pour chaque bloc permet de s'assurer que toutes les parties du fichier sont correctement reçues par le serveur.


6. Envoi de Requête RRQ (Read Request)
J'ai utilisé la fonction "void send_rrq(int sockfd, struct addrinfo *server_info, const char *filename, const char *mode)" car La requête RRQ (opcode 1) est standard pour demander la lecture d'un fichier à partir du serveur TFTP.
Cette fonction crée et envoie une requête RRQ au serveur pour demander un fichier et attend la réponse initiale du serveur.


7. Réception du Fichier du Serveur
J'ai utilisé la fonction "void receive_file(int sockfd, const char *filename)" .Elle permet de diviser le fichier en blocs et envoyer un ACK pour chaque bloc permet de s'assurer que toutes les parties du fichier sont correctement reçues et écrites localement.
Cette fonction reçoit le fichier par blocs du serveur et écrit dans un fichier local, tout en envoyant un accusé de réception (ACK) pour chaque bloc reçu.



8. Gestion des Erreurs
J'ai utilisé la fonction "void error(const char *message) et void erreur(const char *message)"
Ces fonctions affichent un message d'erreur et arrêtent le programme en cas d'erreur.
Une gestion des erreurs robuste est essentielle pour identifier et résoudre les problèmes rapidement et efficacement.

### Commandes utilisées pour compiler le code
Pour exécuter le programme, utilisez les commandes suivantes dans leterminal :

Pour envoyer un fichier au serveur (WRQ) :
"./client_tftp <server_name> <server_port> <filename>"

Pour recevoir un fichier du serveur (RRQ) :
"./client_tftp <server_name> <server_port> <filename>"



 

