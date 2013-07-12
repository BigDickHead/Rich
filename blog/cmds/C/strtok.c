/*
 *strtok函数会破坏被分解字符串的完整，调用前和调用后的s已经不一样了。如果
 *要保持原字符串的完整，可以使用strchr和sscanf的组合等。
 */
//C code
#include <string.h>
#include <stdio.h>
int main(void)
{
    char input[16] = "abc,d";
    char *p;
    /**/ /* strtok places a NULL terminator
            in front of the token, if found */
    p = strtok(input, ",");
    if (p) printf("%s\n", p);
    /**/ /* A second call to strtok using a NULL
            as the first parameter returns a pointer
            to the character following the token */
    p = strtok(NULL, ",");
    if (p) printf("%s\n", p);
    return 0;
}


//C++ code
#include <iostream>
#include <cstring>
using namespace std;
int main()
{
    char sentence[]="This is a sentence with 7 tokens";
    cout<<"The string to be tokenized is:\n"<<sentence<<"\n\nThe tokens are:\n\n";
    char *tokenPtr=strtok(sentence," ");
    while(tokenPtr!=NULL)
    {
        cout<<tokenPtr<<'\n';
        tokenPtr=strtok(NULL," ");
    }
    //cout<<"After strtok, sentence = "<<tokenPtr<<endl;
    return 0;
}
/*
 *函数第一次调用需设置两个参数。第一次分割的结果，返回串中第一个 ',' 之前的字符串,也就是上面的程序第一次输出abc。
 *第二次调用该函数strtok(NULL,","),第一个参数设置为NULL。结果返回分割依据后面的字串，即第二次输出d。
 *strtok是一个线程不安全的函数，因为它使用了静态分配的空间来存储被分割的字符串位置
 *线程安全的函数叫strtok_r,ca
 *运用strtok来判断ip或者mac的时候务必要先用其他的方法判断'.'或':'的个数，因为用strtok截断的话，比如："192..168.0...8..."这个字符串，strtok只会截取四次，中间的...无论多少都会被当作一个key
 */


Linux内核2.6.29，说明了strtok()这个函数已经不再使用，由速度更快的strsep()代替
strtok_r()线程安全，strtok()线程不安全

/*
* linux/lib/string.c
*
* Copyright (C) 1991, 1992 Linus Torvalds
*/
/*
* stupid library routines.. The optimized versions should generally be found
* as inline code in <asm-xx/string.h>
*
* These are buggy as well..
*
* * Fri Jun 25 1999, Ingo Oeser <ioe@informatik.tu-chemnitz.de>
* - Added strsep() which will replace strtok() soon (because strsep() is
* reentrant and should be faster). Use only strsep() in new code, please.
*
* * Sat Feb 09 2002, Jason Thomas <jason@topic.com.au>,
* Matthew Hawkins <matt@mh.dropbear.id.au>
* - Kissed strtok() goodbye
*/

strtok()这个函数大家都应该碰到过,但好像总有些问题, 这里着重讲下它

下面我们来看一个例子：

int main() {
    char test1[] = "feng,ke,wei";
    char *test2 = "feng,ke,wei";
    char *p; p = strtok(test1, ",");
    while(p)
    {
        printf("%s\n", p);
        p = strtok(NULL, ",");
    }
    return 0;
}

运行结果:
feng
ke
wei

但如果用p = strtok(test2, ",")则会出现内存错误,这是为什么呢?是不是跟它里面那个静态变量有关呢? 我们来看看它的原码:

/***
*strtok.c - tokenize a string with given delimiters
*
*         Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*         defines strtok() - breaks string into series of token
*         via repeated calls.
*
*******************************************************************************/

#include <cruntime.h>
#include <string.h>
#ifdef _MT
#include <mtdll.h>
#endif  /* _MT */

/***
*char *strtok(string, control) - tokenize string with delimiter in control
*
*Purpose:
*         strtok considers the string to consist of a sequence of zero or more
*         text tokens separated by spans of one or more control chars. the first
*         call, with string specified, returns a pointer to the first char of the
*         first token, and will write a null char into string immediately
*         following the returned token. subsequent calls with zero for the first
*         argument (string) will work thru the string until no tokens remain. the
*         control string may be different from call to call. when no tokens remain
*         in string a NULL pointer is returned. remember the control chars with a
*         bit map, one bit per ascii char. the null char is always a control char.
*       //这里已经说得很详细了!!比MSDN都好!
*Entry:
*         char *string - string to tokenize, or NULL to get next token
*         char *control - string of characters to use as delimiters
*
*Exit:
*         returns pointer to first token in string, or if string
*         was NULL, to next token
*         returns NULL when no more tokens remain.
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

char * __cdecl strtok (
          char * string,
          const char * control
          )
{
          unsigned char *str;
          const unsigned char *ctrl = control;

          unsigned char map[32];
          int count;

#ifdef _MT
          _ptiddata ptd = _getptd();
#else  /* _MT */
          static char *nextoken;                          //保存剩余子串的静态变量
#endif  /* _MT */

          /* Clear control map */
          for (count = 0; count < 32; count++)
                  map[count] = 0;

          /* Set bits in delimiter table */
          do {
                  map[*ctrl >> 3] |= (1 << (*ctrl & 7));
          } while (*ctrl++);

          /* Initialize str. If string is NULL, set str to the saved
           * pointer (i.e., continue breaking tokens out of the string
           * from the last strtok call) */
          if (string)
                  str = string;                               //第一次调用函数所用到的原串

else
#ifdef _MT
                  str = ptd->_token;
#else  /* _MT */
                str = nextoken;                        //将函数第一参数设置为NULL时调用的余串

#endif  /* _MT */

          /* Find beginning of token (skip over leading delimiters). Note that
           * there is no token iff this loop sets str to point to the terminal
           * null (*str == '\0') */
          while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
                  str++;

        string = str;                                    //此时的string返回余串的执行结果

          /* Find the end of the token. If it is not the end of the string,
           * put a null there. */

//这里就是处理的核心了, 找到分隔符,并将其设置为'\0',当然'\0'也将保存在返回的串中
          for ( ; *str ; str++ )
                  if ( map[*str >> 3] & (1 << (*str & 7)) ) {
                        *str++ = '\0';                //这里就相当于修改了串的内容 ①
                          break;
                  }

          /* Update nextoken (or the corresponding field in the per-thread data
           * structure */
#ifdef _MT
          ptd->_token = str;
#else  /* _MT */
        nextoken = str;                   //将余串保存在静态变量中,以便下次调用
#endif  /* _MT */

          /* Determine if a token has been found. */
          if ( string == str )
                return NULL;
          else
                  return string;


1. strtok介绍

众所周知，strtok可以根据用户所提供的分割符（同时分隔符也可以为复数比如“，。”）

将一段字符串分割直到遇到"\0".



比如，分隔符=“，” 字符串=“Fred，John，Ann”

通过strtok 就可以把3个字符串 “Fred”      “John”       “Ann”提取出来。

上面的C代码为

QUOTE:
int in=0;
char buffer[]="Fred,John,Ann";
char *p[3];
char *buff = buffer;
while((p[in]=strtok(buf,","))!=NULL) {
    i++;
    buf=NULL; }

如上代码，第一次执行strtok需要以目标字符串的地址为第一参数（buf=buffer），之后strtok需要以NULL为第一参数 (buf=NULL)。指针列p[],则储存了分割后的结果，p[0]="John",p[1]="John",p[2]="Ann"，而buf就变 成    Fred\0John\0Ann\0。

2. strtok的弱点
让我们更改一下我们的计划：我们有一段字符串 "Fred male 25,John male 62,Anna female 16" 我们希望把这个字符串整理输入到一个struct，

QUOTE:
struct person {
char [25] name ;
char [6] sex;
char [4] age;
}

要做到这个，其中一个方法就是先提取一段被“，”分割的字符串，然后再将其以“ ”（空格）分割。
比如： 截取 "Fred male 25" 然后分割成 "Fred" "male" "25"
以下我写了个小程序去表现这个过程：

QUOTE:
#include<stdio.h>
#include<string.h>
#define INFO_MAX_SZ 255
int main()
{
    int in=0;
    char buffer[INFO_MAX_SZ]="Fred male 25,John male 62,Anna female 16";
    char *p[20];
    char *buf=buffer;

    while((p[in]=strtok(buf,","))!=NULL) {
        buf=p[in];
        while((p[in]=strtok(buf," "))!=NULL) {
            in++;
            buf=NULL;
        }
        p[in++]="***"; //表现分割
        buf=NULL; }

    printf("Here we have %d strings\n",i);
    for (int j=0; j<in; j++)
        printf(">%s<\n",p[j]);
    return 0;
}

这个程序输出为：
Here we have 4 strings
>Fred<
>male<
>25<
>***<
这只是一小段的数据，并不是我们需要的。但这是为什么呢？ 这是因为strtok使用一个static（静态）指针来操作数据，让我来分析一下以上代码的运行过程：

红色为strtok的内置指针指向的位置，蓝色为strtok对字符串的修改

1. "Fred male 25,John male 62,Anna female 16" //外循环

2. "Fred male 25\0John male 62,Anna female 16" //进入内循环

3.    "Fred\0male 25\0John male 62,Anna female 16"

4.    "Fred\0male\025\0John male 62,Anna female 16"

5 "Fred\0male\025\0John male 62,Anna female 16" //内循环遇到"\0"回到外循环

6   "Fred\0male\025\0John male 62,Anna female 16" //外循环遇到"\0"运行结束。

3. 使用strtok_r
在这种情况我们应该使用strtok_r, strtok reentrant.
char *strtok_r(char *s, const char *delim, char **ptrptr);

相对strtok我们需要为strtok提供一个指针来操作，而不是像strtok使用配套的指针。
代码：

QUOTE:
#include<stdio.h>
#include<string.h>
#define INFO_MAX_SZ 255
int main()
{
    int in=0;
    char buffer[INFO_MAX_SZ]="Fred male 25,John male 62,Anna female 16";
    char *p[20];
    char *buf=buffer;

    char *outer_ptr=NULL;
    char *inner_ptr=NULL;

    while((p[in]=strtok_r(buf,",",&outer_ptr))!=NULL) {
        buf=p[in];
        while((p[in]=strtok_r(buf," ",&inner_ptr))!=NULL) {
            in++;
            buf=NULL;
        }
        p[in++]="***";
        buf=NULL; }

    printf("Here we have %d strings\n",i);
    for (int j=0; jn<i; j++)
        printf(">%s<\n",p[j]);
    return 0;
}

这一次的输出为：
Here we have 12 strings
>Fred<
>male<
>25<
>***<
>John<
>male<
>62<
>***<
>Anna<
>female<
>16<
>***<


让我来分析一下以上代码的运行过程：

红色为strtok_r的outer_ptr指向的位置，
紫色为strtok_r的inner_ptr指向的位置，
蓝色为strtok对字符串的修改

1. "Fred male 25,John male 62,Anna female 16" //外循环

2. "Fred male 25\0John male 62,Anna female 16"//进入内循环

3.   "Fred\0male 25\0John male 62,Anna female 16"

4   "Fred\0male\025\0John male 62,Anna female 16"

5 "Fred\0male\025\0John male 62,Anna female 16" //内循环遇到"\0"回到外循环

6   "Fred\0male\025\0John male 62\0Anna female 16"//进入内循环


}

原来, 该函数修改了原串.

所以,当使用char *test2 = "feng,ke,wei"作为第一个参数传入时,在位置①处, 由于test2指向的内容保存在文字常量区,该区的内容是不能修改的,所以会出现内存错误. 而char test1[] = "feng,ke,wei" 中的test1指向的内容是保存在栈区的,所以可以修改.
