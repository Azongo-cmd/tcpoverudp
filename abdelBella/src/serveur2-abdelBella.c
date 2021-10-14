#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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
    char synack[RCVSIZE] = "SYN-ACK";
    int msg_size;
    int len = sizeof(client_addr);
    int bind_server;
    int bind_datasoc;

    

    

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
        char synack[RCVSIZE] = "SYN-ACK";
        msg_size = recvfrom(server_sfd, (char *)buffer, RCVSIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
        buffer[msg_size] = '\0';
        printf("Client : %s \n", buffer);
        sprintf(client_port,"%d",utils_client_port);
        strcat(synack, client_port);
        
        struct sockaddr_in client_udp;
        int utils_socket;
        char client_buffer[RCVSIZE];
        int client_msg_size;
        int len = sizeof(client_udp);
        char ack[RCVSIZE];
        char seq[6];
        utils_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if(utils_socket <0){
            perror("Socket creation failed \n");
            exit(EXIT_FAILURE);
        }
        memset(&client_udp, 0, sizeof(client_udp));

        client_udp.sin_family = AF_INET;
        client_udp.sin_port = htons(utils_client_port);
        client_udp.sin_addr.s_addr = htonl(INADDR_ANY);

        bind_datasoc= bind(utils_socket, ( const struct sockaddr *) &client_udp, sizeof(client_udp));
        if(bind_datasoc == -1){
            perror("Error when binding 1 \n");
            return -1;
        }
        //printf("Retour socket %d\n", b1);
        
        //msg_size = recvfrom(server_sfd, (char *)buffer, RCVSIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
        

        printf("Server1 : %s\n", synack);
        sendto(server_sfd,(char *) synack,strlen(synack),MSG_CONFIRM,(struct sockaddr *) &client_addr,len);
        msg_size = recvfrom(server_sfd, (char *)buffer, RCVSIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
        buffer[msg_size] = '\0';
        printf("Client : %s\n", buffer);
        
        int fork_val = fork();
        utils_client_port = utils_client_port +1;
        //int fork_val = 0;

        if(fork_val == 0){
            close(server_sfd);
            int WND = 5;
            FILE *fp;
            fd_set set;
            struct timeval t0;
            struct timeval t;
            int wnd = 0;
            int read;
            char packets[WND][RCVSIZE];
            
            int last_ack = 0;

            FD_ZERO(&set);
            int packet_select;
            int wnd_select;
            int sent;

            t0.tv_sec =0;
            t0.tv_usec=0;



            while (1)
            {
                
                client_msg_size = recvfrom(utils_socket, (char *)client_buffer, RCVSIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
                client_buffer[client_msg_size] = '\0';
                printf("Sever Received %d \n", client_msg_size);
                printf("Sever Received %s \n", client_buffer);
                fp = fopen(client_buffer, "r");
                if ( fp == NULL ) {
                    perror( "error" );
                    exit(-1);
                }
                int seq = 1;
                while(!feof(fp)){
                    printf("Window %d \n", wnd);
                    FD_SET(utils_socket,&set);
                    t.tv_sec= 1;
                    t.tv_usec =0;
                    printf("Last ack %d \n", last_ack);
                    if(wnd < WND){
                        sprintf(client_buffer, "%06d", seq);
                        read = fread(buffer,1, RCVSIZE-6,fp);
                        printf("READ: %d\n", read);
                        memcpy(client_buffer+6, buffer,read);
                        memcpy(packets[(seq-1)%WND],client_buffer,RCVSIZE);
                        sent = sendto(utils_socket,(char *) client_buffer,read+6,MSG_CONFIRM,(struct sockaddr *) &client_addr,len);
                        printf("Sever send seq %d \n", seq);
                        wnd = wnd +1;
                        seq = seq +1;
                        printf("Sever send %d \n", sent);
                        if(sent < 0){
                            perror("Sent Error");
                        }     
                    
                        wnd_select = select(utils_socket+1, &set, NULL, NULL, &t0);
                        
                        if(FD_ISSET(utils_socket,&set)){
                            client_msg_size = recvfrom(utils_socket, client_buffer, read+6, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
                            client_buffer[client_msg_size] = '\0';
                            char ack[6];
                            memcpy(ack, client_buffer+3,6);
                            ack[6] = '\0';
                            int diff_ack = atoi(ack) - last_ack;
                            printf("current ack %d \n",atoi(ack));
                            printf("current ack seq %s \n",ack);
                            printf("Diff ack %d \n", diff_ack);

                            if(diff_ack == 0){
                                printf("Retransmission \n");
                                wnd = wnd -1;
                                sent = sendto(utils_socket,(char *) packets[last_ack %5],sizeof(packets[last_ack%5]),MSG_CONFIRM,(struct sockaddr *) &client_addr,len);
                                wnd = wnd +1;
                                char r_seq[6];
                                memcpy(r_seq,packets[last_ack %5],6);
                                r_seq[6] = '\0';
                                printf("Sever resend %d \n", sent);
                                printf("Sever resend %s \n", r_seq);
                            }else{
                                if(diff_ack >0){

                                    wnd = wnd - diff_ack;
                                    last_ack = atoi(ack);
                                }
                            }
                            printf("Client : %s\n", client_buffer);

                        }

                    }else{
                        printf("Fenetre \n");
                        packet_select = select(utils_socket+1, &set, NULL, NULL, &t);
                        printf("temps ecoule \n");
                        if(FD_ISSET(utils_socket,&set)){
                            client_msg_size = recvfrom(utils_socket, client_buffer, read+6, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
                            client_buffer[client_msg_size] = '\0';
                            char ack[6];
                            memcpy(ack, client_buffer+3,6);
                            ack[6] = '\0';
                            int diff_ack = atoi(ack) - last_ack;
                            printf("current ack %d \n",atoi(ack));
                            printf("current ack seq %s \n",ack);
                            printf("Diff ack %d \n", diff_ack);

                            if(diff_ack == 0){
                                printf("Retransmission \n");
                                wnd = wnd -1;
                                sent = sendto(utils_socket,(char *) packets[last_ack%5],sizeof(packets[last_ack%5]),MSG_CONFIRM,(struct sockaddr *) &client_addr,len);
                                wnd = wnd +1;
                                char r_seq[6];
                                memcpy(r_seq,packets[last_ack %5],6);
                                r_seq[6] = '\0';
                                printf("Sever resend %d \n", sent);
                                printf("Sever resend %s \n", r_seq);
                            }else{
                                if(diff_ack >0){

                                    wnd = wnd - diff_ack;
    
                                    last_ack = atoi(ack);
                                }else{
                                    if(diff_ack > 5){

                                    }
                                }
                            }
                            printf("Client : %s\n", client_buffer);
                        }else{
                            printf("Temps ecoulé sans ack \n");
                            printf("Retransmission \n");
                            sent = sendto(utils_socket,(char *) packets[(last_ack +1)%5],sizeof(packets[(last_ack +1)%5]),MSG_CONFIRM,(struct sockaddr *) &client_addr,len);
                            wnd = wnd +1;
                            printf("Sever resend %d \n", sent);
                        }

                    }
                    



                    
                }
                //printf("FIN \n");
                //memset(client_buffer,0,sizeof(client_buffer));
                //memcpy(client_buffer, "FIN", 3);
                char fin[3] = "FIN";
                sent = sendto(utils_socket,(char *) fin,strlen(fin),MSG_CONFIRM,(struct sockaddr *) &client_addr,len);
                if(sent <0){
                    perror("error send de fin");
                }
                printf("Sever send %d \n", sent);
            }
            
            

            //exit(0);


            
        }else{
            close(utils_socket);
        }


    }
    

    return(0);
}