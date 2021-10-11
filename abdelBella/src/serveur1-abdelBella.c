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

#define RCVSIZE 1024

int main(int argc , char *argv[]){
    /*
    Espace dédié aux déclarations de variables
    Mettre une brève description de chaque variable des qu'on la declare
    Donner des noms signifiants aux variables
    */
    int port; // port de connexion du serveur
    struct sockaddr_in server_addr, client_addr;
    char buffer[RCVSIZE];
    int server_sfd;
    int utils_client_port = 2000;
    char client_port[4];
    char ack[RCVSIZE] = "ACK";
    char synack[RCVSIZE] = "SYNACK";
    int msg_size;
    int len = sizeof(client_addr);
    int bind_server;

    

    

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

    /*
        Création des sockets server
        Type de retour int
    */
    server_sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_sfd <0){
        perror("Socket creation failed \n");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    

    // filling server_addr params
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // binding the server socket with the address
    bind_server= bind(server_sfd, ( const struct sockaddr *) &server_addr, sizeof(server_addr));
    if(bind_server == -1){
        perror("Error when binding \n");
        return -1;
    }
    
    while (1)
    {
        msg_size = recvfrom(server_sfd, (char *)buffer, RCVSIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
        buffer[msg_size] = '\0';
        printf("Client : %s \n", buffer);
        sprintf(client_port,"%d",utils_client_port);
        strcat(synack, client_port);
        printf("Server1 : %s\n", synack);
        sendto(server_sfd,(char *) synack,strlen(synack),MSG_CONFIRM,(struct sockaddr *) &client_addr,len);
        msg_size = recvfrom(server_sfd, (char *)buffer, RCVSIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
        buffer[msg_size] = '\0';
        printf("Client : %s\n", buffer);
    }
    

    return(0);
}