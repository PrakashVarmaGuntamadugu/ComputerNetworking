#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 


int main(int argc, char *argv[]){

	int sockfd = 0, n = 0;
    	char recvBuff[1024];
	char serverIp[]="127.0.0.1";	// put server ip address 
	char urlBuff[600];
    	struct sockaddr_in serv_addr; 

	if(argc != 2){
		printf("\n Usage: %s <port of server> \n",argv[0]);
		exit(1);
	} 

	printf("url: ");
	fgets (urlBuff, 600, stdin);
	if(urlBuff[strlen(urlBuff)-1]=='\n'){
		urlBuff[strlen(urlBuff)-1]='\0';
	}

	memset(recvBuff, '0',sizeof(recvBuff));
 	memset(&serv_addr, '0', sizeof(serv_addr)); 

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Error : Could not create socket \n");
		exit(1);
	} 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1])); 

	if(inet_pton(AF_INET, serverIp, &serv_addr.sin_addr)<=0){
		printf("\n inet_pton error occured\n");
		exit(1);
	} 

	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("\n Error : Connect Failed \n");
		exit(1);
	} 

	write(sockfd, urlBuff, strlen(urlBuff)); 
	while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0){
		recvBuff[n] = 0;
		if(fputs(recvBuff, stdout) == EOF){
		    printf("\n Error : Fputs error\n");
		}
	} 
        printf("\n\n proxy server connection close... \n");

    return 0;
}