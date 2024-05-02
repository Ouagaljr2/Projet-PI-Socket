#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define TAILLE 100
#define PORT 9600



// Fonction pour le thread qui gère les entrées au clavier et les envoie au serveur
void *send_message(void *arg) {
    int sockfd_client = *((int *)arg);
    char buffer[BUFFER_SIZE];

    while (1) {
        // lecture des entrée clavier a envoyer au serveur 
        fgets(buffer, BUFFER_SIZE, stdin);

        // Envoyer le message au serveur
        send(sockfd_client, buffer, strlen(buffer), 0);
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    //descriteur de socket pour le client
    int sockfd_client;

    // structue pour les envoi et receptions
    struct sockaddr_in servaddr;
    struct hostent *server;

    // creation de la socket
    if ((sockfd_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur lors de la creation de la socket \n");
        exit(EXIT_FAILURE);
    }

    // recuperation du nom du client 
    char nom[TAILLE];
    fgets(nom, TAILLE, stdin);
    strtok(nom, "\n"); // Supprimer le retour à la ligne

    // entreer le nom d'hote 
    char hostname[TAILLE];
    fgets(hostname, TAILLE, stdin);
    strtok(hostname, "\n"); // Supprimer le retour à la ligne


    // recheche du serveur par son nom dans le sous reseau
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "Erreur, impossible de trouver le nom d'hôte\n");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    bcopy((char *)server->h_addr_list[0], (char *)&servaddr.sin_addr.s_addr, server->h_length);

    // tentative de connexion au serveur 
    if (connect(sockfd_client, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Erreur lors de la connexion au serveur\n");
        exit(EXIT_FAILURE);
    }
    printf("Connecté au serveur.\n");
    fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement


    // Envoi du nom du client au serveur
    send(sockfd_client, nom, strlen(nom), 0);


    fd_set readfds;
    struct timeval timeout;
    char buffer[BUFFER_SIZE]; // Déclaration de buffer

    // Création du thread pour gérer les entrées au clavier et les envois au serveur
    pthread_t send_thread;
    if (pthread_create(&send_thread, NULL, send_message, (void *)&sockfd_client) != 0) {
        perror("Erreur lors de la création du thread\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Effacer le jeu de descripteurs de fichier et y ajouter sockfd_client
        FD_ZERO(&readfds);
        FD_SET(sockfd_client, &readfds);
        
        // Définir le timeout à 100 millisecondes
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;

        // Attendre jusqu'à ce qu'un descripteur de fichier soit prêt pour la lecture ou que le timeout expire
        int activity = select(sockfd_client + 1, &readfds, NULL, NULL, &timeout);

        if ((activity > 0) && (FD_ISSET(sockfd_client, &readfds))) {
            // Si sockfd_client est prêt pour la lecture
            int valread = recv(sockfd_client, buffer, BUFFER_SIZE, 0);
            if (valread > 0) {
                buffer[valread] = '\0';
                printf("%s\n", buffer);
                fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement

                
                
            }
        }

    }

    close(sockfd_client);
    return 0;
}

