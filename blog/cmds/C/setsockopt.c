#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include "tcpthread.h"
#include <sys/types.h>
#include <sys/ioctl.h>
#include <mysql/mysql.h>
#include <net/if.h>
#include "sersql.h"

void *listener(void *ip_srv, unsigned short port_srv);
int init_heart_beat_headnode();
int init_wsdisk_listNode();
int free_wslist();
int getlocaip(char *ip);
void init_socket(int sd);
char *attime();
void sigdie(int signo);
const int daemonization = 1;

char *USER = "root";            //数据库连接用户
char *PASSWD = "passwd";    //数据库连接密码
char *SQLSERVER = "localhost";  //数据库服务器
pheartnode head_ptr;            //初始化主机列表头结点
struct wslistNode *first_ptr;
int listenfd;                   //listen 的句柄变量，全局为了释放资源
int hb_listenfd;                //心跳包的socket

int main(int argc, char **argv)
{
//add
	char buffer[128]={'\0'};
	sprintf(buffer,"cat /proc/net/iet/session |grep tid|awk -F \":\" \'{print $2}\'|awk -F \" \" \'{printf $1}\' >/dev/null 2>&1");
    /*printf("buffer is %s\n",buffer);*/
	FILE *fp;
	fp=popen(buffer,"r");
	char line[30]={'\0'};
	char cmd[30]={'\0'};
	while(fgets(line,29,fp)!=NULL)
	{
	sprintf(cmd,"ietadm --op delete --tid=%s",line);
	/*printf("cmd is %s\n",cmd);*/
	system(cmd);
	}
	pclose(fp);

//end
    /* 加入信号侦测，对资源进行适时清理 */
    signal(SIGINT, sigdie);
    signal(SIGTERM, sigdie);
    signal(SIGHUP, sigdie);
	char ipbuf[16] = { '\0' };
	getlocaip(ipbuf);

	if (1 == daemonization) {
		daemon(1, 0);           //后台运行，不输出信息
	}

    pthread_t heartbeat;
    pthread_t heartbeat_recv;
    init_heart_beat_headnode();

    pthread_t wsdisk;
    init_wsdisk_listNode();


    pthread_create(&wsdisk,NULL,thread_to_check_wsdisk,NULL);
    //pthread_cancel(wsdisk);
	char *ip_srv = ipbuf;	//ip
	int port_srv = 23106;	//port


    pthread_create(&heartbeat, NULL, thread_to_check_heartbeat, NULL);
    pthread_create(&heartbeat_recv, NULL, HeartBeatRecvThread, NULL);
	listener(ip_srv, port_srv);
    pthread_cancel(heartbeat);
    pthread_cancel(heartbeat_recv);
    pthread_cancel(wsdisk);
    free_wslist();
	return 0;
}

//监听函数
void *listener(void *ip_srv, unsigned short port_srv)
{
	printf("listener function beginning!\n");    //for debugging

	int err;
	int connectfd;
	struct sockaddr_in adr_srv, adr_clt;

	socklen_t len_adr;
	pthread_t tid;
	len_adr = sizeof adr_srv;

	memset(&adr_srv, 0, len_adr);

	adr_srv.sin_family = AF_INET;	//设置地址TCP/IP协议族的地址
	adr_srv.sin_port = htons(port_srv);	//设置大端端口
	adr_srv.sin_addr.s_addr = inet_addr((char *)ip_srv);	//设置IP地址

	if (INADDR_NONE == adr_srv.sin_addr.s_addr) {
		printf("Address ((unsigned long int) 0xffffffff)!\n");
	}

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	//参数1固定，参数2当前代表tcp方式，若是udp则是SOCK_DGRAM，参数3一般是0，返回初始化服务端套接字
	{
		printf("socket initialization failed!\n");      //socket初始化失败返回-1
		exit(1);
	}

    /*FIXME*/
    //设置socket的SO_LINGER属性
    init_socket(listenfd);

	if ((err = bind(listenfd, (struct sockaddr *)&adr_srv, len_adr)) == -1)	//绑定端口
	{
		fprintf(stderr, "Address is still in use, try later!\n");
		exit(1);
	}

	if ((err = listen(listenfd, 255)) == -1)	//监听端口，第二个参数设置最大允许连接的的客户端数量
	{
		printf("listen error! %s, %d\n",__FILE__, __LINE__);
		exit(1);
	}

	while (1) {
		len_adr = sizeof adr_clt;
		/*printf("In while function for loop waiting client connect!\n");*/
		if ((connectfd = accept(listenfd, (struct sockaddr *)&adr_clt, &len_adr)) != -1)	//参数2：欲登录的客户端套机字指针，参数3：指针空间长度，返回新的连接的socket描述
		{
			/*printf("\nat %sAcceppt client from %s!\n", attime(), inet_ntoa(adr_clt.sin_addr));*/
			pthread_create(&tid, NULL, server_thread, (void *)connectfd);
		}
	}
	close(listenfd);
	/*printf("Damn it! I am down!!\n");*/
	exit(-1);
}

//get local ip address
int getlocaip(char *ip)
{
	//define socket
	int sockfd;
	if (-1 == (sockfd = socket(PF_INET, SOCK_STREAM, 0))) {
		perror("socket");
		return 0;
	}
	struct ifreq req;
	struct sockaddr_in *host;
	memset(&req, 0, sizeof(struct ifreq));
	strcpy(req.ifr_name, "eth0");

	//get  address of point to point
	ioctl(sockfd, SIOCGIFADDR, &req);

	host = (struct sockaddr_in *)&req.ifr_addr;
	strcpy(ip, inet_ntoa(host->sin_addr));
	close(sockfd);

	return 1;
}

//获取当前时间
char *attime()
{
	time_t t;
	time(&t);
	return (ctime(&t));
}


 /* ===  FUNCTION  ======================================================================
 *  函数名:  init_heart_beat_headnode
 *  描述:    初始化心跳检测主机列表的头结点
 *  参数:    无
 *  返回值:  头结点内存分配失败返回0，成功返回1
 * =====================================================================================
 */
int init_heart_beat_headnode()
{
    if ((head_ptr = (pheartnode)calloc(1, sizeof(struct heart_beat_info))) == NULL)
    {
            printf("%s\n", "Not Enough Memory Allocated for head node!");
            return 0;
    }

    head_ptr->username[0] = '\0';
    head_ptr->next = NULL;
    head_ptr->last_timestamp = 0;
    return 1;
}
int init_wsdisk_listNode()
{
    if((first_ptr=(struct wslistNode*)calloc(1,sizeof(struct wslistNode)))==NULL)
    {
        printf("%s\n","Not Enougt Memory Allocated for workstation node!");
        return 0;
    }
    first_ptr->ipaddr[0]='\0';
    first_ptr->flag=0;
    first_ptr->next=NULL;
    return 1;

}
int free_wslist()
{
  struct wslistNode *curr,*prev,*q;
  curr=head_ptr->next;
  prev=head_ptr;
 while(curr!=NULL)
{
 q=curr;
curr=curr->next;
prev->next=curr;
free(q);
}
free(head_ptr);
}
/*
 * ===  FUNCTION  ======================================================================
 *  函数名:  sigdie
 *  描述:    捕获信号，以便在程序退出或发生意外终止时能释放占用资源
 *  参数:
 *  返回值:
 * =====================================================================================
 */
void sigdie(int signo) {

  switch(signo) {
  case SIGINT:
    /*printf("caught SIGINT signal, cleaning up\n");*/
    break;

  case SIGTERM:
    /*printf("caught SIGTERM signal, cleaning up\n");*/
    break;

  case SIGHUP:
    /*printf("caught SIGHUP signal, cleaning up\n");*/
    break;

  case SIGSEGV:                                     //SIGSEGV是当一个进程执行了一个无效的内存引用，或发生段错误时发送给它的信号
    /*printf("caught SIGSEGV signal, cleaning up\n");*/
    break;

  case SIGBUS:                                      //硬件错误，内存分配失败
    /*printf("caught SIGBUS signal, cleaning up\n");*/
    break;

  default:
    /*printf("caught signal %d, cleaning up\n", signo);*/
    break;
  }

  close(listenfd);
  close(hb_listenfd);
  exit(1);
}

/*
 * ===  FUNCTION  ======================================================================
 *  函数名:  init_socket
 *  描述:    设置socket属性，使其在意外断开连接时不再TIME_WAIT
 *  参数:
 *  返回值:
 * =====================================================================================
 */
__inline__ void init_socket(int sd) {
struct linger l;

l.l_onoff = 1;
l.l_linger = 0;

if (setsockopt(sd, SOL_SOCKET, SO_LINGER,  &l, sizeof(struct linger)))
  {
   fprintf(stderr, "Problem setting socket SO_LINGER, errno: %d\n", errno);
   perror("setsockopt");
  }
}
