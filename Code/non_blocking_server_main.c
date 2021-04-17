#include <stdio.h>
#include<winsock2.h>

#pragma comment(lib, "ws2_32") // windows socket library
#define WSA_VERSION MAKEWORD(2, 2)

#define SOCK_SIZE 20


struct client_informations{
    char *host;
    int port;
};


int SocketWrite(SOCKET sock, fd_set writerset, char *message, int len){
    if(FD_ISSET(sock, &writerset)){
         // cut fraction of buffer
        //char bufsend[] = "say: Hello from server\n",);
        // 會一直跑回圈 要看
        printf("msg: %s\n", message);
        send(sock, message, len,0);
        return 1;           
    }message="";
    return 0;
}
int for_SocketWrite(SOCKET socks[], fd_set writerset, char *message, int len,int times){
    
    for (int i = 0; i < times; i++)
    {
        SocketWrite(socks[i+1],writerset,message,len);
        printf("Send message to sock[%d]\n", i+1);
    }
    
    return 0;
}



int main(void){
    char *message="";
    WSADATA wsa={0};
    

    // initialize
    printf("\nInitialising.....\n");
    if(WSAStartup(MAKEWORD(2,2),&wsa) != 0){
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Initialised\n");

// creating socket
    int nFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(nFd==INVALID_SOCKET){
        printf("Error creating socket, ec:%d\n", WSAGetLastError());
        return -1;
    }

 


// name resolution
    struct sockaddr_in oAddr;
    oAddr.sin_family=AF_INET;
    oAddr.sin_addr.s_addr = INADDR_ANY;
    oAddr.sin_port= htons(8881);


// binding
    if(bind(nFd, (struct sockaddr*)&oAddr, sizeof(oAddr)) == SOCKET_ERROR){
        printf("Error binding sokcet, ec:%d\n", WSAGetLastError());
        closesocket(nFd);
        return -1;
    }

// listening
    if(listen(nFd, SOCK_SIZE) == SOCKET_ERROR){
        printf("Error listening, ec: %d\n", WSAGetLastError());
        closesocket(nFd);
        return -1;
    }

// select

    
    u_long  unblock=1;
    if(ioctlsocket(nFd, FIONBIO, &unblock) == SOCKET_ERROR){
        printf("ioctlsocket(acp) erroe, ec:%d\n", WSAGetLastError());
        return -1;
    }
   
    fd_set ORSet;
    fd_set OWSet;
    FD_ZERO(&ORSet);
    FD_ZERO(&OWSet);
    FD_SET(nFd,&ORSet);
    FD_SET(nFd,&OWSet);
    
    SOCKET socks[64];
    struct client_informations clis[64];

    socks[0]=nFd;
    clis[0].host="127.0.0.1";
    clis[0].port=80;
    int TotalSockets=1;
    
    int c,j=0;
   
    while(1){
        fd_set readerset = ORSet;
        fd_set writerset = OWSet;

    
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int res = select(0,&readerset,&writerset,NULL,&tv);
        if(res == SOCKET_ERROR){
            printf("select error, ec:%d\n", WSAGetLastError());
            break;
        }
            
        if(res==0){
           
            continue; // no connect
        }

        int temTotalSockets = TotalSockets;
         // receive message    
        int i;
        
        for(i=0;i<TotalSockets;i++){
            
            if(FD_ISSET(socks[i], &readerset)){
                // listensocket 可讀

                if(socks[i]==nFd){ // 第一次連線
                    struct sockaddr_in  client;
                    SOCKET acp = accept(nFd, (struct sockaddr *)&client, &c);

                    if(acp==INVALID_SOCKET){
                        printf("accept error, ec:%d\n", WSAGetLastError());
                        break;
                    }

                    if(ioctlsocket(acp, FIONBIO, &unblock) == SOCKET_ERROR){
                        printf("ioctlsocket(acp) erroe, ec:%d\n", WSAGetLastError());
                        break;
                    }

                    FD_SET(acp, &ORSet);
                    FD_SET(acp, &OWSet);
                    //printf("temTotalSockets: %d\n", temTotalSockets);
                    socks[temTotalSockets++]=acp;   
                    
                    // get client address information and save it into struct client_informations 
                    //char* addr_cli = ;
                    clis[j].host = inet_ntoa(client.sin_addr);
                    clis[j].port = client.sin_port;
                    printf("one connection %s:%d by socket[%d] create clis[%d]\n", clis[j].host, clis[j].port,i,j);
                    j++;
                }else{
                    
                    // recv message
                    char buf[1024];
                    int len = recv(socks[i], buf, 1024, 0);
                    

                    if(len==0){
                        
                        printf("No Sleep\n");
                        //break;
                    }else if( len==SOCKET_ERROR){;
                        //printf("Recv error, ec:%d\n", WSAGetLastError());    
                        //break;
                    }else{
                        //char outbuf[len+1];
                        //memcpy(outbuf,buf,len);
                        //outbuf[len] = 0;
                        // '[',']',': '=4
                        int len = strlen(clis[i-1].host) + strlen(buf) + 1 + 4;
                        char msg[len];
                        memset(msg, '\0', len);
                        
                        strcat(msg,"[");
                        strcat(msg,clis[i-1].host);
                        strcat(msg,"]: ");
                        strcat(msg,buf);
                        
                        message = msg;
                        printf("recv: '%s' by socks[%d]\n",msg, i);
                        printf("from %s:%d  \n",clis[i-1].host, clis[i-1].port);


                        for_SocketWrite(socks, writerset, message, strlen(message),j);
                        //free(outbuf); 
                    }
                }
                
            }
           
            
        }
         
        TotalSockets=temTotalSockets;
    }

    
    
    closesocket(nFd);
    WSACleanup();
    return 0;
}