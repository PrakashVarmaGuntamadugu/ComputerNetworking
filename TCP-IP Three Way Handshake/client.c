
#include "client.h"


int main(int argc, char *argv[]){

	int sockfd = 0;
	char serverIp[]="129.120.151.94";	
    	struct sockaddr_in serv_addr; 
 	struct tcp_hdr tcp_send_seg;
	struct tcp_hdr tcp_recive_seg;
  	unsigned short int cksum_arr[12];

	unsigned int initialClientSeq=10;	// Assigned random initial Server Sequence number
	unsigned short int clientAppsPort=21334;	// Assigned random TCP client port number(source)
	unsigned short int ServerAppsPort=2200;	// Assigned random TCP server port number(destination)

	if(argc != 2){
		printf("\n Usage: %s <port of server> \n",argv[0]);
		exit(1);
	}  

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

	printf("\nClient connection start..\n");
	printf("\nDemonstrate TCP handshaking\n");	

	//Client send - connection request TCP segment
	createFrame(&tcp_send_seg,clientAppsPort,ServerAppsPort,initialClientSeq,0,0x2332,0,0,0,0);		// call to create sender frame from client to sever
	/*16 bit data offset,reserved section and flags 
		add any 16 bit unsigned short int with Set the SYN bit to 1
		xxxx xxxx xxxx xx1x
		for example 0x2332
	*/
  	memcpy(cksum_arr, &tcp_send_seg, 24); //Copying 24 bytes
	tcp_send_seg.cksum=computeChecksum(cksum_arr,12); 
	printAll("Client send - connection request TCP segment",&tcp_send_seg);			// calling frame print function
	fileWrite("Client send - connection request TCP segment",&tcp_send_seg,"client.out","w");		// calling frame file write function for client.out(write starting on file)
  	sendFrame(&sockfd,&tcp_send_seg);		// calling send frame 
 
	//server responds to the request by creating a connection granted TCP segment
	reciveFrame(&sockfd,&tcp_recive_seg);	// call received frame from server
	printAll("Client recive - server responds to the request by creating a connection granted TCP segment",&tcp_recive_seg);		// call frame print function
	fileWrite("Client recive - server responds to the request by creating a connection granted TCP segment",&tcp_recive_seg,"client.out","a"); 		// call frame file write function for client.out

	//The client responds back with an acknowledgement TCP segment.
	createFrame(&tcp_send_seg,clientAppsPort,ServerAppsPort,initialClientSeq+1,(tcp_recive_seg.seq+1),0x2310,0,0,0,0);		// call crate sender frame from client to sever
  	/*16 bit data offset,reserved section and flags 
		add any 16 bit unsigned short int with Set the ACK bit to 1
		xxxx xxxx xxx1 xxxx
		ex 0x2310
	*/
	memcpy(cksum_arr, &tcp_send_seg, 24); //Copying 24 bytes
	tcp_send_seg.cksum=computeChecksum(cksum_arr,12); 
	printAll("Client send - client responds back with an acknowledgement TCP segment",&tcp_send_seg);		// call frame print function
	fileWrite("Client send - client responds back with an acknowledgement TCP segment",&tcp_send_seg,"client.out","a");		// call frame file write function for client.out
  	sendFrame(&sockfd,&tcp_send_seg);	// call send frame 
 

	printf("\nDemonstrate closing of a TCP connection\n");

	// Create a close request TCP segment
	createFrame(&tcp_send_seg,clientAppsPort,ServerAppsPort,initialClientSeq,0,0x2301,0,0,0,0);		// call crate sender frame from client to sever
	/*16 bit data offset,reserved section and flags 
		add any 16 bit unsigned short int with Set the FIN bit to 1
		xxxx xxxx xxxx xxx1
		ex 0x2301
	*/
  	memcpy(cksum_arr, &tcp_send_seg, 24); //Copying 24 bytes
	tcp_send_seg.cksum=computeChecksum(cksum_arr,12); 
	printAll("Client send - a close request TCP segment",&tcp_send_seg);		// call frame print function
	fileWrite("Client send - a close request TCP segment",&tcp_send_seg,"client.out","a");		// call frame file write function for client.out
  	sendFrame(&sockfd,&tcp_send_seg);	// call send frame 

	//The server responds back with an acknowledgment TCP segment.
	reciveFrame(&sockfd,&tcp_recive_seg);	// call receive frame from server
	printAll("Client recive - server responds back with an acknowledgment TCP segment",&tcp_recive_seg);		// call frame print function
	fileWrite("Client recive - server responds back with an acknowledgment TCP segment",&tcp_recive_seg,"client.out","a");	// call frame file write function for client.out

	//The server again sends another close acknowledgement TCP segment.
	reciveFrame(&sockfd,&tcp_recive_seg);	// call receive frame from server
	printAll("Client recive - server again sends another close acknowledgement TCP segment",&tcp_recive_seg);		// call frame print function
	fileWrite("Client recive - server again sends another close acknowledgement TCP segment",&tcp_recive_seg,"client.out","a");		// call frame file write function for client.out

	//The client responds back with an acknowledgement TCP segment.
	createFrame(&tcp_send_seg,clientAppsPort,ServerAppsPort,initialClientSeq+1,(tcp_recive_seg.seq+1),0x2310,0,0,0,0);		// call crate sender frame from client to sever
  	/*16 bit data offset,reserved section and flags 
		add any 16 bit unsigned short int with Set the ACK bit to 1
		xxxx xxxx xxx1 xxxx
		ex 0x2310
	*/
	memcpy(cksum_arr, &tcp_send_seg, 24); //Copying 24 bytes
	tcp_send_seg.cksum=computeChecksum(cksum_arr,12); 
	printAll("Client send - client responds back with an acknowledgement TCP segment",&tcp_send_seg);		// call frame print function
	fileWrite("Client send - client responds back with an acknowledgement TCP segment",&tcp_send_seg,"client.out","a"); 	// call frame file write function for client.out
  	sendFrame(&sockfd,&tcp_send_seg);	// call send frame 
	
	
	
	close(sockfd);
	printf("\nClient connection clsoe..\n");

    return 0;
}





// calculate checksum function
unsigned short int computeChecksum(unsigned short int *cksum_arr,int num){
	
	unsigned int i,sum=0, cksum;
	for (i=0;i<num;i++)               // Compute sum
		sum = sum + cksum_arr[i];

	cksum = sum >> 16;              // Fold once
	sum = sum & 0x0000FFFF; 
	sum = cksum + sum;

	cksum = sum >> 16;             // Fold once more
	sum = sum & 0x0000FFFF;
	cksum = cksum + sum;

	/* XOR the sum for checksum */
	cksum=(0xFFFF^cksum);

	return cksum;
}

// print all filed on one of frame data function
void printAll(char *mes,struct tcp_hdr *tcp_seg){
	printf("%s\n",mes);
	printf("Source port		-	0x%04X\n", tcp_seg->src); 	//print source port on terminal
	printf("Destination port	-	0x%04X\n", tcp_seg->des);	//  print destination port terminal
	printf("Sequence number		-	0x%08X\n", tcp_seg->seq);	// print sequence number terminal
	printf("Acknowledgement number  -	0x%08X\n", tcp_seg->ack);	// print acknowledgement number terminal
	printf("Header length & flags	-	0x%04X\n", tcp_seg->hdr_flags);	// print data offset,reserved section and flags terminal
	printf("Receive window		-	0x%04X\n", tcp_seg->rec);	// print window for flow control terminal
	printf("Checksum		-	0x%04X\n", tcp_seg->cksum);		// print checksum terminal
	printf("Urgent data pointer 	-	0x%04X\n", tcp_seg->ptr);	// print urgent data pointer terminal
	printf("Options			-	0x%08X\n", tcp_seg->opt);		// print Options terminal
	
	return;
}

// write into the file all filed on frame .
void fileWrite(char *mes,struct tcp_hdr *tcp_seg,char *fileName,char *writeMode){
	FILE *fp;
	// open file file
	if ((fp=fopen(fileName,writeMode))==NULL){
		printf("Error! log file open error");
		exit(1);         
	}
	fprintf(fp,"\n***********************************************************************************************************\n");
	fprintf(fp, "%s\n",mes);
	fprintf(fp, "Source port		-	0x%04X\n", tcp_seg->src); 	// write source port into the file 
	fprintf(fp, "Destination port		-	0x%04X\n", tcp_seg->des);	// write destination port into the file 
	fprintf(fp, "Sequence number		-	0x%08X\n", tcp_seg->seq);	// write sequence number into the file 
	fprintf(fp, "Acknowledgement number  	-	0x%08X\n", tcp_seg->ack);	// write acknowledgement number into the file 
	fprintf(fp, "Header length & flags	-	0x%04X\n", tcp_seg->hdr_flags);	// write data offset,reserved section and flags into the file 
	fprintf(fp, "Receive window		-	0x%04X\n", tcp_seg->rec);	// write window for flow control into the file 
	fprintf(fp, "Checksum			-	0x%04X\n", tcp_seg->cksum);	// write checksum into the file 
	fprintf(fp, "Urgent data pointer 	-	0x%04X\n", tcp_seg->ptr);	// write urgent data pointer into the file 
	fprintf(fp, "Options			-	0x%08X\n", tcp_seg->opt);	// write Options into the file 
	
	fclose(fp);
	return;
}

// change frame filed struct function 
void createFrame(struct tcp_hdr *tcp_seg,unsigned short int src,unsigned short int des,unsigned int seq,unsigned int ack, unsigned short int hdr_flags,unsigned short int rec,unsigned short int cksum,unsigned short int ptr,unsigned int opt){
	
	tcp_seg->src =src;	// assign source port into the data struct
	tcp_seg->des = des;	// assign destination port into the data struct
	tcp_seg->seq = seq;		// assign sequence number  into the data struct
	tcp_seg->ack = ack;	// assign acknowledgement number  into the data struct
	tcp_seg->hdr_flags = hdr_flags;	// assign data offset,reserved section and flags into the data struct
	tcp_seg->rec = rec;		// assign receive window for flow control into the data struct
	tcp_seg->cksum = cksum;		// assign checksum into the data struct
	tcp_seg->ptr = ptr;		// assign urgent data pointer into the data struct
	tcp_seg->opt = opt;		// assign Options into the data struct

	return;
}

//convert int to binary and send data function 
int sendFrame(int *sockfd,struct tcp_hdr *tcp_seg){
	char sendBuff[1024];
	bzero(sendBuff, 1024);
	sendBuff[0]='\0';
	unsigned_short_int_to_binary(sendBuff,tcp_seg->src );	//convert  16 bit source port
	unsigned_short_int_to_binary(sendBuff,tcp_seg->des );	//convert  16 bit destination port
	unsigned_int_to_binary(sendBuff,tcp_seg->seq );		//convert  32 bit  sequence number 
	unsigned_int_to_binary(sendBuff,tcp_seg->ack );		//convert  32 bit acknowledgement number 
	unsigned_short_int_to_binary(sendBuff,tcp_seg->hdr_flags );		//convert  16 bit data offset,reserved section and flags
	unsigned_short_int_to_binary(sendBuff,tcp_seg->rec );		//convert  16 bit receive window for flow control 
	unsigned_short_int_to_binary(sendBuff,tcp_seg->cksum );		//convert  16 bit  checksum
	unsigned_short_int_to_binary(sendBuff,tcp_seg->ptr );		//convert  16 bit urgent data pointer
	unsigned_int_to_binary(sendBuff,tcp_seg->opt );		//convert  32 bit Options
	write(*sockfd, sendBuff, strlen(sendBuff)); 		// send the frame
	printf("\nSending frame...........\n");
	printf("%s\n",sendBuff);	//print in terminal sending frame bit..
	return 1;
}

//receive binary data and convert binary to unsigned int function 
int reciveFrame(int *sockfd,struct tcp_hdr *tcp_seg){
	int n = 0;
	char reciveBuff[40];
	bzero(reciveBuff, 40);
	printf("\nReceiving frame...........\n");
	n=read(*sockfd,reciveBuff, 16); // read 16 bit source port
	reciveBuff[n]='\0';	
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(16 bit source port)..
	tcp_seg->src =binary_to_unsigned_short_int(reciveBuff); 
	n=read(*sockfd,reciveBuff, 16);	// read 16 bit destination port
	reciveBuff[n]='\0';
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(16 bit destination port)..
	tcp_seg->des =binary_to_unsigned_short_int(reciveBuff);
	n=read(*sockfd,reciveBuff, 32);	// read 32 bit  sequence number 
	reciveBuff[n]='\0';
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(32 bit sequence number )..
	tcp_seg->seq =binary_to_unsigned_int(reciveBuff);
	n=read(*sockfd,reciveBuff, 32);		// read 32 bit acknowledgement number 
	reciveBuff[n]='\0';
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(32 bit acknowledgement number )..
	tcp_seg->ack =binary_to_unsigned_int(reciveBuff);
	n=read(*sockfd,reciveBuff, 16);		// read 16 bit data offset,reserved section and flags
	reciveBuff[n]='\0';
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(16 bit data offset,reserved section and flags)..
	tcp_seg->hdr_flags =binary_to_unsigned_short_int(reciveBuff);
	n=read(*sockfd,reciveBuff, 16);		// read 16 bit receive window for flow control 
	reciveBuff[n]='\0';
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(16 bit window for flow control)..
	tcp_seg->rec =binary_to_unsigned_short_int(reciveBuff);
	n=read(*sockfd,reciveBuff, 16);			// read 16 bit  checksum
	reciveBuff[n]='\0';
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(16 bit checksum)..
	tcp_seg->cksum =binary_to_unsigned_short_int(reciveBuff);
	n=read(*sockfd,reciveBuff, 16);			// read 16 bit urgent data pointer
	reciveBuff[n]='\0';
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(16 bit urgent data pointer)..
	tcp_seg->ptr =binary_to_unsigned_short_int(reciveBuff);
	n=read(*sockfd,reciveBuff, 32);			// read 32 bit Options
	reciveBuff[n]='\0';
	printf("%s",reciveBuff);	//print in terminal Receiving frame bit(32 bit Options)..
	tcp_seg->opt =binary_to_unsigned_int(reciveBuff);
	
	return 1;
}

// converting unsigned int to binary string function
void unsigned_int_to_binary(char *sendBuff,unsigned  int value ){
   	const int SHIFT = 8 * sizeof( unsigned ) - 1;
   	const unsigned MASK = 1 << SHIFT;
	char b[36];
	bzero(b, 36);
    	b[0] = '\0';
	int i;
	//calculate binary string
	for (i = 1; i <= SHIFT + 1; i++ ) {
		strcat(b, ( value & MASK ? "1" : "0" ));
		value <<= 1;
	}//add to send buffer
	strcat(sendBuff,b);
    	return ;
}

// convert unsigned short int to binary string function
void unsigned_short_int_to_binary(char *sendBuff,unsigned short int value ){
	const int SHIFT = 8 * sizeof( unsigned short) - 1;
	const unsigned short MASK = 1 << SHIFT;
	char b[36];
	bzero(b, 36);
	b[0] = '\0';
	int i;
	//calculate binary string
	for (i = 1; i <= SHIFT + 1; i++ ) {
		strcat(b, ( value & MASK ? "1" : "0" ));
		value <<= 1;
	}
	//add to send buffer
	strcat(sendBuff,b);
	return ;
}

// convert binary string to unsigned int value function
unsigned  int  binary_to_unsigned_int(char *bitStr){
	unsigned  int value=0;
	int i;
	int k=31;
	//calculate unsigned int value
	for (i = 0; i <= 32 ; i++ ) {
		if(bitStr[i]=='1'){
			value=value+pow(2,k);
		}
		k--;
	}
    	return value;
}

// convert binary string to unsigned short int value function
unsigned short int  binary_to_unsigned_short_int(char *bitStr){
	unsigned  int value=0;
	int i;
	int k=15;
	//calculate unsigned short int value
	for (i = 0; i <= 15 ; i++ ) {
		if(bitStr[i]=='1'){
			value=value+pow(2,k);
		}
		k--;
	}
    	return value;
}