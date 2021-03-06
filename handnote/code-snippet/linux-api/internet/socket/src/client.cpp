#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char *argv[])
{
	int sockfd;
    char recvline[MAXLINE];
    char sendline[MAXLINE];
	struct sockaddr_in servaddr;
	const char *srv_addr = "127.0.0.1";

	//if( argc != 2){
	//	printf("usage: ./client <ipaddress>\n");
	//	exit(0);
	//}

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6666);
	if( inet_pton(AF_INET, srv_addr, &servaddr.sin_addr) <= 0){
		printf("inet_pton error for %s\n", srv_addr);
	    exit(0);
	}

	if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
	    exit(0);
	}

    while (true)
    {
	    printf("send msg to server: \n");
    	fgets(sendline, 4096, stdin);
    	if( send(sockfd, sendline, strlen(sendline), 0) < 0) {
	    	printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		    exit(0);
    	}
        recv(sockfd, recvline, MAXLINE, 0);
        printf("recv msg from server:%s\n", recvline);
    }

	close(sockfd);
	
	return 0;
}
