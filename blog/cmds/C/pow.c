// X 的 Y次方

//FIXME
//2^10, 在 C 中是2与10 异或

#include<math.h>
#include <stdio.h>
int main(void)
{
    double x = 2.0, y = 3.0;
    printf("%lf raised to %lf is %lf\n", x, y, pow(x, y));
    return 0;
}
