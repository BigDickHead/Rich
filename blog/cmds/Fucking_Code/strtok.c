#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char Server[512] = { "192.168.0.0|192.168.0.1|192.168.0.2|192.168.0.3" };

int GetIpNum(const char *str)
{
    int count = 0;
    while(*str) if(*str++ == '|') count++;
    return count + 1;
}

//FIXME char* GetIpValue(const char *str, int n)
char* GetIpValue(char *str, int n)
{
    int i = 0;
    char *IpString = NULL;

//FIXME char string[512] { '\0' };
//FIXME Global, Local Variable
//FIXME strncpy(string, str, 512);          //FIXME Fucking sizeof(str), str is a pointer, that's size is 4 byte.
//FIXME strtok will changed source string. XXX

    IpString = strtok(str, "|");

    if ((n == 0) && (IpString != NULL))
        return IpString;
    else {
        for (; i != n; i++)
        {
            IpString = strtok(NULL, "|");
        }

        if (IpString != NULL)
            return IpString;
    }

    return str;
}



int main(int argc, const char *argv[])
{

    printf("Num %d\n", GetIpNum(Server));
    printf("Num %d\n", GetIpNum(Server));
    printf("Vaule %s\n", GetIpValue(Server, 0));
    printf("Vaule %s\n", GetIpValue(Server, 1));
    printf("Vaule %s\n", GetIpValue(Server, 2));
    printf("Vaule %s\n", GetIpValue(Server, 2));
    printf("Vaule %s\n", GetIpValue(Server, 3));
    printf("Vaule %s\n", GetIpValue(Server, 4));
    printf("Server %s\n", Server);
    return 0;
}
