#include<unistd.h>
#include<sys/types.h>
函数原型：
pid_t fork( void);

(pid_t 是一个宏定义，其实质是int 被定义在#include<sys/types.h>中)

返回值： 若成功调用一次则返回两个值，子进程返回0，父进程返回子进程ID；否则，出错返回-1
函数说明：
//TODO
一个现有进程可以调用fork函数创建一个新进程。由fork创建的新进程被称为子进程（child process）。fork函数被调用一次但返回两次。两次返回的唯一区别是子进程中返回0值而父进程中返回子进程ID。


子进程是父进程的副本，它将获得父进程数据空间、堆、栈等资源的副本。注意，子进程持有的是上述存储空间的“副本”，这意味着父子进程间不共享这些存储空间。

UNIX将复制父进程的地址空间内容给子进程，因此，子进程有了独立的地址空间。在不同的UNIX (Like)系统下，我们无法确定fork之后是子进程先运行还是父进程先运行，这依赖于系统的实现。所以在移植代码的时候我们不应该对此作出任何的假设。

为什么fork会返回两次？
由于在复制时复制了父进程的堆栈段，所以两个进程都停留在fork函数中，等待返回。因为fork函数会返回两次，一次是在父进程中返回，另一次是在子进程中返回，这两次的返回值是不一样的。


调用fork之后，数据、堆栈有两份，代码仍然为一份但是这个代码段成为两个进程的共享代码段都从fork函数中返回，箭头表示各自的执行处。当父子进程有一个想要修改数据或者堆栈时，两个进程真正分裂。


示例代码：
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>


int main(int argc, char ** argv )
{
    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "error!");
    }
    else if( 0 == pid )
    {
        printf("This is the child process!");
        _exit(0);
    }
    else
    {
        printf("This is the parent process! child process id = %d", pid);
    }
    //可能需要时候wait或waitpid函数等待子进程的结束并获取结束状态
    exit(0);
}

注意！样例代码仅供参考，样例代码存在着父进程在子进程结束前结束的可能性。必要的时候可以使用wait或 waitpid函数让父进程等待子进程的结束并获取子进程的返回状态。

fork()在Linux系统中的返回值是没有NULL的.
Error Codes
出错返回错误信息如下:
EAGAIN
达到进程数上限.
ENOMEM
没有足够空间给一个新进程分配.
fork函数的特点概括起来就是“调用一次，返回两次”，在父进程中调用一次，在父进程和子进程中各返回一次。
fork的另一个特性是所有由父进程打开的描述符都被复制到子进程中。父、子进程中相同编号的文件描述符在内核中指向同一个file结构体，也就是说，file结构体的引用计数要增加。



FORK(2)                                           Linux Programmer's Manual                                           FORK(2)



NAME
       fork - create a child process

SYNOPSIS
       #include <unistd.h>

       pid_t fork(void);

DESCRIPTION
       fork()  creates  a  new  process by duplicating the calling process.  The new process, referred to as the child, is an
       exact duplicate of the calling process, referred to as the parent, except for the following points:

       *  The child has its own unique process ID, and this PID  does  not  match  the  ID  of  any  existing  process  group
          (setpgid(2)).

       *  The child's parent process ID is the same as the parent's process ID.

       *  The child does not inherit its parent's memory locks (mlock(2), mlockall(2)).

       *  Process resource utilizations (getrusage(2)) and CPU time counters (times(2)) are reset to zero in the child.

       *  The child's set of pending signals is initially empty (sigpending(2)).

       *  The child does not inherit semaphore adjustments from its parent (semop(2)).

       *  The child does not inherit record locks from its parent (fcntl(2)).

       *  The child does not inherit timers from its parent (setitimer(2), alarm(2), timer_create(2)).

       *  The child does not inherit outstanding asynchronous I/O operations from its parent (aio_read(3), aio_write(3)), nor
          does it inherit any asynchronous I/O contexts from its parent (see io_setup(2)).

       The process attributes in the preceding list are all specified in POSIX.1-2001.  The parent and child also differ with
       respect to the following Linux-specific process attributes:

       *  The  child  does  not  inherit  directory  change  notifications  (dnotify) from its parent (see the description of
          F_NOTIFY in fcntl(2)).

       *  The prctl(2) PR_SET_PDEATHSIG setting is reset so that the child does not receive a signal when its  parent  termi‐
          nates.

       *  The  default  timer  slack  value  is  set  to  the  parent's  current  timer  slack value.  See the description of
          PR_SET_TIMERSLACK in prctl(2).

       *  Memory mappings that have been marked with the madvise(2) MADV_DONTFORK flag are not inherited across a fork().

       *  The termination signal of the child is always SIGCHLD (see clone(2)).

       Note the following further points:

       *  The child process is created with a single thread—the one that called fork().  The entire virtual address space  of
          the  parent  is  replicated  in the child, including the states of mutexes, condition variables, and other pthreads
          objects; the use of pthread_atfork(3) may be helpful for dealing with problems that this can cause.

       *  The child inherits copies of the parent's set of open file descriptors.  Each file descriptor in the  child  refers
          to  the  same  open  file description (see open(2)) as the corresponding file descriptor in the parent.  This means
          that the two descriptors share open file status flags, current file offset, and signal-driven I/O  attributes  (see
          the description of F_SETOWN and F_SETSIG in fcntl(2)).

       *  The  child  inherits  copies  of  the  parent's  set  of open message queue descriptors (see mq_overview(7)).  Each
          descriptor in the child refers to the same open message queue description as the corresponding  descriptor  in  the
          parent.  This means that the two descriptors share the same flags (mq_flags).

       *  The  child  inherits copies of the parent's set of open directory streams (see opendir(3)).  POSIX.1-2001 says that
          the corresponding directory streams in the parent  and  child  may  share  the  directory  stream  positioning;  on
          Linux/glibc they do not.

RETURN VALUE
       On success, the PID of the child process is returned in the parent, and 0 is returned in the child.  On failure, -1 is
       returned in the parent, no child process is created, and errno is set appropriately.

ERRORS
       EAGAIN fork() cannot allocate sufficient memory to copy the parent's page tables and allocate a task structure for the
              child.

       EAGAIN It  was  not possible to create a new process because the caller's RLIMIT_NPROC resource limit was encountered.
              To exceed this limit, the process must have either the CAP_SYS_ADMIN or the CAP_SYS_RESOURCE capability.

       ENOMEM fork() failed to allocate the necessary kernel structures because memory is tight.

       ENOSYS fork() is not supported on this platform (for example, hardware without a Memory-Management Unit).

CONFORMING TO
       SVr4, 4.3BSD, POSIX.1-2001.

NOTES
       Under Linux, fork() is implemented using copy-on-write pages, so the only penalty that it incurs is the time and  mem‐
       ory required to duplicate the parent's page tables, and to create a unique task structure for the child.

       Since  version  2.3.3, rather than invoking the kernel's fork() system call, the glibc fork() wrapper that is provided
       as part of the NPTL threading implementation invokes clone(2) with flags that provide the same effect  as  the  tradi‐
       tional  system  call.   (A  call to fork() is equivalent to a call to clone(2) specifying flags as just SIGCHLD.)  The
       glibc wrapper invokes any fork handlers that have been established using pthread_atfork(3).

EXAMPLE
       See pipe(2) and wait(2).

SEE ALSO
       clone(2), execve(2), setrlimit(2), unshare(2), vfork(2), wait(2), daemon(3), capabilities(7), credentials(7)

COLOPHON
       This page is part of release 3.44 of the Linux man-pages project.  A description of the project, and information about
       reporting bugs, can be found at http://www.kernel.org/doc/man-pages/.



Linux                                                     2012-10-27                                                  FORK(2)
