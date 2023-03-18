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
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFSIZE 1024


char buf[BUFSIZE];
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

void CreateSocket(Conn conn){
  
  if (strcmp(conn.mode, "udp") == 0){
    Debug("In udp");
    int family = AF_INET;

  }else{
    //TODO: 
  }
} 


int main (int argc, const char * argv[]) {


    Conn conn = parseArgs(argc, argv);
    //GetIpFromDNS(conn);
    CreateSocket(conn);
    
    printf("Mode is %s, ip: %s , port: %d", conn.mode, conn.host, conn.port );

    return 0;
}

void Debug(char* msg){
printf("DEBUG: %s",msg);
}