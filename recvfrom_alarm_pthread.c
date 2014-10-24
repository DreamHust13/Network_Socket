/*
	使用线程的注意事项：编译时，加上-lpthread；
	线程中的函数，返回值为(void *)而不是void;
	使用pthread_join
*/
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>

int n;
char recvbuf[1024];

void *listen_board();

static void dealSigAlarm(int sigo)
{
	n = -1;
	printf("alarm interrupt!\n");
	return;//just interrupt the recvfrom()
}

void main()
{

	pthread_t pid;
	struct sigaction alarmact;


//	signal(SIGALRM,dealSigAlarm);

	bzero(&alarmact,sizeof(alarmact));
	alarmact.sa_handler = dealSigAlarm;
//	alarmact.sa_flags = SA_RESTART;
	alarmact.sa_flags = SA_NOMASK;
//无论使用哪种设置，都会阻塞在recvfrom函数上

	sigaction(SIGALRM,&alarmact,NULL);

	pthread_create(&pid,NULL,listen_board,NULL);
	pthread_join(pid,NULL);
}
void *listen_board()
{
	int sock;
	struct sockaddr_in fromaddr;
	int len = sizeof(struct sockaddr_in);

	bzero(&fromaddr,len);
	fromaddr.sin_family = AF_INET;
	fromaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	fromaddr.sin_port = htons(9000);

	if((sock = socket(AF_INET,SOCK_DGRAM,0)) == -1 )
	{
		perror("socket create error.\n");
	}

	
	while(1)
	{
		alarm(5);
		n = recvfrom(sock,recvbuf,1024,0,(struct sockaddr *)&fromaddr,&len);
		if(n < 0)
		{
			if(errno == EINTR)
				printf("recvfrom timeout.\n");
			else
				printf("recvfrom error.\n");
		}
		else
			alarm(0);
	}
}
