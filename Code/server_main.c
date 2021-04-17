#include <stdio.h>
#include<winsock2.h>

#pragma commnet(lib, "ws2_32.lib") // windows socket library

int main(void){

    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c;
   
    
    // initialize
    printf("\nInitialising.....\n");
    if(WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Initialised\n");

    // Create socket
    if((s=socket(AF_INET,SOCK_STREAM,0))== INVALID_SOCKET){
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Socket created. \n");

    // Prepare th sockaddr_in structrue
    server.sin_family=AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if(bind(s, (struct sockaddr *)&server, sizeof(server))==SOCKET_ERROR){
        printf("Bind failed with error code: %d\n", WSAGetLastError);
        exit(EXIT_FAILURE);
    }

    printf("Bind done\n");

    // Listen to incoming connections
    listen(s, 5);

    printf("Waiting for incoming connections...\n");

    //c=sizeof(struct sockaddr_in);
    while(1){
        
        
        new_socket = accept(s,(struct sockaddr *)&client, &c)
        if(new_socket == INVALID_SOCKET){
            printf("Accept failed with error code: %d\n", WSAGetLastError);
            exit(EXIT_FAILURE);
        } printf("Connection accepted\n");


        // Who
        char *client_addr = inet_ntoa(client.sin_addr);
        printf("[from]  %s:%d\n", client_addr,client.sin_port);

        // Receive message
        char server_reply[2000]; // message to receive
        if( recv(new_socket,server_reply, sizeof(server_reply), 0) == SOCKET_ERROR){
            printf("recv error\n");
        }
        server_reply[2000] = ' '; 
        printf("[content]  %s\n", server_reply);
    

        if(send(new_socket, server_reply, sizeof(server_reply) , 0)<=0){
            printf("send error\n");
            exit(-1);
        }
        //printf("message is sent\n");

    }

    if(new_socket==INVALID_SOCKET){
        printf("accept failed with error code: %d\n", WSAGetLastError);
    }

    closesocket(s);
    WSACleanup();
    return 0;
}