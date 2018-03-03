#include "server.h"
		
// main function
int main(int argc, char *argv[]){

	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr; 

	struct tcp_hdr tcp_send_seg;
	struct tcp_hdr tcp_recive_seg;
	unsigned short int cksum_arr[DATAFRAMESIZEBYTES/2];

	unsigned int initialServerSeq=20;		// assigned random initial Server Sequence number
	unsigned short int clientAppsPort;	
	unsigned short int ServerAppsPort;

	if(argc != 2){
        	printf("\n %s <new port of server> \n",argv[0]);
        	exit(1);
    	} 

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1])); 
	// create server TCP socket
	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(listenfd, 10); // maximum 10 clients waiting at same time 
	printf("\nServer connection start..\n");

	while(1){
		// new client request accept
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		printf("\n***************************************************************\n");
		printf("new client connection start...\n");

		printf("\nDemonstrate TCP handshaking\n");	

		//Server recive - connection request TCP segment
		reciveFrame(&connfd,&tcp_recive_seg,0);	// call receive frame from client
		printAll("Server recive - connection request TCP segment",&tcp_recive_seg,0);	// call frame print function
		fileWrite("Server recive - connection request TCP segment",&tcp_recive_seg,"server.out","w",0);	// call frame file write function for server.out(also append)
		clientAppsPort=tcp_recive_seg.src;
		ServerAppsPort=tcp_recive_seg.des;		
		
		//Server send - The server responds to the request by creating a connection granted TCP segment
		createFrame(&tcp_send_seg,ServerAppsPort,clientAppsPort,initialServerSeq,(tcp_recive_seg.seq+1),0x6012,0,0,0,0); // call crate sender frame from sever to client
	  	/*16 bit data offset,reserved section and flags 
			data offset 6
			add any 16 bit unsigned short int with Set the SYN bit and ACK bit to 1
			xxxx-xxxx xx-xx xxxx
			0110-0000 00-01 0010
			0x6012
		*/
		memcpy(cksum_arr, &tcp_send_seg, 24); //Copying 24 bytes
		tcp_send_seg.cksum=computeChecksum(cksum_arr,12); 
		printAll("Server send - server responds to the request by creating a connection granted TCP segment",&tcp_send_seg,0);		// call frame print function
		fileWrite("Server send - server responds to the request by creating a connection granted TCP segment",&tcp_send_seg,"server.out","a",0);	// call frame file write function for server.out(also append)
	  	sendFrame(&connfd,&tcp_send_seg,0);	// call send frame 
		
		
		//Server receive - The client responds back with an acknowledgement TCP segment
		reciveFrame(&connfd,&tcp_recive_seg,0);	// call receive frame from client
		printAll("Server recive - client responds back with an acknowledgement TCP segment",&tcp_recive_seg,0);		// call frame print function
		fileWrite("Server recive - client responds back with an acknowledgement TCP segment",&tcp_recive_seg,"server.out","a",0);		// call frame file write function for server.out(also append)
		clientAppsPort=tcp_recive_seg.src;
		ServerAppsPort=tcp_recive_seg.des;
	 


		printf("\nDemonstrate recive a text file\n");
		printf("************************************************************************\n\n");	
		
		FILE *textFP;
	
		// open file
		if ((textFP=fopen("recive.txt","w"))==NULL){
			printf("Error! log file open error");
			exit(1);         
		}

		while(1){
			
		  	//Server recive - close request TCP segment from client
			reciveFrame(&connfd,&tcp_recive_seg,128);	// call receive frame from client
	 		if(tcp_recive_seg.hdr_flags!=24577){
				reciveDataPayload(&tcp_recive_seg,textFP);
				printAll("Server receive - receive data TCP segment from client",&tcp_recive_seg,128);		// call frame print function
				fileWrite("Server receive - receive data TCP segment from client",&tcp_recive_seg,"server.out","a",128);	// call frame file write function for server.out(also append)
				clientAppsPort=tcp_recive_seg.src;
				ServerAppsPort=tcp_recive_seg.des;

				createFrame(&tcp_send_seg,ServerAppsPort,clientAppsPort,(tcp_recive_seg.seq),128,0x6010,0,0,0,0);	// call crate sender frame from sever to client
			  	/*16 bit data offset,reserved section and flags 
					data offset 6
					add any 16 bit unsigned short int with Set the SYN bit and ACK bit to 1
					xxxx-xxxx xx-xx xxxx
					0110-0000 00-01 0000
					0x6010
				*/
				memcpy(cksum_arr, &tcp_send_seg, DATAFRAMESIZEBYTES); //Copying 24 bytes
				tcp_send_seg.cksum=computeChecksum(cksum_arr,DATAFRAMESIZEBYTES/2); 
				printAll("Server send - server responds back with an acknowledgment TCP segment",&tcp_send_seg,128);		// call frame print function
				fileWrite("Server send - server responds back with an acknowledgment TCP segment",&tcp_send_seg,"server.out","a",128);		// call frame file write function for server.out(also append)
				sendFrame(&connfd,&tcp_send_seg,128);	// call send frame 

			}else{
				break;
			}

		}
		fclose(textFP);

		printf("\nDemonstrate closing of a TCP connection\n");

		
		//Server receive - close request TCP segment from client
		//reciveFrame(&connfd,&tcp_recive_seg);	// call receive frame from client
		printAll("Server recive - close request TCP segment from client",&tcp_recive_seg,0);		// call frame print function
		fileWrite("Server recive - close request TCP segment from client",&tcp_recive_seg,"server.out","a",0);	// call frame file write function for server.out(also append)
		clientAppsPort=tcp_recive_seg.src;
		ServerAppsPort=tcp_recive_seg.des;
		
		//Server send - The server responds back with an acknowledgment TCP segment.
		createFrame(&tcp_send_seg,ServerAppsPort,clientAppsPort,initialServerSeq,(tcp_recive_seg.seq+1),0x6010,0,0,0,0);	// call crate sender frame from sever to client
	  	/*16 bit data offset,reserved section and flags 
			data offset 6
			add any 16 bit unsigned short int with Set the ACK bit to 1
			xxxx-xxxx xx-xx xxxx
			0110-0000 00-01 0000
			0x6010
		*/
		memcpy(cksum_arr, &tcp_send_seg, 24); //Copying 24 bytes
		tcp_send_seg.cksum=computeChecksum(cksum_arr,12); 
		printAll("Server send - server responds back with an acknowledgment TCP segment",&tcp_send_seg,0);		// call frame print function
		fileWrite("Server send - server responds back with an acknowledgment TCP segment",&tcp_send_seg,"server.out","a",0);		// call frame file write function for server.out(also append)
	  	sendFrame(&connfd,&tcp_send_seg,0);	// call send frame 

		//Server send - The server again sends another close acknowledgement TCP segment
		createFrame(&tcp_send_seg,ServerAppsPort,clientAppsPort,initialServerSeq,(tcp_recive_seg.seq+1),0x6001,0,0,0,0);	// call crate sender frame from sever to client
	  	/*16 bit data offset,reserved section and flags 
			data offset 6
			add any 16 bit unsigned short int with Set the FIN bit to 1
			xxxx-xxxx xx-xx xxxx
			0110-0000 00-00 0001
			0x6001
		*/
		memcpy(cksum_arr, &tcp_send_seg, 24); //Copying 24 bytes
		tcp_send_seg.cksum=computeChecksum(cksum_arr,12); 
		printAll("Server send - server again sends another close acknowledgement TCP segment",&tcp_send_seg,0);		// call frame print function
		fileWrite("Server send - server again sends another close acknowledgement TCP segment",&tcp_send_seg,"server.out","a",0);		// call frame file write function for server.out(also append)
	  	sendFrame(&connfd,&tcp_send_seg,0);		// call send frame 

		//Server receive - The client responds back with an acknowledgement TCP segment.
		reciveFrame(&connfd,&tcp_recive_seg,0);	// call receive frame from client
		printAll("Server recive - client responds back with an acknowledgement TCP segment",&tcp_recive_seg,0);		// call frame print function
		fileWrite("Server recive - client responds back with an acknowledgement TCP segment",&tcp_recive_seg,"server.out","a",0);		// call frame file write function for server.out(also append)

		
		close(connfd);
		printf("\nClient connection clsoe..\n");
		
	}
 
	close(listenfd);
	printf("\nServer clsoe..\n");
	return 0;

}


int reciveDataPayload(struct tcp_hdr *tcp_seg,FILE *textFP){
	int i=0;
	for(i=0;i<128;i++){
		if(tcp_seg->data[i]!=0){
			fputc(tcp_seg->data[i],textFP);
		}
	}
	return 1;
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

// function to print all filed on one of frame data 
void printAll(char *mes,struct tcp_hdr *tcp_seg,int dataBytes){
	int i;
	printf("%s\n",mes);
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("Source port		-	0x%04X\n", tcp_seg->src); 	//print source port on terminal
	printf("Destination port	-	0x%04X\n", tcp_seg->des);	//  print destination port terminal
	printf("Sequence number		-	0x%08X\n", tcp_seg->seq);	// print sequence number terminal
	printf("Acknowledgement number  -	0x%08X\n", tcp_seg->ack);	// print acknowledgement number terminal
	printf("Header length & flags	-	0x%04X\n", tcp_seg->hdr_flags);	// print data offset,reserved section and flags terminal
	printf("Receive window		-	0x%04X\n", tcp_seg->rec);	// print window for flow control terminal
	printf("Checksum		-	0x%04X\n", tcp_seg->cksum);		// print checksum terminal
	printf("Urgent data pointer 	-	0x%04X\n", tcp_seg->ptr);	// print urgent data pointer terminal
	printf("Options			-	0x%08X\n", tcp_seg->opt);		// print Options terminal
	printf("Data			-	");		// print Options terminal
	if(dataBytes!=0){
		for(i=0;i<dataBytes;i++){
			printf("%c",tcp_seg->data[i]);
		}
	}
	printf("\n\n");
	
	return;
}

// write into the file all filed on frame .
void fileWrite(char *mes,struct tcp_hdr *tcp_seg,char *fileName,char *writeMode,int dataBytes){
	int i;
	FILE *fp;
	// open file 
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
	fprintf(fp,"Data			-	");		// print Options terminal
	if(dataBytes!=0){
		for(i=0;i<dataBytes;i++){
			fprintf(fp,"%c",tcp_seg->data[i]);
		}
	}
	fprintf(fp,"\n\n");
	fclose(fp);
	return;
}

// function to change frame filed struct  
void createFrame(struct tcp_hdr *tcp_seg,unsigned short int src,unsigned short int des,unsigned int seq,unsigned int ack, unsigned short int hdr_flags,unsigned short int rec,unsigned short int cksum,unsigned short int ptr,unsigned int opt){
	int i;

	tcp_seg->src =src;	// assign source port into the data struct
	tcp_seg->des = des;	// assign destination port into the data struct
	tcp_seg->seq = seq;		// assign sequence number  into the data struct
	tcp_seg->ack = ack;	// assign acknowledgement number  into the data struct
	tcp_seg->hdr_flags = hdr_flags;	// assign data offset,reserved section and flags into the data struct
	tcp_seg->rec = rec;		// assign receive window for flow control into the data struct
	tcp_seg->cksum = cksum;		// assign checksum into the data struct
	tcp_seg->ptr = ptr;		// assign urgent data pointer into the data struct
	tcp_seg->opt = opt;		// assign Options into the data struct

	
	for(i=0;i<128;i++){
		tcp_seg->data[i]=0;
	}

	return;
}

//function to convert int to binary and send data  
int sendFrame(int *sockfd,struct tcp_hdr *tcp_seg,int dataBytes){
	int i;
	char sendBuff[FRAMESIZE];
	bzero(sendBuff, FRAMESIZE);
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
	if(dataBytes!=0){
		for(i=0;i<dataBytes;i++){
			char_to_binary(sendBuff,tcp_seg->data[i]);
		}
	}
	write(*sockfd, sendBuff, strlen(sendBuff)); 		// send the frame
	printf("Sending frame...........\n");
	printf("%s\n",sendBuff);	//print in terminal sending frame bit..
	return 1;
}

//function to receive binary data and convert binary to unsigned int 
int reciveFrame(int *sockfd,struct tcp_hdr *tcp_seg,int dataBytes){
	int n = 0,i;
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
	if(dataBytes!=0 && tcp_seg->hdr_flags!=24577){
		for(i=0;i<dataBytes;i++){
			n=read(*sockfd,reciveBuff, 8);	
			reciveBuff[n]='\0';
			printf("%s",reciveBuff);
			tcp_seg->data[i]=binary_to_char(reciveBuff);
		}
	}
	printf("\n\n");
	
	return 1;
}

// function to convert unsigned int to binary string 
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

// function to convert unsigned short int to binary string 
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

// function to convert binary string to unsigned int value 
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

// function to convert binary string to unsigned short int value 
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

void char_to_binary(char *sendBuff, char value){
    	const char SHIFT = 8 * sizeof( char) - 1;
	const char MASK = 1 << SHIFT;
	char b[9];
	bzero(b, 9);
	b[0] = '\0';
	int i;
	//calculate binary string
	for (i = 1; i <= SHIFT + 1; i++ ) {
		strcat(b, ( value & MASK ? "1" : "0" ));
		value <<= 1;
	}
	//add to send buffer
	strcat(sendBuff,b);
    	//printf("%s\n", output);
	return;
}

// function to convert binary string to unsigned short int value 
int  binary_to_char(char *bitStr){
	unsigned  int value=0;
	int i;
	int k=7;
	//calculate unsigned short int value
	for (i = 0; i <= 7 ; i++ ) {
		if(bitStr[i]=='1'){
			value=value+pow(2,k);
		}
		k--;
	}
    	return value;
}