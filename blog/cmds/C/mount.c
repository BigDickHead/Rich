#include <stdio.h>
#include <sys/mount.h>

int main(int argc, char const* argv[])
{
    if (mount("rootfs", "/", "*", MS_REMOUNT|MS_MANDLOCK, NULL))
    {
        printf("remount failed\n");
        return -1;
    }
    printf("remount ok\n");
    return 0;
}
