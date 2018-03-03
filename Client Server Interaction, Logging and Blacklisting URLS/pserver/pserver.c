
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <fcntl.h>


#define BUFFER_SIZE 1025
#define USERAGENT "HTMLGET 1.0"


/* create TCP connection with new host function
	hostname - Domain Name for new TCP connection
	port_address - port number for new TCP connection 
	
	return - connection socket discrter
*/
int http_connection(char *hostname, in_port_t port_address){
	struct hostent *hp;
	struct sockaddr_in addr;
	int on = 1;
	int httpSock;     

	// To find ip address from domain name
	if((hp = gethostbyname(hostname)) == NULL){
		herror("host name error");
		exit(1);
	}
	// create a TCP socket  for connecting HTTP Sever 
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port_address);
	addr.sin_family = AF_INET;
	httpSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(httpSock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

	if(httpSock == -1){
		perror("setsockopt");
		exit(1);
	}
	// connected HTTP Server with TCP connection.. 
	if(connect(httpSock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("connect");
		exit(1);

	}
	return httpSock;
}


/* check that domain is block or not function
	domainName - Domain Name for new TCP connection
	requestTime - client request time with YYYYMMDDHHMMSS format
	
	return - if blocked 1 otherwise return 0
*/
int is_block_domain(char *domainName, char *requestTime){
	int is_block=0;
	char *p;
	char domain[300];
	char startTime[40];
	char endTime[40];
	FILE *fpBlo;
	//  Loop to open blacklist.txt file
	if ((fpBlo=fopen("blacklist.txt","r"))==NULL){
		printf("Error! opening file");
		exit(1);        
	}

	// Loop to check domain is blocked or not
	while(!feof(fpBlo)){
		bzero(domain, 300);
		bzero(startTime, 40);
		bzero(endTime, 40);
		fscanf(fpBlo,"%s %s %s\n",domain,startTime,endTime);
		if((p = strstr(domain, "wwww."))) {
			sscanf(domain, "wwww.%99[^\n]", domain);
		}
		endTime[14]='\0';
		// checking domain have block list..
		if(strcmp(domain, domainName)==0){
			// checking client request time between start and end time  ..
			if((strcmp(requestTime,startTime)>=0) && (strcmp(requestTime,endTime)<=0))
			return 1;	// retuen is block
		}
	}
	fclose(fpBlo);
	return is_block;	// retuen is not block
}

/* check that client request url is cached or not  function
	domainName - client request page url
	requestTime - if cached then store cached File name
	
	return - if cached 1 otherwise return 0
*/
int is_cached_url(char *urlBuff, char *cachedFile){
	int is_cashed=0;
	char *p;int tem;
	char url[600];
	char fileUrl[600];
	char fileName[40];
	FILE *fpCach;
	// open cache stored file  
	if ((fpCach=fopen("list.txt","r"))==NULL){
		printf("Error! opening file..");
		exit(1);       
	}
	// remove http,www from url
	bzero(url, 600);
	strcpy(url,urlBuff);
	if((p = strstr(url, "http://"))) {
		sscanf(url, "http://%99[^\n]", url);
	}
	if((p = strstr(url, "www."))) {
		sscanf(url, "www.%99[^\n]", url);
	}
	// checking url is  cached or not
	while(!feof(fpCach)){
		bzero(fileUrl, 600);
		bzero(fileName, 40);
		fscanf(fpCach,"%s %s\n",fileUrl,fileName);
		tem=strlen(fileUrl);
		fileUrl[tem]='\n';
		fileUrl[tem+1]='\0';
		// remove http,www from cached url
		if((p = strstr(fileUrl, "http://"))) {
			sscanf(fileUrl, "http://%99[^\n]", fileUrl);
		}
		if((p = strstr(fileUrl, "www."))) {
			sscanf(fileUrl, "www.%99[^\n]", fileUrl);
		}
		// checking cached
		if(strcmp(url, fileUrl)==0){
			// if exist cached store cached file name
			strcpy(cachedFile,fileName);
			cachedFile[14]='\0';
			return 1;	// retuen is cached url
		}

	}
	fclose(fpCach);
	return is_cashed;	// retuen is not cached url
}

/* check requested page is inappropriate language or not  function
	requestTime - temporary response file name
	
	return - if exist inappropriate language 1 otherwise return 0
*/
int is_inappropriate_page(char *requestTime){
	int is_inappropriate=0;
	char *p;
	char inappropriate[200];
	char line[1000];
	FILE *fpIna;
	FILE *fbCah;
	// open inappropriate language file
	if ((fpIna=fopen("lang.txt","r"))==NULL){
		printf("Error! lang.txt opening error..\n");
		exit(1);        
	}
	// open temporary response file
	if ((fbCah=fopen(requestTime,"r"))==NULL){
		printf("Error! opening error..\n");
		exit(1);        
	}

	// checking has inappropriate language word into the response.
	while(!feof(fpIna)){
		bzero(inappropriate, 200);
		fseek( fbCah, 0, SEEK_SET );
		fscanf(fpIna,"%s\n",inappropriate);
		bzero(line, 1000);
		while (fgets ( line, 1000, fbCah ) != NULL ){ /* read a line */
			if((p = strstr(line, inappropriate))) {
				return 1;	// if exist return 1
			}
			bzero(line, 1000);
		}		
	}
	fclose(fpIna);
	fclose(fbCah);

	return is_inappropriate; // if not inappropriate return 0
}

/* new web response(200 OK) added into the cache file(list.txt)   function
	domainName - client request page url
	requestTime - if cached then store cached File name
	
*/
void add_cache_page(char *urlBuff,char *requestTime){
	FILE *fpCach;
	char tempBuffer[6000];
	char tem[1000];
	char fileUrl[600];
	char fileName[40];
	// open list.txt file
	if ((fpCach=fopen("list.txt","r"))==NULL){
		printf("Error! opening list.txt file error");
		exit(1);       
	}
	// redy to add new web response in to the cache file  
	bzero(tempBuffer, 6000);
	sprintf(tempBuffer,"%s %s\n",urlBuff,requestTime);
	int i=0;
	// get last four cache url with file name
	while(!feof(fpCach)){
		bzero(tem, 1000);
		bzero(fileUrl, 600);
		bzero(fileName, 40);
		fscanf(fpCach,"%s %s\n",fileUrl,fileName);
		sprintf(tem,"%s %s\n",fileUrl,fileName);
		if(i<4 && strlen(tem)>15){
			//redy to add last four cache url with file name
			strcat(tempBuffer, tem);
		}else{
			remove(fileName); // remove other cached file 
		}
		
		i++;	
	}
	fclose(fpCach);
	if ((fpCach=fopen("list.txt","w"))==NULL){
		printf("Error! opening list.txt file error");
		exit(1);       
	}
	// added last five cache url into the list.txt
	fprintf(fpCach, "%s",tempBuffer);
	fclose(fpCach);
	return;
}

/* added log for every client request with status  function
	domainName - client request page url
	status - status of that client request(block, 200ok ,...)
	
*/
int create_log_file(char *url, char *status){
	FILE *fpLog;
	// opean log file
	if ((fpLog=fopen("log.txt","a"))==NULL){
		printf("Error! log file open error");
		exit(1);         
	}
	// append log with url and status
	fprintf(fpLog, "url: %s status: %s\n",url,status);
	fclose(fpLog);
	return 1;
}

/* create time format(YYYYMMDDHHMMSS)  function
	buffer - store format into the char buffer
*/
void time_string_now(char *buffer){
	time_t timer;
    	struct tm* tm_info;
	time(&timer);//get time
	tm_info = localtime(&timer);
    	strftime(buffer, 16, "%Y%m%d%H%M%S", tm_info); //store format
	return;
}


// main function
int main(int argc, char *argv[]){

	int listenfd = 0, connfd = 0;
	int n;
	int webfd;
	char sendBuff[BUFFER_SIZE];
	char urlBuff[600];
	char GETQurey[1200];
	char requestTime[16];
	char cachedFile[30];
	char *p;
	FILE *fbCached;
	char currentDomanname[100],currentPath[500] ;
	int is_block,is_cached,is_inappropriate,page_error;
	struct sockaddr_in serv_addr; 

	if(argc != 2){
        	printf("\n %s <new port of server> \n",argv[0]);
        	exit(1);
    	} 

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1])); 
	// create server TCP socket
	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

	listen(listenfd, 10); // maximum 10 client waiting same time 

	while(1){
		// new client request accept
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		printf("\n***************************************************************\n");
		printf("new client connection start...\n");
		is_block=0,is_cached=0,is_inappropriate=0,page_error=0;		

		// get needed url from client
		bzero(urlBuff, 600);
		n=read(connfd, urlBuff, sizeof(urlBuff)-1);
		urlBuff[n]='\0';
		time_string_now(requestTime);
		printf("client requst url : %s \n",urlBuff);

		//get domain name and file path from url
		if((p = strstr(urlBuff, "http://"))) {
			sscanf(urlBuff, "http://%99[^/]%99[^\n]", currentDomanname, currentPath);
		}else{
			sscanf(urlBuff, "%99[^/]%99[^\n]", currentDomanname, currentPath);
		}	
		if((p = strstr(currentDomanname, "wwww."))) {
			sscanf(currentDomanname, "wwww.%99[^\n]", currentDomanname);
		}
		// generate HTTP request format 
		bzero(GETQurey, 1200);
		sprintf(GETQurey, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n", currentPath,currentDomanname,USERAGENT);

		//check domain is block or not
		if(is_block_domain(currentDomanname,requestTime)){
			is_block=1;
			bzero(sendBuff, BUFFER_SIZE);
			strcpy(sendBuff,"website blocked\n");
			write(connfd, sendBuff, strlen(sendBuff)); 	// send block message to client
			create_log_file(urlBuff,"BLOCK");		// added block log
			printf("domain blocked...\n");
		}

		if(is_block!=1){
			//check url page is cached or not
			if(is_cached_url(urlBuff,cachedFile)){
				is_cached=1;
				fbCached=fopen(cachedFile,"r");
		    		if (!fbCached){
					printf("Error! Cached file open error");
					exit(1);         
				}
				// send url page from cached file to client
				bzero(sendBuff, BUFFER_SIZE);
		    		while (fgets(sendBuff,BUFFER_SIZE, fbCached)!=NULL){
					write(connfd, sendBuff, strlen(sendBuff)); 
					bzero(sendBuff, BUFFER_SIZE);
				}
				fclose(fbCached);
				create_log_file(urlBuff,"OK200");	// added 200OK log
				printf("sent page data from cached file...\n");
			}
		}

		if(is_block!=1 && is_cached!=1){
			// if not blocked and not cached file then get page from web server
			webfd = http_connection(currentDomanname, 80); 	// create TCP HTTP socket
			write(webfd,GETQurey, strlen(GETQurey));  //send HTTP Request
			bzero(sendBuff, BUFFER_SIZE);
				
			n=0;
			while(read(webfd, sendBuff, BUFFER_SIZE - 1) != 0){
				if(n==0){
					// checked response 200OK or not
					if(!(p = strstr(sendBuff, "HTTP/1.1 200 OK")) && !(p = strstr(sendBuff, "HTTP/1.0 200 OK"))) {
						page_error=1;
						create_log_file(urlBuff,"BADRQ");	// added BADRQ log
						printf("sent bad request header...\n");
					}else{
						if ((fbCached=fopen(requestTime,"w"))==NULL){
							printf("Error! Cached file create error");
							exit(1);         
						}
					}
				}
				if(page_error){
					write(connfd, sendBuff, strlen(sendBuff));	// send bad response header to client
				}else{
					fprintf(fbCached, "%s",sendBuff);// temporary store eb response 
				}
				bzero(sendBuff, BUFFER_SIZE);
				n++;
			}
			close(webfd);
			// if not  bad response then checking inappropriate language
			if(page_error!=1){
				fclose(fbCached);
				is_inappropriate=is_inappropriate_page(requestTime);
				// checked inappropriate language
				if(is_inappropriate==1){
					bzero(sendBuff, BUFFER_SIZE);
					strcpy(sendBuff,"inappropriate language blocked\n");	// send inappropriate language blocked to client
					write(connfd, sendBuff, strlen(sendBuff)); 
					create_log_file(urlBuff,"INAPP");		// added inappropriate log 
					remove(requestTime);					// remove temporary file
					printf("inappropriate language blocked...\n");
				}else{
			    		fbCached=fopen(requestTime,"r");
			    		if (!fbCached){
						printf("Error! Cached file open error");
						exit(1);         
					}
					// send web page to client( not inappropriate)
					bzero(sendBuff, BUFFER_SIZE);
			    		while (fgets(sendBuff,BUFFER_SIZE, fbCached)!=NULL){
						write(connfd, sendBuff, strlen(sendBuff)); 
						bzero(sendBuff, BUFFER_SIZE);
					}
					fclose(fbCached);
					create_log_file(urlBuff,"OK200");	//added OK200 log 
					add_cache_page(urlBuff,requestTime);	// added cached
					printf("sent page data from web server...\n");
				}
			}
		}
		close(connfd);
		printf("client connection closed...\n\n");
		
	}
 
	return 0;

}