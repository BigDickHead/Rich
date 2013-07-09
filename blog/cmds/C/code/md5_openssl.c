apt-get install libssl-dev


/*
 * MD2_Init() initializes a MD2_CTX structure.
 *
 * MD2_Update() can be called repeatedly with chunks of the message to be hashed (len bytes at data).
 *
 * MD2_Final() places the message digest in md, which must have space for MD2_DIGEST_LENGTH == 16 bytes of output, and erases the MD2_CTX.
 *
 * MD4_Init(), MD4_Update(), MD4_Final(), MD5_Init(), MD5_Update(), and MD5_Final() are analogous using an MD4_CTX and MD5_CTX structure.
 */
方法一：
#include<stdio.h>
#include<openssl/md5.h>
#include<string.h>

int main( int argc, char **argv )
{
    MD5_CTX ctx;
    unsigned char *data="123";
    unsigned char md[16];
    char buf[33]={'\0'};
    char tmp[3]={'\0'};
    int i;

    MD5_Init(&ctx);
    MD5_Update(&ctx,data,strlen(data));
    MD5_Final(md,&ctx);

    for( i=0; i<16; i++ ){
        sprintf(tmp,"%02X",md[i]);
        strcat(buf,tmp);
    }
    printf("%s\n",buf);
    return 0;
}

输出：
202CB962AC59075B964B07152D234B70

方法二：
#include<stdio.h>
#include<openssl/md5.h>
#include<string.h>

int main( int argc, char **argv )
{
    unsigned char *data = "123";
    unsigned char md[16];
    int i;
    char tmp[3]={'\0'},buf[33]={'\0'};
    MD5(data,strlen(data),md);
    for (i = 0; i < 16; i++){
        sprintf(tmp,"%2.2x",md[i]);
        strcat(buf,tmp);
    }
    printf("%s\n",buf);
    return 0;
}
输出：
202cb962ac59075b964b07152d234b70

总结：
两种实现方式，一样的结果。相比较，第二种方法更直观简单些。
遇到的问题：
1.sprintf时的格式，%02X和%2.2格式是一样的，强制输出两位，比如十进制的8，十六进制也是8，这个格式是控制输出两位，08。如果加密结果要小写字母显示，就是"%2.2x",大写就是"2.2X"。
2.一定要使用unsigned char,虽然是char，其实就是个int,当然，它只有一个字节，和char的区别是，unsigned char 0~255 ,char -127~127。使用char会出问题。这个md5加密函数，返回16个十进制数，范围在0～255间，把它format为十六进制就是32为md5编码了。
3.gcc编译的时候，后面加上参数 -lcrypto 如果系统没有安装libcrypto，是不能用这个方法的。


Example #1: md5_sample1.cpp
#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

int main(int argc, const char *argv[])
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    char string[] = "happy";

    MD5((unsigned char*) &string, strlen(string), (unsigned char*) &digest);

    char mdString[33];

    for (int i = 0; i < 16; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int) digest[i]);

    printf("md5 digest: %s\n", mdString);

    return 0;
}

Example #2: md5_sample2.cpp
#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

int main(void)
{
    unsigned char digest[16];
    const char *string = "happy";

    printf("string length: %d\n", strlen(string));

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, string, strlen(string));
    MD5_Final(digest, &ctx);

    char mdString[33];
    for (int i = 0; i < 16; i++)
        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    printf("md5 digest: %s\n", mdString);

    return 0;
}
