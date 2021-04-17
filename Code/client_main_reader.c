#include <stdio.h>
#include<winsock2.h>

#pragma commnet(lib, "ws2_32.lib") // windows socket library

int main(void){
    WSADATA wsa;

    SOCKET s;
    int rc;
    struct sockaddr_in server;

    printf("\nWinsock initializing....\n");
    if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){
        printf("Failes. Error Code: %d\n",WSAGetLastError());
        return 1;
    }

    printf("Initialized\n");

    // Create socket
    if((s=socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET){
        printf("Could not create socket: %d\n", WSAGetLastError());
    }

    printf("Socket created\n");
    
    //設定連線
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8881);

    // Connect to remote server
    rc = connect(s,(struct sockaddr *)&server, sizeof(server));
    if(rc<0){
        printf("connect error\n");
        return 1;
    }

    printf("Connected\n");
    while(1){

        

         // message to send

        char *server_reply; // message to receive
        server_reply = (char *) malloc(sizeof(char)*1024);
        if( (rc = recv(s,server_reply, 1024, 0)) == SOCKET_ERROR){
            printf("recv error\n");
        }
        //server_reply[1024] = ' '; 
        printf(">> %s\n", server_reply);
       // free(server_reply);
       
    }
    
   

    closesocket(s);
    WSACleanup();
    return 0;
}

