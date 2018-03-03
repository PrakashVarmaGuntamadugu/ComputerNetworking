Procedure :

1. A server program is created and complied  on cse01.cse.unt.edu machine.

   gcc -Wall -o server server.c server.h -lm (command used to compile)

2.A client program is created and compiled on  cse02.cse.unt.edu machine.

   gcc -Wall -o client client.c client.h -lm 

3. Run the program on server side.
    
   ./server 12255

   Server waits for request from client. 

4. Run the program on client side.
   
   ./client 12255 send.txt

5. We receive the file successfully on to the server side as recive.txt


In the zip folder,

client.c represents client program

client.h is a file that represents header lines used in client program

send.txt represents the file that is sent from client side.

client.out represents output on client side where in we send data in portions of 128 bits each time. 

server.c represents server program

server.h is a file that represents header lines used in server program

server.out represents output on server side where in we receive data in portions of 128 bits each time. 

recive.txt represents the file received on server side. 
