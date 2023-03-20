/**
 * 
 * @file ipkcpc.c
 * 
 * @brief Project: IPK23 Calculator Protocol
 *
 * @author xbalek02 Miroslav Bálek
 * 
 * Note: This project is both UPD and TCP communication significantly inspired by 
 * code written by Ondrej Rysavy (rysavy@fit.vutbr.cz)
 * SRC: 
 * - https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoUdp/client.c
 * - https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoUdp/client.c
 * 
 *  Last modified: Mar 20, 2023
 * 
 * Known problem: when udp msg is longer then 255;
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "ipkcpc.h"

#define BUFSIZE 1024


volatile sig_atomic_t flag = 0;
int client_socket;
socklen_t serverlen;


void TCP_SigHandler(int sig){
    flag = 1; 
    printf("\n");
    TCP_SendBye(true);
    //close connection
    shutdown(client_socket,SHUT_RDWR);
    close(client_socket);
    exit(0);
}

Conn ParseArgs(int argc, char *const  argv[]){
    int opt;
    Conn conn;
    conn.port=-1;


    while ((opt = getopt(argc, argv, "h:p:m:")) != -1) {
        switch (opt) {
            case 'h':
                conn.host = optarg;
                break;
            case 'p':
                conn.port = atoi(optarg);
                break;
            case 'm':
                conn.mode = optarg;
                break;
            default:
                fprintf(stderr,"Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (conn.host == NULL || conn.port == -1 || conn.mode == NULL) {
        fprintf(stderr, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(conn.mode, "udp") != 0 && strcmp(conn.mode, "tcp") != 0) {
        fprintf(stderr, "Error: Invalid mode. Mode must be udp or tcp.\n");
        exit(EXIT_FAILURE);
    }

    return conn;
}



void CreateSocket(bool is_tcp){
    int type;
    int family = AF_INET; // ipv4
  
  if (is_tcp){
    Debug("In tcp");
     type = SOCK_STREAM; // tcp

  }else{
    Debug("In udp");
     type = SOCK_DGRAM; // udp
  }
   
    client_socket= socket(family,type,0); // create ipv4 socket of given type
    if (client_socket <= 0){
        fprintf(stderr,"ERROR: could not create socket");
        exit(EXIT_FAILURE);
    }
} 

void SetIsTcp(char* mode, bool *is_tcp){
    if(strcmp(mode,"tcp") == 0) *is_tcp = true;
    
}

void TCP_Connect(Conn conn){
    Debug("Creating Connection");
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // ipv4
    serverAddress.sin_port = htons(conn.port);// set port
    serverAddress.sin_addr.s_addr = inet_addr(conn.host); // set host ip address

    // try to establish connection
    if (connect(client_socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) !=0)
    {
		perror("ERROR: connection: ");
		exit(EXIT_FAILURE);        
    }
  
    Debug("Connected");
}

void Read(char *bufin){

    bzero(bufin, BUFSIZE);// refresh buffer
    if(fgets(bufin,BUFSIZE, stdin) == NULL){
        perror("ERROR in read");
    }

}

void TCP_Send(char *bufin){
    int bytestx;
    // try to send buffer to connected host
    bytestx = send(client_socket, bufin, strlen(bufin), 0);
    if (bytestx < 0) 
      perror("ERROR in sendto");
}

void TCP_Receive(char *buf){
    int bytesrx;
    bzero(buf, BUFSIZE);// refresh buffer
    bytesrx = recv(client_socket, buf, BUFSIZE, 0);  // recive response  to buffer
    if (bytesrx < 0) 
    perror("ERROR in recvfrom");
}

void TCP_PrintBuf(char *buf){  
        printf("%s", buf);  
}

bool TCP_CheckForBye(char *bufin, char *buf){
    bool sendbyeback = true;
    
    if((strcmp("BYE\n",bufin)==0)){ // user is sending BYE
        sendbyeback=false;
        return false;
    }

    if((strcmp("BYE\n",buf)==0)){// user recived BYE
        if(sendbyeback){
        TCP_SendBye(false);
        }
        return false;
    }
    return true;
}

void TCP_SendBye(bool expectbye){
    char buf[BUFSIZE];
    char bufin[BUFSIZE]="BYE\n";
    bzero(buf, BUFSIZE);

    TCP_Send(bufin); // send BYE
    TCP_PrintBuf(bufin); // print client buff (BYE) 
    if(expectbye){ //wether or not to expect response
        TCP_Receive(buf);
        TCP_PrintBuf(buf);
    }
}


void TCP_Run(Conn conn){
    bool repeat = true; // repeat until bye is send of C-c
    char buf[BUFSIZE]; // recived msg buffer 
    char bufin[BUFSIZE];// user input buffer
    CreateSocket(true);
    TCP_Connect(conn);
   
    while(repeat){
        Read(bufin);

        TCP_Send(bufin);
        TCP_Receive(buf);
 
        TCP_PrintBuf(buf);
        repeat = TCP_CheckForBye(bufin,buf);
        if (flag){ // C-c recived from user
            return;
        }
    }
}

struct sockaddr_in UDP_CreateAddress(Conn conn){
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // ipv4
    serverAddress.sin_port = htons(conn.port); // port
    serverAddress.sin_addr.s_addr = inet_addr(conn.host);//  host ip address

    return serverAddress;
}



void UDP_Send(char * bufin, struct sockaddr_in serverAddress){
    
    int flags = 0;
    int len = strlen(bufin); // user input length
    char output[len+HEAD_SIZE_REQ]; // array larger by size of metadata
    // metadata
    output[0] = 0; 
    output[1] = len;  
    
    // -1 ignores newline
    memcpy(&output[HEAD_SIZE_REQ], bufin, len);
    serverlen = sizeof(serverAddress);
    int bytestx=  sendto(client_socket, output, len+HEAD_SIZE_REQ, flags, (struct sockaddr *) &serverAddress, serverlen);
    if (bytestx < 0) 
      perror("ERROR: sendto");
       
    
}

void UDP_Receive(char *buf, struct sockaddr_in serverAddress){
    
    int flags = 0;
    struct timeval timeout;
    timeout.tv_sec = 5; 
    timeout.tv_usec = 0;

    // Create 5 sec timeout to socket 
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("ERROR: setting socket timeout");
    }
    
    //read recived msg to buffer
    bzero(buf,BUFSIZE);
    int bytesrx =  recvfrom(client_socket, buf, BUFSIZE, flags, (struct sockaddr *) &serverAddress, &serverlen);
    if (bytesrx < 0) {
      perror("ERROR: Receive");
    }
    
}

void UDP_PrintBuf(char *buf){
        // opcode is response
        if(buf[0] == OP_CODE_RES){
            //status code is error
            if(buf[1] == STAT_CODE_ERR){
                printf("ERR:%s\n",&buf[HEAD_SIZE_RES]);
            }else{
                //status code is ok
                printf("OK:%s\n",&buf[HEAD_SIZE_RES]);
            }
        }else{
        //recv request
            fprintf(stderr,"Received request being ignored.\n");
        }

  
    
}
void UDP_Run(Conn conn){

    char buf[BUFSIZE]; // server response
    char bufin[BUFSIZE]; // user input
    struct sockaddr_in serverAddress;
    

    CreateSocket(false);
    while(true){
        serverAddress=UDP_CreateAddress(conn);
        Read(bufin);
        UDP_Send(bufin,serverAddress);
        UDP_Receive(buf,serverAddress);
        UDP_PrintBuf(buf);
    }
}


int main (int argc,  char * const  argv[]) {

    bool is_tcp = false;
    Conn conn = ParseArgs(argc, argv);
   
    SetIsTcp(conn.mode, &is_tcp);
    if (is_tcp){
        signal(SIGINT, TCP_SigHandler);
        TCP_Run(conn);
    }else{
        UDP_Run(conn);
    }
    
    shutdown(client_socket,SHUT_RDWR); // prevent socket from sending and reciving 

    if (close(client_socket) == -1){// close socket
        perror("ERROR: closing socket");
    } 
    return 0;
}

void Debug(char* msg){
if(DEBUG){
    fprintf(stderr,"DEBUG: %s\n",msg);
}
}