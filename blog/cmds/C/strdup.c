

char *strdup(char *s);
头文件：string.h

//TODO
strdup()在内部调用了 malloc() 为变量分配内存，不需要使用返回的字符串时，需要用 free() 释放相应的内存空间，否则会造成内存泄漏。


返回一个指针,指向为复制字符串分配的空间;如果分配空间失败,则返回NULL值。

①.// strdup.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main()
{
    char *s="Golden Global View";
    char *d;
    clrscr();
    d=strdup(s);        //strdup() 内部自动调用malloc()
    printf("%s",d);
    free(d);
    getchar();
    return 0;
}
运行结果：
Golden Global View

②.Example：
CString sPath="d:\\1.jpg";
LPTSTR str = strdup( sPath );
