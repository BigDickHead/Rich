#include <stdio.h>
#include <string.h>

int main(void)
{
    char *src = "This is a source string!";
    char dest[80] = { '\0' };
    char *ptr = NULL;

    ptr = (char*)memccpy(dest, src, 'a', strlen(src));

    if (ptr)
    {
        *ptr = '\0';
        printf("The character was found, %s\n", dest);
    } else {
        printf("The character wasn't found\n");
    }
    return 0;
}
