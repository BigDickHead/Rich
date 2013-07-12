STRTOK(3)                                         Linux Programmer's Manual                                         STRTOK(3)



NAME
       strtok, strtok_r - extract tokens from strings

SYNOPSIS
       #include <string.h>

       char *strtok(char *str, const char *delim);

       char *strtok_r(char *str, const char *delim, char **saveptr);

   Feature Test Macro Requirements for glibc (see feature_test_macros(7)):

       strtok_r(): _SVID_SOURCE || _BSD_SOURCE || _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE

DESCRIPTION
       The  strtok()  function  parses  a  string  into a sequence of tokens.  On the first call to strtok() the string to be
       parsed should be specified in str.  In each subsequent call that should parse the same string, str should be NULL.

       The delim argument specifies a set of bytes that delimit the tokens in the parsed string.  The caller may specify dif‐
       ferent strings in delim in successive calls that parse the same string.

       Each  call  to strtok() returns a pointer to a null-terminated string containing the next token.  This string does not
       include the delimiting byte.  If no more tokens are found, strtok() returns NULL.

       A sequence of two or more contiguous delimiter bytes in the parsed string is considered  to  be  a  single  delimiter.
       Delimiter  bytes  at the start or end of the string are ignored.  Put another way: the tokens returned by strtok() are
       always nonempty strings.

       The strtok_r() function is a reentrant version strtok().  The saveptr argument is a pointer to a char * variable  that
       is used internally by strtok_r() in order to maintain context between successive calls that parse the same string.

       On the first call to strtok_r(), str should point to the string to be parsed, and the value of saveptr is ignored.  In
       subsequent calls, str should be NULL, and saveptr should be unchanged since the previous call.

       Different strings may be parsed concurrently using sequences of calls to strtok_r()  that  specify  different  saveptr
       arguments.

RETURN VALUE
       The strtok() and strtok_r() functions return a pointer to the next token, or NULL if there are no more tokens.

CONFORMING TO
       strtok()
              SVr4, POSIX.1-2001, 4.3BSD, C89, C99.

       strtok_r()
              POSIX.1-2001.

BUGS
       Be cautious when using these functions.  If you do use them, note that:

       * These functions modify their first argument.

       * These functions cannot be used on constant strings.

       * The identity of the delimiting byte is lost.

       * The  strtok()  function uses a static buffer while parsing, so it's not thread safe.  Use strtok_r() if this matters
         to you.

EXAMPLE
       The program below uses nested loops that employ strtok_r() to break a string into a  two-level  hierarchy  of  tokens.
       The  first  command-line  argument  specifies  the  string  to be parsed.  The second argument specifies the delimiter
       byte(s) to be used to separate that string into "major" tokens.  The third argument specifies the delimiter byte(s) to
       be used to separate the "major" tokens into subtokens.

       An example of the output produced by this program is the following:

           $ ./a.out 'a/bbb///cc;xxx:yyy:' ':;' '/'
           1: a/bbb///cc
                    --> a
                    --> bbb
                    --> cc
           2: xxx
                    --> xxx
           3: yyy
                    --> yyy

   Program source

       #include <stdio.h>
       #include <stdlib.h>
       #include <string.h>

       int
       main(int argc, char *argv[])
       {
           char *str1, *str2, *token, *subtoken;
           char *saveptr1, *saveptr2;
           int j;

           if (argc != 4) {
               fprintf(stderr, "Usage: %s string delim subdelim\n",
                       argv[0]);
               exit(EXIT_FAILURE);
           }

           for (j = 1, str1 = argv[1]; ; j++, str1 = NULL) {
               token = strtok_r(str1, argv[2], &saveptr1);
               if (token == NULL)
                   break;
               printf("%d: %s\n", j, token);

               for (str2 = token; ; str2 = NULL) {
                   subtoken = strtok_r(str2, argv[3], &saveptr2);
                   if (subtoken == NULL)
                       break;
                   printf(" --> %s\n", subtoken);
               }
           }

           exit(EXIT_SUCCESS);
       }

       Another example program using strtok() can be found in getaddrinfo_a(3).

SEE ALSO
       index(3), memchr(3), rindex(3), strchr(3), string(3), strpbrk(3), strsep(3), strspn(3), strstr(3), wcstok(3)

COLOPHON
       This page is part of release 3.44 of the Linux man-pages project.  A description of the project, and information about
       reporting bugs, can be found at http://www.kernel.org/doc/man-pages/.



GNU                                                       2012-05-10                                                STRTOK(3)
    vim:tw=78:ts=8:ft=man:norl:
