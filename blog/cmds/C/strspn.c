#include <string.h>
#include <stdio.h>

#if 0
/* the proto of function strspn */
int strspn(const char *s,const char *accept)
{
    const char *p;
    const char *a;
    int count = 0;
    for(p = s; *p != '\0'; ++p)
    {
        for (a = accept; *a != '\0'; ++a)
        {
            if (*p == *a)
            {
                break;
            }
        }
        if (*a == '\0')
        {
            return count;
        }
        ++count;
    }
    return count;
}
#endif


int main()

{
    char *str = "Linux was first developed for 386/486-based pcs.";
    printf("%d\n", strspn(str, "Linux"));
    printf("%d\n", strspn(str, "/-"));
    printf("%d\n", strspn(str, "1234567890"));
}
