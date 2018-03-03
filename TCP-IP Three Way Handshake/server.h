#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h> 



struct tcp_hdr{
                unsigned short int src;
                unsigned short int des;
                unsigned int seq;
                unsigned int ack;
                unsigned short int hdr_flags;
                unsigned short int rec;
                unsigned short int cksum;
                unsigned short int ptr;
                unsigned int opt;
              };



unsigned short int computeChecksum(unsigned short int *cksum_arr,int num);
void printAll(char *mes,struct tcp_hdr *tcp_seg);
void fileWrite(char *mes,struct tcp_hdr *tcp_seg,char *fileName,char *writeMode);
void createFrame(struct tcp_hdr *tcp_seg,unsigned short int src,unsigned short int des,unsigned int seq,unsigned int ack, unsigned short int hdr_flags,unsigned short int rec,unsigned short int cksum,unsigned short int ptr,unsigned int opt);
int sendFrame(int *sockfd,struct tcp_hdr *tcp_seg);
int reciveFrame(int *sockfd,struct tcp_hdr *tcp_seg);
void unsigned_int_to_binary(char *sendBuff,unsigned int value );
void unsigned_short_int_to_binary(char *sendBuff,unsigned short int value );
unsigned  int  binary_to_unsigned_int(char *bitStr);
unsigned short int  binary_to_unsigned_short_int(char *bitStr);

