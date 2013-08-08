sigsuspend用法

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

void my_op(int);

main()
{
        sigset_t new_mask;
        struct sigaction act;

        sigemptyset(&act.sa_mask);
        act.sa_flags=0;
        act.sa_handler=my_op;

        if(sigaction(SIGINT,&act,NULL))   //注册信号SIGINT处理函数为my_op
        {
                printf("install signal SIGINT error\n");
        }

        if(sigaction(SIGQUIT,&act,NULL)) //注册信号SIGQUIT处理函数为my_op
        {
                printf("install signal SIGQUIT error\n");
        }

        printf("please send signal: kill -s %d %d\n",SIGQUIT, getpid());

        sigemptyset(&new_mask);

        if(sigprocmask(0, NULL,&new_mask) < 0) //查询当前进程被挂起的信号存入new_mask中，
                                               //可用sigpending(&new_mask)代替
        {
                printf("get pending mask error\n");
        }

        sigaddset(&new_mask,SIGQUIT);     //将信号SIGQUIT，添加到信号集new_mask中，
                                           //也就是在原有的信号掩码中加入信号SIGQUIT。
         sigsuspend(&new_mask);            //将当前的信号掩码替换成new_mask，
                                           //也就是把信号SIGQUIT给阻塞后，当前进程挂起。
                                           //必须等待除了未阻塞的信号到来才恢复成原来的信号掩码
        exit(0);
}

void my_op(int signum)
{
        if ( signum == SIGINT )
        {
                printf("SIGINT\n");
        }
        else if ( signum == SIGQUIT )
        {
                printf("SIGQUIT\n");
        }

        return;
}
----------------------------------------------------------------------
编译该程序，并运行。在另一终端向该进程发送信号(运行kill -s SIGQUIT pid，SIGQUIT以及pid具体多少看程序的输出)，一直未见到应该打印的信息"SIGQUIT\n"，这就说明SIGQUIT信号被阻塞了，当发送SIGINT信号时给该程序时，因为SIGINT信号并未阻塞，所以打印了"SIGINT\n"，紧接着sigsuspend恢复原来的信号掩码（原来的信号掩码并未阻塞SIGQUIT信号），最后才打印了刚开始接收到的SIGQUIT信号的信息息"SIGQUIT\n"。具体情况如下：

情况一：
1、程序运行后，执行到sigsuspend函数，用new_mask替换原来的信号掩码，即SIGQUIT信号被阻塞。
2、开启另一终端，执行 kill -s SIGQUIT pid 程序结果如下：
please send signal: kill -s 3 4026

3、这就说明SIGQUIT信号被阻塞了，未见到应该打印的信息"SIGQUIT\n"，程序任在被挂起状态。

情况二：
1、程序运行后，执行到sigsuspend函数，用new_mask替换原来的信号掩码，即SIGQUIT信号被阻塞。
2、开启另一终端，执行 kill -s SIGQUIT pid 程序结果如下：
please send signal: kill -s 3 4026

3、这就说明SIGQUIT信号被阻塞了，未见到应该打印的信息"SIGQUIT\n""，程序任在被挂起状态。
4、继续在另一终端上，执行 kill -s SIGINT pid 程序结果如下：
please send signal: kill -s 3 4026
SIGINT
SIGQUIT

程序退出

5、这说明sigsuspend恢复原来的信号掩码（原来的信号掩码并未阻塞SIGQUIT信号），最后才打印了刚开始接收到的SIGQUIT信号的信息息"SIGQUIT\n"，打印结束程序退出。

================================================

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

volatile quitflag;
void my_op(int);

main()
{
        sigset_t new_mask,old_mask,zero_mask;
        struct sigaction act;
        quitflag = 0;

        sigemptyset(&act.sa_mask);
        act.sa_flags=0;
        act.sa_handler=my_op;

        if(sigaction(SIGINT,&act,NULL))   //注册信号SIGINT处理函数为my_op
        {
                printf("install signal SIGINT error\n");
        }

        if(sigaction(SIGQUIT,&act,NULL)) //注册信号SIGQUIT处理函数为my_op
        {
                printf("install signal SIGQUIT error\n");
        }

        printf("please send signal: kill -s %d %d\n",SIGQUIT, getpid());

        sigemptyset(&new_mask);
        sigaddset(&new_mask,SIGQUIT);    //将信号SIGQUIT，添加到空信号集new_mask中

        if(sigprocmask(SIG_BLOCK, &new_mask,&old_mask)) //把信号SIGQUIT给阻塞
        {
                printf("block signal SIGQUIT error\n");
        }

        sigemptyset(&zero_mask);    //清空信号集zero_mask

        while(quitflag == 0)
        {
              sigsuspend(&zero_mask); //将当前的信号掩码替换成空信号掩码zero_mask，
                                        //也就是没有任何信号被阻塞，当前进程挂起。
                                       //等待任意信号到来就可恢复成原来的信号码，但是这里有个循环
                                     //判断当quitflag == 0 时继续挂起当前进程，这里呢除了SIGQUIT信号
                                                  //外其它信号都不可以改变quitflag的值
        }

        if(sigprocmask(SIG_SETMASK,&old_mask,NULL)<0) //程序继续运行后恢复进程原来的信号掩码
        {
                printf("unblock signal error\n");
        }

        exit(0);
}

void my_op(int signum)
{
        if ( signum == SIGINT )
        {
                printf("SIGINT\n");
        }
        else if ( signum == SIGQUIT )
        {
                printf("SIGQUIT\n");
                quitflag = 1;
        }

        return;
}
----------------------------------------------------------------------

这里程序的情况分三种：
情况一：
1、程序运行后，执行到sigsuspend函数，用空信号掩码zero_mask替换原来的信号掩码，即不阻塞任何信号。

2、开启另一终端，执行 kill -s SIGINT pid 程序结果如下：
please send signal: kill -s 3 4026
SIGINT

3、程序任在被挂起状态，这就说明还在循环中，因为SIGINT信号的处理函数不能更改quitflag的值，仍然不阻塞任何信号。

情况二：
1、程序运行后，执行到sigsuspend函数，用空信号掩码zero_mask替换原来的信号掩码，即不阻塞任何信号。

2、开启另一终端，执行 kill -s SIGINT pid 程序结果如下：
please send signal: kill -s 3 4026
SIGINT

3、程序任在被挂起状态，这就说明还在循环中，因为SIGINT信号的处理函数不能更改quitflag的值，仍然不阻塞任何信号。

4、继续在另一终端上，执行 kill -s SIGQUIT pid 程序结果如下：
please send signal: kill -s 3 4026
SIGINT
SIGQUIT

执行完后程序退出

5、为什么打印了“SIGQUIT\n”？不是被阻塞了么？原因是：sigsuspend不阻塞任何信号，所以呢，就执行了SIGQUIT的处理函数，改变了quitflag的值，quitflag = 1，打印"SIGQUIT\n"; 然后退出循环。紧接着sigsuspend恢复“原来的”信号掩码（这里的“原来的信号掩码”是包含阻塞SIGQUIT信号的），最后把信号掩码设置成最初的样子old_mask，程序退出。

情况三：
1、程序运行后，执行到sigsuspend函数，用空信号掩码zero_mask替换原来的信号掩码，即不阻塞任何信号。

2、开启另一终端，执行 kill -s SIGINT pid 程序结果如下：
please send signal: kill -s 3 4026
SIGINT

3、程序任在被挂起状态，这就说明还在循环中，因为SIGINT信号的处理函数不能更改quitflag的值，仍然不阻塞任何信号。

4、继续在另一终端上，执行 kill -s 40 pid 程序结果如下：
please send signal: kill -s 3 4026
SIGINT
Real-time signal 5

执行完后程序退出

5、为什么打印了“Real-time signal 5\n”？原因是：sigsuspend不阻塞任何信号，所以呢，就执行了40的默认处理函数。那为什么退出了循环了？应该没改变quitflag的值啊？实际上，40信号的默认处理函数是打印一行字符后就直接终止程序了，所以后面的代码都没执行了。想要改变就必须重新注册40的处理函数。
===============================================================================

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

volatile quitflag;
void my_op(int);

main()
{
        sigset_t new_mask,old_mask,full_mask;
        struct sigaction act;
        quitflag = 0;

        sigemptyset(&act.sa_mask);
        act.sa_flags=0;
        act.sa_handler=my_op;

        if(sigaction(SIGINT,&act,NULL))   //注册信号SIGINT处理函数为my_op
        {
                printf("install signal SIGINT error\n");
        }

        if(sigaction(SIGQUIT,&act,NULL)) //注册信号SIGQUIT处理函数为my_op
        {
                printf("install signal SIGQUIT error\n");
        }

        printf("please send signal: kill -s %d %d\n",SIGQUIT, getpid());

        sigemptyset(&new_mask);
        sigaddset(&new_mask,SIGQUIT);    //将信号SIGQUIT，添加到空信号集new_mask中

        if(sigprocmask(SIG_BLOCK, &new_mask,&old_mask)) //把信号SIGQUIT给阻塞
        {
                printf("block signal SIGQUIT error\n");
        }

        sigfillset(&full_mask);          //将所有信号添加到信号集full_mask
        sigdelset(&full_mask,SIGQUIT); //将信号集full_mask中的SIGQUIT信号删除

        while(quitflag == 0)
        {
              sigsuspend(&full_mask); //将当前的信号掩码替换成信号掩码full_mask (不包含SIGQUIT信号)，
                                          //也就是除了SIGQUIT信号其它信号都被阻塞，当前进程挂起。
                                          //只能等待SIGQUIT信号到来就可恢复成原来的信号掩码，
                                      //（这里“原来的信号掩码”是SIGQUIT信号被阻塞的信号集）
        }

        if(sigprocmask(SIG_SETMASK,&old_mask,NULL)<0) //程序继续运行后恢复进程最初始的信号掩码
        {
                printf("unblock signal error\n");
        }

        exit(0);
}

void my_op(int signum)
{
        if ( signum == SIGINT )
        {
                printf("SIGINT\n");
        }
        else if ( signum == SIGQUIT )
        {
                printf("SIGQUIT\n");
                quitflag = 1;
        }

        return;
}
----------------------------------------------------------------------
这里程序的情况分三种：
情况一：
1、程序运行后，执行到sigsuspend函数，用full_mask替换原来的信号掩码，除了SIGQUIT信号其它信号都被阻塞。

2、开启另一终端，执行 kill -s SIGINT pid 程序结果如下：
please send signal: kill -s 3 4026

3、程序任在被挂起状态，这就说明还在循环中，因为SIGINT信号被阻塞。

情况二：
1、程序运行后，执行到sigsuspend函数，用full_mask替换原来的信号掩码，除了SIGQUIT信号其它信号都被阻塞。

2、开启另一终端，执行 kill -s SIGINT pid 程序结果如下：
please send signal: kill -s 3 4026

3、程序任在被挂起状态，这就说明还在循环中，因为SIGINT信号被阻塞。

4、继续在另一终端上，执行 kill -s SIGQUIT pid 程序结果如下：
please send signal: kill -s 3 4026
SIGQUIT
SIGINT

执行完后程序退出

5、为什么先打印了“SIGQUIT\n”？原因是：sigsuspend不阻塞SIGQUIT信号，所以呢，就执行了SIGQUIT的处理函数，改变了quitflag的值，quitflag = 1，打印"SIGQUIT\n"; 然后退出循环。紧接着sigsuspend恢复“原来的”信号掩码（这里的“原来的信号掩码”是阻塞SIGQUIT信号而不阻塞SIGINT的）所以接着打印“SIGINT\n”，最后把信号掩码设置成最初的样子old_mask，程序退出。

情况三：
1、程序运行后，执行到sigsuspend函数，用full_mask替换原来的信号掩码，除了SIGQUIT信号其它信号都被阻塞。

2、开启另一终端，执行 kill -s SIGINT pid 程序结果如下：
please send signal: kill -s 3 4026

3、程序任在被挂起状态，这就说明还在循环中，因为SIGINT信号被阻塞。

4、继续在另一终端上，执行 kill -s 40 pid 程序结果如下（40信号也是被阻塞的，所以还在循环中）：
please send signal: kill -s 3 4026

5、继续在另一终端上，执行 kill -s SIGQUIT pid 程序结果如下：
please send signal: kill -s 3 4026
SIGQUIT
Real-time signal 5

执行完后程序退出

6、为什么没打印了“SIGINT\n”？原因是：实际上，40信号的默认处理函数是打印一行字符后就直接终止程序了，所以后面的代码都没执行了。想要改变就必须重新注册40的处理函数。
