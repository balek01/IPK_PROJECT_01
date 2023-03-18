/*
 * IPK.2015L
 *
 * Demonstration of trivial TCP communication.
 *
 * Ondrej Rysavy (rysavy@fit.vutbr.cz)
 *
 */
 
#include <stdio.h>
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



int client_socket;


void Debug(char* msg);

typedef struct {
    char* host;
    int port;
    char* mode;
} Conn;

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

void Connect(Conn conn){
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
    if(fgets(bufin, BUFSIZE, stdin) == NULL){
        perror("ERROR in read");
    }
}
void Send(char *bufin){

    int bytestx;
    bytestx = send(client_socket, bufin, strlen(bufin), 0);
    if (bytestx < 0) 
      perror("ERROR in sendto");
}

void Recieve(char *buf){
    int bytesrx;
    bzero(buf, BUFSIZE);
    bytesrx = recv(client_socket, buf, BUFSIZE, 0);
    if (bytesrx < 0) 
    perror("ERROR in recvfrom");
}

void PrintResponse(char *buf){

    printf("Echo from server: '%s'", buf);
}

bool CheckForBye(char *bufin, char *buf){

    //printf("BUFF: %d", (strcmp("BYE\n",buf)));
    if((strcmp("BYE\n",bufin)==0)){
        return false;
    }

    if((strcmp("BYE\n",buf)==0)){
        //TODO: SEND BYE BACK
        return false;
    }
    return true;
}
int main (int argc, const char * argv[]) {

    bool is_tcp = false;
    bool repeat = true;
    char buf[BUFSIZE];
    char bufin[BUFSIZE];

    Conn conn = parseArgs(argc, argv);
    fprintf(stderr,"Mode is %s, ip: %s , port: %d\n", conn.mode, conn.host, conn.port );
    //GetIpFromDNS(conn);
    SetIsTcp(conn.mode, &is_tcp);
    CreateSocket(conn, is_tcp);
    Connect(conn);
   
    while(repeat){
    Read(bufin)
    Send(bufin);
    Recieve(buf);
    PrintResponse(buf);
    repeat = CheckForBye(bufin,buf);

    }
    shutdown(client_socket,SHUT_RDWR);
    close(client_socket);
    return 0;
}

void Debug(char* msg){
fprintf(stderr,"DEBUG: %s\n",msg);
}