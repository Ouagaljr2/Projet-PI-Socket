#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#define PORT 9600
#define TAILLE 1024
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define ATTENTE 10
#define WELCOME_MSG_SIZE 1024

struct Client {
    int sockfd;
    char nom[TAILLE];
};

int main(int argc, char **argv) {

    int sockfd;
    socklen_t fromlen = sizeof(struct sockaddr_in); 
    socklen_t addrlen = sizeof(struct sockaddr_in); 
    struct sockaddr_in my_addr, client_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);



    if (sockfd < 0) {
        
        printf("Erreur lors de la création de la socket \n");
        fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(PORT);

    char hostname[1024];
    if (gethostname(hostname, 1024) != 0) {
        perror("Erreur lors de l'obtention du nom de la machine \n");
        fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement

    } else {
    }

    if(bind(sockfd, (struct sockaddr *)&my_addr, addrlen) != 0)
    {
        perror("erreur lors de l'appel a bind -> ");
        fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement

      exit(-2);
    }

    listen(sockfd, ATTENTE);

    printf ("Serveur demmarer. En attente de connexions...\n");
    fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement


    printf("Nom de la machine serveur: %s\n", hostname);
    fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement




    fd_set read_fds;
    int max_sd, activity;
    int client_sockets[MAX_CLIENTS] = {0};
    struct Client clients[MAX_CLIENTS];

    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    max_sd = sockfd;

    while (1) {
        fd_set tmp_fds = read_fds;

        activity = select(max_sd + 1, &tmp_fds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            perror("Erreur de sélection");
            fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement

            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sockfd, &tmp_fds)) {
            int new_socket;
            int client_addr_len = sizeof(client_addr);
            new_socket = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
            if (new_socket < 0) {
                perror("Acceptation de connexion échouée");
                fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement

                exit(EXIT_FAILURE);
            }

            FD_SET(new_socket, &read_fds);
            if (new_socket > max_sd) {
                max_sd = new_socket;
            }

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;

                    // Recevoir le nom du client
                    recv(new_socket, clients[i].nom, TAILLE, 0);
                    strcpy(clients[i].nom, strtok(clients[i].nom, "\n")); // Supprimer le retour à la ligne
                    clients[i].sockfd = new_socket;
                    printf("%s s'est connecté, socket fd est %d, IP : %s, port : %d\n",clients[i].nom, new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement

                    char* welcome_msg="Bienvenu dans le serveur.";
                    //sprintf(welcome_msg, "Bonjour %s\n", clients[i].nom);
                    send(new_socket, welcome_msg, strlen(welcome_msg), 0);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &tmp_fds)) {
                char buffer[BUFFER_SIZE] = {0};
                int valread = read(sd, buffer, BUFFER_SIZE);

                if (valread == 0) {
                    printf("%s s'est déconnecté, socket fd est %d\n",clients[i].nom, sd);
                    fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement

                    // lorsqu'un client se deconnect on notifie les autres
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int dest_sd = client_sockets[j];
                        if (dest_sd > 0 && dest_sd != sd) {
                            char message[TAILLE + BUFFER_SIZE + 20]; // Taille suffisante pour contenir le nom du client, le message et le texte supplémentaire
                            snprintf(message, sizeof(message), "%s s'est déconnecté: %s", clients[i].nom, buffer);

                            send(dest_sd, message, strlen(message), 0);
                        }
                    }
                    

                    close(sd);
                    FD_CLR(sd, &read_fds);
                    client_sockets[i] = 0;
                }
                else {
                    // pour afficher un message recu d'un client
                    printf("Reçu du client %s: %s\n", clients[i].nom, buffer);
                    fflush(stdout); // Force le vidage du buffer stdout pour s'assurer que le message est bien envoyé immédiatement


                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int dest_sd = client_sockets[j];
                        if (dest_sd > 0 && dest_sd != sd) {
                            char message[TAILLE + BUFFER_SIZE + 20]; // Taille suffisante pour contenir le nom du client, le message et le texte supplémentaire
                            sprintf(message, "%s a envoyé : %s", clients[i].nom, buffer);
                            send(dest_sd, message, strlen(message), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
