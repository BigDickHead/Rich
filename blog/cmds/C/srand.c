srand函数是随机数发生器的初始化函数。

原型：void srand(unsigned seed);
用法：它需要提供一个种子，这个种子会对应一个随机数，如果使用相同的种子后面的rand()函数会出现一样的随机数。如： srand(1); 直接使用1来初始化种子。不过为了防止随机数每次重复常常使用系统时间来初始化，即使用 time函数来获得系统时间，它的返回值为从 00:00:00 GMT, January 1, 1970 到现在所持续的秒数，然后将time_t型数据转化为(unsigned)型再传给srand函数，即： srand((unsigned) time(&t)); 还有一个经常用法，不需要定义time_t型t变量,即： srand((unsigned) time(NULL)); 直接传入一个空指针，因为你的程序中往往并不需要经过参数获得的t数据。srand((int)getpid()); 使用程序的ID(getpid())来作为初始化种子，在同一个程序中这个种子是固定的[1]。

srand((unsigned) time(NULL)); /*播种子*/
Or
srand(time(NULL));

例1（C语言）
随机输出十个0-100之间的整数[2]
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19

#include <stdlib.h> /*用到了srand函数，所以要有这个头文件*/
#include <stdio.h>
#define MAX 10

int main( void)
{
    int number[MAX] = {0};
    int i;
    unsigned int seed;
    scanf("%d",&seed);/*手动输入种子*/
    srand(seed);
    for(i = 0; i < MAX; i++)
    {
        number[i] = rand() % 100; /*产生100以内的随机整数*/
        printf("%d ", number[i]);
    }
    printf("\n");
    return 0;
}

例2（C语言）
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18

#include <stdlib.h>
#include <stdio.h>
#include <time.h> /*用到了time函数，所以要有这个头文件*/
#define MAX 10

int main( void)
{
    int number[MAX] = {0};
    int i;
    srand((unsigned) time(NULL)); /*播种子*/
    for(i = 0; i < MAX; i++)
    {
        number[i] = rand() % 100; /*产生100以内的随机整数*/
        printf("%d ", number[i]);
    }
    printf("\n");
    return 0;
}
例3（C++）
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27

#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

int main()
{
    double random(double,double);
    srand(unsigned(time(0)));
    for(int icnt = 0; icnt != 10; ++icnt)
        cout << "No." << icnt+1 << ": " << int(random(0,10))<< endl;
    return 0;
}
double random(double start, double end)
{ return start+(end-start)*rand()/(RAND_MAX+ 1.0);}

/* 运行结果
* No.1: 3
* No.2: 9
* No.3: 0
* No.4: 9
* No.5: 5
* No.6: 6
* No.7: 9
* No.8: 2
* No.9: 9
* No.10: 6*/
