
# defects 记录

## 一、SIGABRT崩溃
堆越界
+	double free/free 没有初始化的地址或者错误的地址
+	strcpy sprintf strcmp memcpy等越界访问读写

assert abort函数

## 二、SIGFPE崩溃
(上/下)溢出 
除零

## 三、SIGSEGV
+ 1.试图对只读映射区域进行写操作 。
+ 2.访问的内存已经被释放，也就是已经不存在或者越界。

Segment Fault. The possible cases of your encountering this error are:
（1）buffer overflow --- usually caused by a pointer reference out of range. 
（2）stack overflow --- please keep in mind that the default stack size is 8192K.
（3）illegal file access --- file operations are forbidden on our judge system.

## 四、SIGBUS
1.硬件故障，不用说，程序员最常碰上的肯定不是这种情形。
2.Linux平台上执行malloc()，如果没有足够的RAM，Linux不是让malloc()失败返回， 而是向当前进程分发SIGBUS信号。
3.某些架构上访问数据时有对齐的要求，比如只能从4字节边界上读取一个4字节的 数据类型。IA-32架构没有硬性要求对齐，尽管未对齐的访问降低执行效率。另外一些架构，比如SPARC、m68k，要求对齐访问，否则向当前进程分发SIGBUS信号。
4.试图访问一块无文件内容对应的内存区域，比如超过文件尾的内存区域，或者以前有文件内容对应，现在为另一进程截断过的内存区域。

SIGBUS的缺省行为是终止当前进程并产生core dump。
SIGBUS与SIGSEGV信号的一般区别如下:
1.SIGBUS(Bus error)意味着指针所对应的地址是有效地址，但总线不能正常使用该指针。通常是未对齐的数据访问所致。
2.SIGSEGV(Segment fault)意味着指针所对应的地址是无效地址，没有物理内存对应该地址。

## 五、栈里显示函数为问号
1.源代码和可执行程序版本不一致；
2.没有符号表；
3.gdb版本比gcc版本老，有些内容无法解析；
？？.=》栈破坏corrupt stack
	strcpy sprintf strcmp memcpy等造成
	snprintf strncpy虽然提供了安全拷贝，但是使用不当仍然会造成栈破坏


## 六、glib 程序报错到终端
``` log
1.*** Error in `/app/bin/demo': double free or corruption (out): 0xb6646800 ***
	// 两次free
2.*** Error in `/app/bin/demo': malloc(): smallbin double linked list corrupted: 0xb5c1f070 ***
  // ？？？？头文件和库文件不一致（https://blog.csdn.net/shaochat/article/details/40817313）？？？
3.*** Error in `/app/bin/demo': free(): invalid next size (fast): 0xb55191a8 ***
	// strncpy堆越界后（必须越界很多，导致档案信息被覆盖），free时触发
4.*** Error in `/app/bin/demo': corrupted double-linked list: 0xb5d18fd0 ***

5.*** Error in `/app/bin/demo': realloc(): invalid next size: 0xb4220018 ***
	// strncpy堆越界后（必须越界很多，导致档案信息被覆盖），realloc时触发
```

档案信息  
使用malloc动态分配内存时，操作系统为这段空间建立一个档案信息，用于记录此空间的大小、位置等。
```
·············· 高地址    
档案信息  
··············  
分配的空间  
·············· 低地址  
```
从开始地址使用时，是往上增长的，这就可能覆盖档案信息了。释放时，系统会根据指针地址查找档案信息，来释放相应的空间。由于被覆盖，找不到相应的档案信息，或者档案信息不准确。就是提示所说的，地址无效或则空间大小无效。


## 当把Foo\*\*转换成const Foo\*\*时为什么会出错？

因为把Foo\*\*转换成const Foo\*\*是非法且危险的。

C++允许Foo*到const Foo*的转换（这是安全的）。但如果想要将Foo\*\*隐式转换成const Foo\*\*则会报错。

这么做的原因如下所示:  
但首先，这里有个最普通的解决办法：只要把const Foo**改成const Foo* const*就可以了。
```
 class Foo { /* ... */ };
 
 void f(const Foo** p);
 void g(const Foo* const* p);
 
 int main()
 {
   Foo** p = /*...*/;
   ...
   f(p);  // 错误：将Foo**转换成const Foo**是非法且邪恶的
   g(p);  // 可以：将Foo**转换成const Foo* const*是合法且合理的
   ...
 }
```
之所以Foo\*\*到const Foo\*\*的转换是危险的，是因为这会使你没有经过转换就在不经意间修改了const Foo对象。
```
 class Foo {
 public:
   void modify();  // 修改this对象
 };
 
 int main()
 {
   const Foo x;
   Foo* p;
   const Foo** q = &p;  // 这时q指向p；（幸亏）这是个错误。
   *q = &x;             // 这时p指向x
   p->modify();         // 啊：修改了const Foo！！
   ...
 }
```
记住：请不要用指针转换绕过这里。别这么做就是了！ 

## 混用unix文件接口和glibc接口
不要混用unix文件接口(open/read/write/close)和glibc文件接口（fopen/fread/fwrite/fclose）

问题举例：  
先用mkstemp打开一个临时文件，返回fd，然后转FILE *，fwrite后，先close了fd，再fclose

症状：  
fwrite了size个字节，stat得文件长度length，length < size

原因：  
fclose会先把缓冲区的内容flush到磁盘，再关闭。close直接关闭了文件，缓冲区的内容丢失了。

## set_new_handler
C语言的malloc/realloc/calloc失败没有系统提供回调函数处理（相比C++的new失败了有set_new_handler），需要程序员自己封装malloc等函数来实现

## OOM killer
Out Of Memery  
linux上因为有OOM等，一般程序是不用检查malloc是否返回NULL。而一定要保证稳定性，持续存在的程序，必须要判断malloc返回值

