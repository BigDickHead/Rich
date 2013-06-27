#include <fcntl.h>

int fcntl(int filedes, int cmd, ... /* int arg */ );

fcntl 函数有5种功能:
(1), 复制一个现有到描述符 (cmd = F_DUPFD)
(2), 获得/设置文件描述符标记 (cmd = F_GETFD / F_SETFD)
(3), 获得/设置文件状态标记 (cmd = F_GETFL / F_SETFL)
(4), 获得/设置异步I/O所有权 (cmd = F_GETOWN / F_SETOWN)
(5), 获得/设置记录锁 (cmd = F_GETLK / F_SETLK /F_SETLKW)



F_DUPFD     复制文件描述符filedes. 新文件描述符作为函数值返回.

F_GETFD     对应于filedes的文件描述符标志作为函数值返回.

F_SETFD     对于filedes设置文件描述符标志。新标志值按第三个参数(取为整数值)设置。

F_GETFL
注意: 三种访问方式标志(O_RDONLY, O_WRONLY, O_RDWR)并不是各占1位(这三种标志的值分别是0, 1, 2, 由于历史原因，这三种值互斥---一个文件只能有这三种值之一).因此首先必须用屏蔽字O_ACCMODE取得访问模式位，然后将结果与这三种值中的任一种做比较.

#include <fcntl.h>
int main(int argc, const char *argv[])
{
    int val;

    if (argc != 2)
    {
        printf("usage: a.out <descriptor#>");
        exit (1);
    }

    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
    {
        printf("fcntl error for fd %d", atoi(argv[1]));
        exit (1);
    }

    switch (val & O_ACCMODE) {
        case O_RDONLY:
            printf("read only");
            break;
        case O_WRONLY:
            printf("write only");
            break;
        case O_RDWR:
            printf("read write\n");
            break;
        default:
            printf("unknown access mode");
    }

    if (val & O_APPEND)
        printf(", append");

    if (val & O_NONBLOCK)
        printf(", nonblocking");

#if defined (O_SYNC)
    if (val & O_SYNC)
        printf(", synchronous writes\n");
#endif

#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC)
    if (val & O_FSYNC)
        printf(", synchronous writes\n");
#endif
    putchar ('\n');
    exit (0);
}


在修改文件描述符标志或文件状态标志时必须谨慎，先要取得现有的标志值，然后根据需要修改它，最后设置新标志值。不能只执行F_SETFD / F_SETFL, 这样会关闭以前设置的标志位.
