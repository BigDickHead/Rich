#include <string.h>
#include <stdio.h>

/* 若strspn()返回的数值为n，则代表字符串s 开头连续有n 个字符都是属于字符串accept内的字符. */
/* 注意是 开头连续有 */
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
