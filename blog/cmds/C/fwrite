size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
注意：这个函数以二进制形式对文件进行操作，不局限于文本文件
返回值：返回实际写入的数据块数目
（1）buffer：是一个指针，对fwrite来说，是要输出数据的地址；
（2）size：要写入内容的单字节数；
（3）count:要进行写入size字节的数据项的个数；
（4）stream:目标文件指针；
（5）返回实际写入的数据项个数count。****
说明：写入到文件的哪里？ 这个与文件的打开模式有关，如果是w+，则是从file pointer指向的地址开始写，替换掉之后的内容，文件的长度可以不变，stream的位置移动count个数；如果是a+，则从文件的末尾开始添加，文件长度加大。
fseek对此函数有作用，但是fwrite[1]函数写到用户空间缓冲区，并未同步到文件中，所以修改后要将内存与文件同步可以用fflush（FILE *fp）函数同步。


fwrite(&s, sizeof(s), 1, stream);  //将string s 中的 1 个sizeof(s)长度写入stream. 返回count数1
