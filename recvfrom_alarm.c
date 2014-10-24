/*
	用alarm()函数设置定时器，防止recvfrom()长时间阻塞，但效果不好，并不能达到想要的效果。
	经过查资料发现，原因在于使用的signal()函数：signal()是重启函数，超时以后会自动启动以阻塞的函数，而不是中断它的执行，如recvfrom，给人的感觉就是使用了alarm，但依然阻塞在了recvfrom上，不往下执行。但在中断处理函数中用printf函数打印一条消息，就会发现，其实它是中断过的，只是又回去了而已。
	
	而使用sigaction函数，可以设置是否要重启函数，即alarmact.sa_flags = SA_NOMASK;选项，它会中断已阻塞的函数，使程序继续往下执行。而SA_RESTART选项则等同于signal效果，会重启函数，阻塞在recvfrom上。
	http://www.cnblogs.com/dreamrun/p/4047795.html
	另：信号处理流程：若设置了信号处理函数，当信号到达时，会将控制流转向信号处理器。从信号处理器返回后，继续执行原来的控制流。
*/
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
http://www.cnblogs.com/dreamrun/p/4047795.html
int n;
char recvbuf[1024];

void listen_board();

static void dealSigAlarm(int sigo)
{
	n = -1;
	printf("alarm interrupt!\n");
	return;//just interrupt the recvfrom()
}

void main()
{

	struct sigaction alarmact;


//	signal(SIGALRM,dealSigAlarm);

	bzero(&alarmact,sizeof(alarmact));
	alarmact.sa_handler = dealSigAlarm;
//	alarmact.sa_flags = SA_RESTART;
	alarmact.sa_flags = SA_NOMASK;

	sigaction(SIGALRM,&alarmact,NULL);

	listen_board();

}
void listen_board()
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
