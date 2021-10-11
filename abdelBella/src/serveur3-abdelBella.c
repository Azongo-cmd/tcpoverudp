#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/select.h>

int main(int argc , char *argv[]){
    /*
    Espace dédié aux déclarations de variables
    Mettre une brève description de chaque variable des qu'on la declare
    Donner des noms signifiants aux variables
    */
   int port; // port de connexion du serveur

    /*

    Espace dedié au code
    Essayer d'etre le plus clair possible
    mettre des commentaires
    */
    
    if(argc >=2){
        port = atoi(argv[1]);	
        printf("Connexion port %d \n", port);
    }else{
        printf("Pas de port specifie ! \n");
        return -1;
    }

    return(0);
}