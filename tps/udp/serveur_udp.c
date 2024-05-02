#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>


/* port d’attache du serveur */
#define PORT 9600
#define TAILLE 1024



int main(int argc, char *argv[])
{
  
  /*/ verification du nombre d'arguments sur la ligne de commande
  if(argc != 2)
    {
      printf("Usage: %s port_local\n", argv[0]);
      exit(-1);
    }*/

 
    /********** variables du serveur **********/
    /*
    * Definir ici la variable socket du serveur
    */
    int sockfd;                   // descripteur de socket

    /*
    * Definir ici la structure d’une socket
    * d’information sur le serveur
    */

    // taille d'une structure sockaddr_in utile pour la fonction recvfrom
    socklen_t fromlen = sizeof(struct sockaddr_in); 
    socklen_t addrlen = sizeof(struct sockaddr_in); 
    struct sockaddr_in my_addr, client_addr;   // structnée 2019-202

    /*
    * Definir ici la structure d’une socket
    * d’information sur le client
    */


    /*
    * Definir ici la taille de la socket du client
    */



    /********** code du serveur **********/
    /*
    * Ouvrir ici le point de communication de
    * la socket du serveur
    */
    // creation de la socket
    sockfd = socket(AF_INET, SOCK_DGRAM,0);
    /*
    * tester si la socket a ete bien creer
    * */

    if(sockfd > 0){
        printf("Socket de reception créee avec succes\n");
    }else{
        printf("Erreur lors de la creation de la socket \n");
    }
    /*
    * Garnir ici la structure de la socket
    * du serveur avec:
    *
    - sa famille
    *
    - sur (la/les)quelle(s) de ses
    *
    adresses elle ecoute
    *
    - sur quel port elle ecoute
    */

    // initialisation de la structure d'adresse du recepteur (pg local)

    // famille d'adresse
    my_addr.sin_family = AF_INET;

    // adresse IPv4 du recepteur
    inet_aton("127.0.0.0.1", &(my_addr.sin_addr));
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // recuperation du port du recepteur
    //my_addr.sin_port = INADDR_ANY; //pour ecouter sur tous les ports;
    //je ne sais pas pourquoi mais il ne marche pas avec le any
    my_addr.sin_port = htons(PORT);


    char hostname[1024];
    if (gethostname(hostname, 1024) != 0) {
        perror("Erreur lors de l'obtention du nom de la machine \n");
    } else {
        printf("Nom de la machine serveur: %s\n", hostname);
    }

 
   
    /*
    * Attacher ici le file-descripteur de la
    * socket a sa structure
    */
    // association de la socket et des param reseaux du recepteur
    const struct sockaddr  addr;

    if(bind(sockfd,( struct sockaddr *)&my_addr,addrlen) != 0)
    {
      perror("erreur lors de l'appel a bind -> ");
      exit(-2);
    }


   
    /*
    * Boucle generale du serveur (infinie)
    * Le serveur ne doit jamais s’arreter en
    * principe (pas d’exit()). Le serveur
    * doit lire a partir de la socket et
    * afficher sur Ècran le message reçu
    * de la part du client.
    */
    do {
        char message[TAILLE];        // espace necessaire pour stocker le message recu

        // reception de la chaine de caracteres
        printf("attente du message sur le port %d\n",PORT);

        if(recvfrom(sockfd,message,TAILLE,0,(struct sockaddr *)&client_addr, &fromlen) == -1)
          {
            perror("erreur de reception -> ");
            exit(-3);
          }

            // affichage de la chaine recue
            printf("vous avez recu le massage %s :\n",message);

    }while(1);

    // fermeture de la socket
    close(sockfd);


    exit(0);
}