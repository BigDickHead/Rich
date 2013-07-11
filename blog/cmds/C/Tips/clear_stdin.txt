scanf, cin 溢出问题

//C++ 解决方法
char greet[20];
cin.width(12);
cin >> greet;
cout << greet << '\n';

//C 解决方法
fgets(str, sizeof(str), stdin);


================================================================================
清空stdin缓冲区, 防止对后续输入的影响
Windows
fflush(stdin)

char ch;
while((ch=getchar())!='\n'&&ch!=EOF);

--------------------------------------------------------------------------------
Linux
setbuf(stdin, NULL);

char ch;
while((ch=getchar())!='\n'&&ch!=EOF);
