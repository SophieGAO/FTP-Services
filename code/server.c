#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <time.h>

#define WAITBUF 10
#define MAXBUF 1024
#define BUFSIZE 100
int system(const char* string);

long getFileLength(FILE *stream)
{
   long int flen;
   long int FileLength;

   flen= ftell(stream);
   fseek(stream, 0L, SEEK_END);
   FileLength = ftell(stream);
   fseek(stream, flen, SEEK_SET);
   return FileLength;
}

static void handle_request(int new_fd)
{
	int nbytes;
 	char USER[BUFSIZE];
	char ACKUSER[BUFSIZE];
	char PSWD[BUFSIZE];
	char ACKPSWD[BUFSIZE];
	char CWD[BUFSIZE];
	char ACKCWD[BUFSIZE];
	char REQUEST[BUFSIZE];
	char ACKREQUEST[BUFSIZE];
  	char RECVbuffer[MAXBUF];     
	char ACK[BUFSIZE];

 //Receive the user name	 
	memset(USER,0,sizeof(USER));
	if((nbytes=recv(new_fd,USER,BUFSIZE,0))==-1)
	{
       	 	fprintf(stderr,"Receive Error:%s\n",strerror(errno));
        	exit(1);
 	}
	else
 	{
		printf("");
	
	}

//Check the user name	
	if(((strcmp(USER,"Chenchen"))!=0)&&((strcmp(USER,"Sophie"))!=0))
	{
		printf("User name does not exist.");
		exit(0);		
	}
    	else
	{
		strcpy(ACKUSER,"User name correct");
		if(send(new_fd,ACKUSER,strlen(ACKUSER),0)==-1)
        	{
       		  	fprintf(stderr,"Service Error:%s\n",strerror(errno));
         		exit(1);
        	}
 	}

//Receive the password	
	memset(PSWD,0,sizeof(PSWD));
	if((nbytes=recv(new_fd,PSWD, BUFSIZE,0))==-1)
	{
       	 	fprintf(stderr,"Receive Error:%s\n",strerror(errno));
        	exit(1);
 	}
	else
 	{
		printf("");
	
	}

//check the password
  	if(((strcmp(PSWD,"1087"))!=0)&&((strcmp(PSWD,"9518"))!=0))
	{                
		printf("Worng password.");
		exit(0);
 	      
	}
	else
	{
		strcpy(ACKPSWD,"Password correct");
		if(send(new_fd,ACKPSWD,strlen(ACKPSWD),0)==-1)
        	{
       		  	fprintf(stderr,"Service Error:%s\n",strerror(errno));
         		exit(1);
        	}	
  	}
 
//change the directory
	memset(CWD,0,sizeof(CWD));
	if((nbytes=recv(new_fd,CWD, BUFSIZE,0))==-1)
	{
       	 	fprintf(stderr,"Receive Error:%s\n",strerror(errno));
        	exit(1);
 	}
	else
 	{
		printf("");
	}

  	system(CWD);

	
		strcpy(ACKCWD,"Directory in server side has change successfully.");
		if(send(new_fd,ACKCWD,strlen(ACKCWD),0)==-1)
        	{
       		  	fprintf(stderr,"Service Error:%s\n",strerror(errno));
         		exit(1);
        	}	
  
while(1)
{	
//check the Request
	char buffer[BUFSIZE];
	char Request[BUFSIZE];
	char ReqFileName[BUFSIZE];
	int i=0;
	int j=0;
	int k=0;
	int record=0;
	memset(Request,0,sizeof(Request));
	memset(ReqFileName,0,sizeof(ReqFileName));
	memset(buffer,0,sizeof(buffer));
//receive request
	if((nbytes=recv(new_fd,buffer,BUFSIZE,0))==-1)
	{
       	 	fprintf(stderr,"Receive Error:%s\n",strerror(errno));
        	exit(1);
 	}
	else
 	{		
		printf("The request is:%s\n\n\n",buffer);	
	}

	for(i;i<strlen(buffer);i++)
	{
		if(buffer[i]!=' ')
		{
			Request[j]=buffer[i];
			j++;
		}
		else
		{
			record=i;
			break;
		}
	}

	int l=record+1;
	for(l;l<strlen(buffer);l++)
	{
		ReqFileName[k]=buffer[l];
		k++;
	}

//determine the request type

	if((strcmp(Request,"PUT"))==0)
	{          	

		memset(ACK,0,sizeof(ACK));
		strcpy(ACK,"READY");
		if(send(new_fd,ACK,strlen(ACK),0)==-1)
        	{
       		  	fprintf(stderr,"Service Error:%s\n",strerror(errno));
         		exit(1);
        	}

		char rvCount[BUFSIZE];
		memset(rvCount,0,sizeof(rvCount));
		nbytes=0;
		nbytes=recv(new_fd,rvCount,sizeof(rvCount),0);

		long int countNum = atoi(rvCount);
	
		FILE *fd2;
		char filename[]="copy_put";

		fd2=fopen("copy_put","w+");
		if(fd2==NULL)
		{
			printf("Creat file %s has failed!\n",filename);
			exit(1);
		}
		else
		{ 
			printf("Creat file %s has success!\n",filename);
					
		}
			
		long int i;
		int rb;
		memset(RECVbuffer,0,sizeof(RECVbuffer));
		
		for(i = 0; i <countNum; i++)
		{
			rb=recv(new_fd,RECVbuffer,MAXBUF,0);
			fwrite(RECVbuffer,rb,1,fd2);		
			memset(RECVbuffer,0,sizeof(RECVbuffer));
		}//end of while(nbytes)

		printf("write success!\n");

		fclose(fd2);
		continue;
	}//end of PUT	
  	else if((strcmp(Request,"GET"))==0)
	{                	 	
//File transmission
		int size;
		char buf[MAXBUF];
		FILE *fd1;
		fd1=fopen(ReqFileName,"r");
		if(fd1==NULL)
		{printf("Open file %s is fail!\n\n\n",ReqFileName);}
		else 
		{printf("Open file %s is success!\n\n\n",ReqFileName);}

		char sdCount[BUFSIZE];
		memset(sdCount,0,sizeof(sdCount));
		long int fileLength;
		long int countNum;

		fileLength = getFileLength(fd1);

		countNum = fileLength/1024;
		if((fileLength % 1024)> 0)
		countNum++;	
		snprintf(sdCount, sizeof(sdCount), "%ld", countNum);
		send(new_fd, sdCount, sizeof(sdCount), 0);

		memset(buf,0,sizeof(buf));

		int i;
		int rd;
		for(i = 0; i < countNum; i++)
		{
			rd=fread(buf,1,1024,fd1);
			rd=send(new_fd,buf,rd,0);		
			memset(buf,0,MAXBUF);
		}//end of sending message!
		printf("end of sending message!");
	
		fclose(fd1);	
 	        continue;
	}//end of GET
	else if((strcmp(Request,"QUIT"))==0)
	{
		printf("Log out success!\n");
		exit(0);
	}
}//end of while(1)
        close(new_fd);	


}

static void handle_connect(int sockfd)
{
	int new_fd;
	socklen_t sin_size; 
	struct sockaddr_in client_addr;
  	sin_size=sizeof(struct sockaddr_in);

     while(1)
     {  
        if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1)
        {
                fprintf(stderr,"Accept error:%s\n\a",strerror(errno));
                exit(1);
    	}
	
	if(new_fd>0)
	{
		if(fork()>0)
		{
			close(new_fd);
		}
		else
		{
			handle_request(new_fd);
			return(0);
		}			
	}
    }  

}

int main(int argc, char *argv[])
{
 int sockfd;
 struct sockaddr_in server_addr;
 
 int portnumber;  

 portnumber=3333;

 if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)  
 {
        fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
        exit(1);
 }

 bzero(&server_addr,sizeof(struct sockaddr_in));
 server_addr.sin_family=AF_INET;
 server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
 server_addr.sin_port=htons(portnumber);
 
 if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
 {
        fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
        exit(1);
 }
 
 if(listen(sockfd, WAITBUF)==-1)
 {
        fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
        exit(1);
 }

 printf("Waiting for client request:\n");
      
 handle_connect(sockfd);
	
	
      close(sockfd);
      exit(1);
}
