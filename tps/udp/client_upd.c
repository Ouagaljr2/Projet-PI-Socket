/*
* Ce client attend des entrees sur stdin et les
* envoie au serveur auquel il est connecte.
*/
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



int main (int argc, char *argv)
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


    // creation de la socket client
    if ((sockfd_client = socket(AF_INET, SOCK_DGRAM, 0)) > 0) {
        printf("Socket creeer avec Succès \n");

    }else{
            printf("Erreur lors de la creation de la socket \n");

    }

    
    // recuperer le message de l'entree standar
    char message[TAILLE];
    char hostname[TAILLE];

    printf("Entrer votre message a envoyer au serveur\n");
    scanf("%s",message);

    printf("entrer le Hostname du serveur\n");
    scanf("%s",hostname);

    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"Erreur, impossible de trouver le nom d'hote\n");
        exit(1);
    }


 
    // Configuration de l'adresse du serveur
    servaddr.sin_family = AF_INET; // Famille d'adresses Internet
    //servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Adresse IP du serveur
    servaddr.sin_port = htons(PORT); // Port du serveur
    bcopy((char *)server->h_addr_list[0], (char *)&servaddr.sin_addr.s_addr, server->h_length);


    // Envoyer le message via le socket

    socklen_t addrlen=sizeof(struct sockaddr_in);
    //int bytes_envoyes = sendto(sockfd_client, message, TAILLE, 0, NULL, 0);
    int bytes_envoyes = sendto(sockfd_client, message, TAILLE, 0, (struct sockaddr *)&servaddr, addrlen);

    if (bytes_envoyes == -1) {
        printf("Erreur lors de l'envoi du message\n");
    }else {
        printf("Message envoyé avec succès.\n");
    }



    // fermeture de la socket
    int fermeture=close (sockfd_client);

    if (!fermeture ){
        printf("Socket fermer avec succès\n");
    }
    else{
        perror(" Erreur lors de la fermeture de la socket\n");
    }
    
    return 0;
}