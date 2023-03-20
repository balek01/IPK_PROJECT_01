# IPK_PROJECT_1


## Brief
The project implements a client for the IPK Calculator protocol. It communicates with the server in the way specified in the assignment. According to the selected method, the data entered by the user is sent to the server. The program then receives the server response and prints it to the standard output. 

## UDP
UDP communication is implemented in the following steps. First, a socket is created on which the communication with the remote server will take place. The socket contains information about the version of the IP addresses and the type of communication(UDP/TCP). Then the message to be sent is read from the standard input. This message is sent to the IP address and port specified by the user when the program is started. The program then expects a response. There is no connection during UDP communication and no guarantee of message delivery.  Therefore, if a response from the server is not received within five seconds, an error message is printed to error output. If the message is received, its contents are printed to the standard output. Messages are sent in binary form.

## TCP
TCP communication first creates a socket in the same way as UDP. However, a connection is then established. The client sends a message requesting communication(HELLO), then the server replies with an acknowledgement. After the communication exchange, the connection is closed by sending and receiving a message (BYE). In the event of an unexpected termination of the connection on the client side by a C-c signal, the signal is delayed to allow the TCP connection to be properly terminated.Messages are sent in text form.

## Author
- [Miroslav Bálek](https://git.fit.vutbr.cz/xbalek02/)

## References:
RYSAVY, Ondrej. Used code from. NESFIT: IPK-Projekty [online]. Brno:, 2015 [cit. 2023-03-20]. Available from: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp