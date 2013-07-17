//remove blank from a string.
#include <stdio.h>

int main(int argc, const char *argv[])
{
    char s[512] = "abc da dfdsf  defg";
    int i = 0, j= 0;
    for (; j < 512; j++)
    {
        if (s[j] != ' ')
            s[i++] = s[j];
    }
    s[i] = '\0';
    printf("%s\n", s);
    return 0;
}
