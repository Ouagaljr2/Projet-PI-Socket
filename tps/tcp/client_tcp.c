#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define TAILLE 100
#define PORT 9600

int main (int argc, char *argv[])
{
    /*if (argc != 4)
    {
        printf ("Usage : %s adress_ip_serveur numéro_de_portnombre_de_joueur\n ", argv[0]);
        exit (-1);
    }*/

    // descripteur de la socket du client
    int sockfd_client;
    struct sockaddr_in servaddr;
    struct hostent *server;

    // création de la socket client
    if ((sockfd_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur lors de la creation de la socket \n");
        exit(EXIT_FAILURE);
    }
    printf("Socket créée avec succès\n");

    // récupérer le message de l'entrée standard
    char message[TAILLE];
    char hostname[TAILLE];

    printf("Entrer votre message à envoyer au serveur\n");
    scanf("%s",message);

    printf("Entrer le nom d'hôte du serveur\n");
    scanf("%s",hostname);

    // Supprimer le saut de ligne ajouté par fgets
    message[strcspn(message, "\n")] = 0;
    hostname[strcspn(hostname, "\n")] = 0;

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"Erreur, impossible de trouver le nom d'hôte\n");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    servaddr.sin_family = AF_INET; // Famille d'adresses Internet
    servaddr.sin_port = htons(PORT); // Port du serveur
    bcopy((char *)server->h_addr_list[0], (char *)&servaddr.sin_addr.s_addr, server->h_length);

    // Établir une connexion au serveur
    if (connect(sockfd_client, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Erreur lors de la connexion au serveur\n");
        exit(EXIT_FAILURE);
    }
    printf("Connexion au serveur établie\n");

    // Envoyer le message au serveur via le socket
    ssize_t bytes_envoyes = send(sockfd_client, message, strlen(message), 0);
    if (bytes_envoyes < 0) {
        perror("Erreur lors de l'envoi du message\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Message envoyé avec succès.\n");
    }

    // Fermeture de la connexion
    close(sockfd_client);

    printf("Connexion fermée.\n");

    exit(EXIT_SUCCESS);
}
