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
 * 
 *   Last modified: Mar 20, 2023
 */

#ifndef IPKCPC_H
#define IPKCPC_H

int OP_CODE_REQ = 0;
int OP_CODE_RES=1;
int STAT_CODE_ERR =1;
int STAT_CODE_OK=0;
bool DEBUG = false;
int HEAD_SIZE_REQ = 2;
int HEAD_SIZE_RES = 3;

typedef struct {
    char* host;
    int port;
    char* mode;
} Conn;


/**
 * Prints given string to stderr.
 *
 * @param msg Message to be printed. 
 */
void Debug(char* msg);

/**
 * Parse given arguments int Conn struct.
 *
 * @param argc Count of arguments passed by user. 
 * @param argv Array od arguments.
 */
Conn ParseArgs(int argc, char * const argv[]);

/**
 * Creates socket of give mode for given address.
 *
 * @param is_tcp Creates socket in TCP mode is true, else UDP mode.
 */
void CreateSocket(bool is_tcp);

/**
 * Sets given variable to true if given mode is tcp else udp.
 *
 * @param mode   Expects "udp" | "tcp".
 * @param is_tcp Varable that will be set.
 */
void SetIsTcp(char* mode, bool *is_tcp);

/**
 * Creates tcp connection to given address. 
 *
 * @param conn Struct with credentials of host address.   
 */
void TCP_Connect(Conn conn);

/**
 * Reads from stdin to given buffer
 *
 * @param bufin Bufer to contain stdin. 
 */
void Read(char *bufin);

/**
 * Sends buffer contents to connected host.
 *
 * @param bufin Bufer to be sent.
 */
void TCP_Send(char *bufin);

/**
 * Reads to buffer contents of recived message.
 *
 * @param buf Bufer to contain recived message. 
 */
void TCP_Receive(char *buf);

/**
 * Prints content of buffer to stdout.
 *
 * @param buf Bufer to be printed.
 */
void TCP_PrintBuf(char *buf);

/**
 * Sends BYE to host, and prints sended and recived BYE to stdout.
 *
 * @param expectbye whether or not expect BYE as response.
 */
void TCP_SendBye(bool expectbye);

/**
 * Checks whether user or host send BYE. Returns true if either did.
 *
 * @param bufin user input buffer.
 * @param buf host response buffer.
 * 
 * @return Returns true if either did send bye otherwise false.
 */
bool TCP_CheckForBye(char *bufin, char *buf);

/**
 * Handles C-c signal from user by sending BYE, waitnig for BYE response and enging program.
 *
 * @param sig whether or not expect BYE as response.
 */
void TCP_SigHandler(int sig);


/**
 * Wraper function to run TCP_* functions.
 *
 * @param conn connection info
 */
void TCP_Run(Conn conn);


/**
 * Wraper function to run UDP_* functions.
 *
 * @param conn connection info
 */
void UDP_Run(Conn conn);

/**
 * Creates struct sockaddr_in containing connection information
 *
 * @param conn connection info.
 * 
 * @return struct sockaddr_in containing connection information.
 */
struct sockaddr_in UDP_CreateAddress(Conn conn);

/**
 * Sends contains of given buffer to given host address.
 *
 * @param bufin buffer to be sent.
 * @param serverAddres struct sockaddr_in containing connection information.
 */
void UDP_Send(char *bufin, struct sockaddr_in serverAddress);

/**
 * Expects message from given host address. If message is recived save its contents to buffer. 
 * This function wait only for 5 seconds, then error is printed and program continues.
 *
 * @param buf Bufer to contain recived message. 
 * @param serverAddres struct sockaddr_in containing connection information.
 */
void UDP_Receive(char *buf, struct sockaddr_in serverAddress);

/**
 * Prints contents of buffer to stdout.
 *
 * @param buf Bufer with content to be printed. 
 */
void UDP_PrintBuf(char *buf);

#endif 