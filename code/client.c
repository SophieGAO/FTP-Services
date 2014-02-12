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
#define BUFSIZE 100
#define MAXBUF 1024
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

int main(int argc, char **argv)
{
 	int sockfd;
  	struct sockaddr_in server_addr;
  	struct hostent *host;
  	int portnumber;
  	char msg[BUFSIZE];
  	int nbytes;
  	char RECVbuffer[MAXBUF];
	char REDYbuffer[BUFSIZE];
	char ACKbuffer[BUFSIZE];
 	if(argc!=3)
 	{
 	 	fprintf(stderr,"Usage:%s hostname portnumber\a\n",argv[0]);
  		exit(1);
 	}
 
 	if((portnumber=atoi(argv[2]))<0)
 	{
       	 	fprintf(stderr,"Usage:%s hostname portnumber\a\n",argv[0]);
        	exit(1);
 	}
 
 	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
 	{
        	fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
        	exit(1);
 	}

 	bzero(&server_addr,sizeof(server_addr));
 	server_addr.sin_family=AF_INET;
 	server_addr.sin_port=htons(portnumber);
 	server_addr.sin_addr.s_addr=inet_addr(argv[1]);
 	//server_addr.sin_addr.s_addr=INADDR_ANY;

 	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
	{
  		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
  		exit(1);
	}

//Input the user name
   	printf("Please input your user name:\n");	
	memset(msg,0,sizeof(msg));
   	gets(msg);

        if(send(sockfd,msg,strlen(msg),0)==-1)
        {
         	fprintf(stderr,"Send Error:%s\n",strerror(errno));
         	exit(1);
        }

	memset(RECVbuffer,0,sizeof(RECVbuffer));
	if((nbytes=recv(sockfd,RECVbuffer,BUFSIZE,0))==-1)
	{
       	 	fprintf(stderr,"Receive Error:%s\n",strerror(errno));
        	exit(1);
 	}
	else
	{
		printf("%s\n",RECVbuffer);		
	}

//Input the password		
	printf("Please input your password:\n");	
	memset(msg,0,sizeof(msg));
   	gets(msg);

	if(send(sockfd,msg,strlen(msg),0)==-1)
        {
         	fprintf(stderr,"Send Error:%s\n",strerror(errno));
         	exit(1);
        }

	memset(RECVbuffer,0,sizeof(RECVbuffer));
	if((nbytes=recv(sockfd,RECVbuffer,BUFSIZE,0))==-1)
	{
       	 	fprintf(stderr,"Receive Error:%s\n",strerror(errno));
        	exit(1);
 	}
	else
	{
		printf("%s\n",RECVbuffer);
	}

//Change directory		
	printf("Input the directory you want to change:\n");	
	memset(msg,0,sizeof(msg));
   	gets(msg);

	if(send(sockfd,msg,strlen(msg),0)==-1)
        {
         	fprintf(stderr,"Send Error:%s\n",strerror(errno));
         	exit(1);
        }

	memset(RECVbuffer,0,sizeof(RECVbuffer));
	if((nbytes=recv(sockfd,RECVbuffer,BUFSIZE,0))==-1)
	{
       	 	fprintf(stderr,"Receive Error:%s\n",strerror(errno));
        	exit(1);
 	}
	else
	{
		printf("");
	}

	system(msg);

while(1)
{
//Input the requested file name		
	printf("Please input your requested and file name:\n");	
	memset(msg,0,sizeof(msg));
   	gets(msg);

	if(send(sockfd,msg,strlen(msg),0)==-1)
        {
         	fprintf(stderr,"Send Error:%s\n",strerror(errno));
         	exit(1);
        }

//determine the request command
	char Request[BUFSIZE];
	char ReqFileName[BUFSIZE];
	int i=0;
	int j=0;
	int k=0;
	int record=0;
	memset(Request,0,sizeof(Request));
	memset(ReqFileName,0,sizeof(ReqFileName));
	for(i;i<strlen(msg);i++)
	{
		if(msg[i]!=' ')
		{
			Request[j]=msg[i];
			j++;
		}
		else
		{
			record=i;
			//printf("The request type is:%s\n",Request);
			break;
		}
	}

	int l=record+1;
	for(l;l<strlen(msg);l++)
	{
		ReqFileName[k]=msg[l];
		k++;
	}

	memset(msg,0,sizeof(msg));

	
	if((strcmp(Request,"PUT"))==0)
	{
//open the file and send it to the server 
		memset(REDYbuffer,0,sizeof(REDYbuffer));
		if((nbytes=recv(sockfd,REDYbuffer,BUFSIZE,0))==-1)
		{
       	 		fprintf(stderr,"Receive Error:%s\n",strerror(errno));
        		exit(1);
 		}

		if((strcmp(REDYbuffer,"READY"))==0)
		{
		int size;
		char buf[MAXBUF];
		FILE *fd2;
		fd2=fopen(ReqFileName,"r");
		if(fd2==NULL)
		{printf("Open file %s is fail!\n",ReqFileName);}
		else 
		{printf("Open file %s is success!\n",ReqFileName);}

		char sdCount[BUFSIZE];
		memset(sdCount,0,sizeof(sdCount));
		long int fileLength;
		long int countNum;

		fileLength = getFileLength(fd2);

		countNum= fileLength / 1024;
		if(fileLength % 1024 > 0)
		countNum++;	
		snprintf(sdCount, sizeof(sdCount), "%ld",countNum);
		send(sockfd, sdCount, sizeof(sdCount), 0);

		memset(buf,0,sizeof(buf));

		int i;
		int rd;
		for(i = 0; i <countNum; i++)
		{

			rd=fread(buf,1,1024,fd2);		
			rd=send(sockfd,buf,rd,0);
			memset(buf,0,MAXBUF);
		}//end of sending message!
		printf("end of sending message!\n");
	
		fclose(fd2);
	 }//end of if ready
	else
		{printf("not ready\n");}
		continue;	
	}
	else if((strcmp(Request,"GET"))==0)
	{
//client get file from the server, create file and write
		char rvCount[BUFSIZE];
		memset(rvCount, 0, sizeof(rvCount));
		nbytes=0;
		nbytes=	recv(sockfd,rvCount, sizeof(rvCount), 0);
		long int countNum= atoi(rvCount);

		FILE *fd1;
		char filename[]="copy_get";

		fd1=fopen("copy_get","w+");
		if(fd1==NULL)
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
		for(i = 0; i < countNum; i++)
		{		
			rb=recv(sockfd,RECVbuffer,MAXBUF,0);

			fwrite(RECVbuffer,rb,1,fd1);
		
			memset(RECVbuffer,0,sizeof(RECVbuffer));
		}//end of while(nbytes)

		printf("write success!\n");

		fclose(fd1);
		continue;
	}//end of GET
	else if((strcmp(Request,"QUIT"))==0)
	{
		printf("Log out success!\n");
		exit(0);
	}
	
			
}//end of while(1)
	close(sockfd);
 	exit(1);

}
