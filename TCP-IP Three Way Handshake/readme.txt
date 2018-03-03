                              Programming assignment 4

Objective :

Demonstrate TCP 3-way handshake and closing a TCP connection using a client-server
architecture.

Requirements:

1. Create a C based client-server architecture using sockets

2. The server should run on cse01.cse.unt.edu machine and the client should run on
cse02.cse.unt.edu machine

3. The client should be able to communicate with the server using TCP segments

4. The client should be able to demonstrate a 3-way TCP handshake with the server

5. The client should be able to demonstrate closing a TCP connection.


Procedure and result :

1. A server program is created and complied  on cse01.cse.unt.edu machine.

   gcc -Wall -o server server.c server.h -lm (command used to compile)

2.A client program is created and compiled on  cse02.cse.unt.edu machine.

   gcc -Wall -o client client.c client.h -lm 


3.On client side, we assign random a initial Server Sequence number, random TCP client port number(source) and Assigned random TCP server port number(destination).

4. Connection request is sent from client side to server side to initiate a connection and on server side we receive this request and server responds 
   by creating a connection granted TCP segment.

5.The client responds back with an acknowledgement TCP segment ( ACK set to 1) and then the server responds back with an acknowledgment TCP segment. 
  The server again sends another close acknowledgement TCP segment.


6. Then we close the connection.

In the zip file,

client.c represents client program

client.h is a file that represents header lines used in client program

client.out represents output on client side.

server.c represents server program

server.h is a file that represents header lines used in server program

server.out represents output on server side.