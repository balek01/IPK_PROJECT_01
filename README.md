# IPK_PROJECT_1


## Brief
The project implements a client for the IPK Calculator protocol. The program establishes the connection and communicates with the server in the way specified in the assignment. According to the selected method, the data entered by the user is sent to the server. The program then receives the server response and prints it to the standard output. 

## UDP
UDP communication is implemented in the following steps. First, a socket is created on which the communication with the remote server will take place. The socket contains information about the version of the IP address and the type of communication(UDP/TCP). Then the message to be sent is read from the standard input. This message is sent to the IP address and port specified by the user when the program is started. The program then expects a response. There is no connection during UDP communication and no guarantee of message delivery.  Therefore, if a response from the server is not received within five seconds, an error message is printed to error output. If the message is received, its contents are printed to the standard output. Messages are sent in binary form.

## TCP
TCP communication first creates a socket in the same way as UDP. However, a connection is then established. The client sends a message requesting communication(HELLO), then the server replies with an acknowledgement. After the communication exchange, the connection is closed by sending and receiving a message (BYE). In the event of an unexpected termination of the connection on the client side by a C-c signal, the signal is delayed to allow the TCP connection to be properly terminated. Messages are sent in text form.

## Tests
### UDP 
#### UDP server offline
Sends request to server that is offline. After 5 seconds prints error.

```
./ipkcpc -h 127.0.0.70  -p 2023 -m udp
( * 2 2 )
ERROR: Receive: Resource temporarily unavailable
```


#### UDP too long input
For testing purposes max allowed length has been changed to 4 characters. 

```
./ipkcpc -h 127.0.0.70  -p 2023 -m udp
12345
Max input is length 4 characters!
 ```

#### UDP Incorect input format
Sends incorectly formated message to server. Prints error message. 

```
./ipkcpc -h 127.0.0.70  -p 2023 -m udp
random msg 
ERR:Could not parse the message
```

#### UDP Test all correct operations
Sends multiple corectly formated message to server. Prints response messages. 

```
./ipkcpc -h 127.0.0.70  -p 2023 -m udp
( - 2 22 )
OK:-20
( * 2 -32 ) 
OK:-64
( / 2 0 )
ERR:Could not parse the message
( / 2 2 )
OK:1
( + 333 33)
OK:366
```

#### UDP Combination of correct and incorrect messages

```
./ipkcpc -h 127.0.0.70  -p 2023 -m udp
( * 2 22)
OK:44
random
ERR:Could not parse the message
random
ERR:Could not parse the message
( + 2 2 2 )
OK:6
```

### TCP
#### TCP server not running
Sends request to server that is offline. Prints error.

```
./ipkcpc -h 127.0.0.70  -p 2023 -m tcp
ERROR: connection: : Connection refused
```

#### TCP incorrect format
Sends incorectly formated message to server. 

```
./ipkcpc -h 127.0.0.70  -p 2023 -m tcp
HELLO 
HELLO
random
BYE
BYE
```

#### TCP multiple correct
Sends multiple corectly formated messages to server. Prints response.

```
./ipkcpc -h 127.0.0.70  -p 2023 -m tcp
HELLO
HELLO
SOLVE ( * 2 2 ) 
RESULT 4
SOLVE ( + 2 2 )
RESULT 4
SOLVE ( / 2 2 )
RESULT 1
SOLVE ( - 2 2 )
RESULT 0
BYE
BYE
```

#### TCP interupt
Catches C-c signal and terminate connection correctly.

```
./ipkcpc -h 127.0.0.70  -p 2023 -m tcp
HELLO 
HELLO
^C
BYE
BYE
```

#### TCP combination of correct and incorrect messages

```
./ipkcpc -h 127.0.0.70  -p 2023 -m tcp
HELLO
HELLO
SOLVE ( * 2 2 )
RESULT 4
random
BYE
BYE
```

## Author
- [Miroslav Bálek](https://git.fit.vutbr.cz/xbalek02/)

## References:
RYSAVY, Ondrej. Used code from. NESFIT: IPK-Projekty [online]. Brno: Brno University of Technology, 2015 [cit. 2023-03-20]. Available from: https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp