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

#define BUFSIZE 1024

// TOOD : fix ctrl c in udp 
volatile sig_atomic_t flag = 0;
int client_socket;
socklen_t serverlen;

typedef struct {
    char* host;
    int port;
    char* mode;
} Conn;

void Debug(char* msg);
Conn parseArgs(int argc, const char * argv[]);
void CreateSocket(Conn conn, bool is_tcp);
void SetIsTcp(char* mode, bool *is_tcp);
void TCP_Connect(Conn conn);
void Read(char *bufin);
void TCP_Send(char *bufin);
void TCP_Recieve(char *buf);
void PrintBuf(char *buf,bool server);
void TCP_SendBye(bool expectbye);
bool TCP_CheckForBye(char *bufin, char *buf);
void TCP_SigHandler(int sig);
void TCP_Run(Conn conn);

void UDP_Run(Conn conn);
struct sockaddr_in UDP_CreateAddress(Conn conn);
void UDP_Send(char *bufin, struct sockaddr_in serverAddress);
void UDP_Recieve(char *buf, struct sockaddr_in serverAddress);

void TCP_SigHandler(int sig){
    flag = 1;
    printf("\n");
    TCP_SendBye(true);
    shutdown(client_socket,SHUT_RDWR);
    close(client_socket);
    exit(0);
}

Conn parseArgs(int argc, const char * argv[]){
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



int GetIpFromDNS(Conn conn){
  /*
    struct hostent *server;
      if ((server = gethostbyname(conn.host)) == NULL) {
        fprintf(stderr,"ERROR: no such host as %s\n", conn.host);
        exit(EXIT_FAILURE);
    }
    conn.host=server;*/
}

void CreateSocket(Conn conn, bool is_tcp){
    int type;
    int family = AF_INET;
  if (is_tcp){

      Debug("In tcp");
     type = SOCK_STREAM;

  }else{
      Debug("In udp");
     type = SOCK_DGRAM;
  }
   
    client_socket= socket(family,type,0);
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
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(conn.port);
    serverAddress.sin_addr.s_addr = inet_addr(conn.host);


    if (connect(client_socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) !=0)
    {
		perror("ERROR: connection: ");
		exit(EXIT_FAILURE);        
    }
  
    Debug("Connected");
}

void Read(char *bufin){
    bzero(bufin, BUFSIZE);
    printf("Please enter msg: ");
    if(fgets(bufin,BUFSIZE, stdin) == NULL){
        perror("ERROR in read");
    }

}

void TCP_Send(char *bufin){
    int bytestx;
    bytestx = send(client_socket, bufin, strlen(bufin), 0);
    if (bytestx < 0) 
      perror("ERROR in sendto");
}

void TCP_Recieve(char *buf){
    int bytesrx;
    bzero(buf, BUFSIZE);
    bytesrx = recv(client_socket, buf, BUFSIZE, 0);
    if (bytesrx < 0) 
    perror("ERROR in recvfrom");
}

void PrintBuf(char *buf,bool server){
    if (server){
        printf("Response: '%s'", buf);
    }else{
        printf("Your msg: '%s'", buf);
    }
    
}
bool TCP_CheckForBye(char *bufin, char *buf){
    bool sendbyeback = true;
    
    if((strcmp("BYE\n",bufin)==0)){
        sendbyeback=false;
        return false;
    }

    if((strcmp("BYE\n",buf)==0)){
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

    TCP_Send(bufin);
    PrintBuf(bufin,false);
    if(expectbye){
    TCP_Recieve(buf);
    PrintBuf(buf,true);
    }
}


void TCP_Run(Conn conn){
    bool repeat = true;
    char buf[BUFSIZE];
    char bufin[BUFSIZE];
    CreateSocket(conn, true);
    TCP_Connect(conn);
   
    while(repeat){
    Read(bufin);

    TCP_Send(bufin);
    TCP_Recieve(buf);
 
    PrintBuf(buf,true);
    repeat = TCP_CheckForBye(bufin,buf);
       if (flag){
        return;
    }
    }
}

struct sockaddr_in UDP_CreateAddress(Conn conn){
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(conn.port);
    serverAddress.sin_addr.s_addr = inet_addr(conn.host);
    return serverAddress;
}



void UDP_Send(char * bufin, struct sockaddr_in serverAddress){
    
    int flags = 0;
    int len = strlen(bufin);
    char output[len+2];
    
    output[0] = 0; 
    output[1] = len;  
    
    
    strncpy(&output[2], bufin, len-1);
    serverlen = sizeof(serverAddress);
    int bytestx=  sendto(client_socket, output, len+2, flags, (struct sockaddr *) &serverAddress, serverlen);
    if (bytestx < 0) 
      perror("ERROR: sendto");
       
    
}

void UDP_Recieve(char *buf, struct sockaddr_in serverAddress){
    //TODO: recieve msg prints bad
    int flags = 0;
    char bufs[BUFSIZE];
    bzero(bufs, BUFSIZE);
    int bytesrx=  recvfrom(client_socket, bufs, BUFSIZE, flags, (struct sockaddr *) &serverAddress, &serverlen);
    printf("%s",bufs);
    if (bytesrx < 0) {
      perror("ERROR: recieve");}
    
}
void UDP_Run(Conn conn){
    char buf[BUFSIZE];
    bzero(buf, BUFSIZE);
    char bufin[BUFSIZE];
    struct sockaddr_in serverAddress;
    

    CreateSocket(conn, false);
    Read(bufin);
    serverAddress=UDP_CreateAddress(conn);
    UDP_Send(bufin,serverAddress);
    UDP_Recieve(buf,serverAddress);
    PrintBuf(buf,true);
}


int main (int argc, const char * argv[]) {

    bool is_tcp = false;
   
  
    signal(SIGINT, TCP_SigHandler);

    Conn conn = parseArgs(argc, argv);
    fprintf(stderr,"Mode is %s, ip: %s , port: %d\n", conn.mode, conn.host, conn.port );
    //GetIpFromDNS(conn);
    SetIsTcp(conn.mode, &is_tcp);

    if (is_tcp){
        TCP_Run(conn);
    }else{
        UDP_Run(conn);
    }
    
    shutdown(client_socket,SHUT_RDWR);
    close(client_socket);
    return 0;
}

void Debug(char* msg){
fprintf(stderr,"DEBUG: %s\n",msg);
}