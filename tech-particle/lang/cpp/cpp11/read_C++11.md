# Understanding C++11 ReadNOte

## note
```cpp
vector<list<int>> veclist; // C++11 有效；C++98/03报错>>被解释为右移 

// 强类型枚举
enum class Color {red, blue, green};
int x = Color::red;     // C++11报错，Color不能转int
Color y = 7;            // C++11报错，int不能转Color
Color z = red;          // C++11报错，red不在作用域
Color c = Color::red;   // C++11正常；C++98/03报错

// 指示编译器，该文件只编译一次
/* 同头文件预编译#ifndef-define-endif */
#pragma once

// 操作符_Pragma，效果同#pragma，但可以用在宏中
_Pragma("once");

// 日志
#define LOG(...) { \
    fprintf(stderr, "%s: Line %d: \t", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
}

/* C++引入C头文件编译c++03被预定义位199711L；C++11被预定义为201103L
    #ifdef __cplusplus
    extern "C" {
    #endif
    // ... 
    #ifdef __cplusplus
    }
    #endif
*/
#ifdef __cplusplus < 201103L 
    #error "should use C++11 implementation"
#endif

// 禁止包含某些头文件，并提示包含其包装头文件
// 这是一种断言
#ifndef __DEST_H__
#error "Never use <this header file> directly; include <dest.h> instead."
#endif

/* 静态断言小技巧 
    原assert是用在运行时的，有些断言最好应该用在编译时。
    比如模板类型的sizeof运算值是否相等，进而决定是否拷贝。
*/
#define assert_static(e) \
do { \
    enum { assert_static__ = 1/(e) }; \
} while (0)
/* 编译时判定expr，如果否则编译失败，输出msg
    expr必须是常量表达式
*/
static_assert(expr, msg);

/* noexcept修饰符与noexcept操作符
    C++11弃用了动态异常声明，使用新的noexcept
    noexcept/noexcept(expr=true)修饰的函数如果抛出异常，则程序被终止运行
    noexcept比throw更高效，广泛使用于标准库 */
    void excpt_func() throw(int, double) { ... } // 弃用异常规范
    void excpt_func() noexcept; // 相当于noexcept(true)，相当于原throw()
    void excpt_func() noexcept(expr); // expr - 常量表达式：true - 不会抛异常； false - 会抛出异常
/* noexcept(expr)操作符通常用于模板 */
    template <class T>
        void fun() noexcept(noexcept(T())) {} 
/* 第二个noexcept是一个操作符，fun()是否是noexcept修饰函数取决于T()是否会抛出异常。
    当T()表达式会抛出异常（即noexcept(false)修饰函数），noexcept操作符返回false；否则为true。
    这样设计保证了通过表达式推导达到编译时判断泛型编程函数是否会抛出异常 */
/* 注意：noexcept修饰函数默认通过std::terminate调用终止异常函数，无法保证对象析构函数调用及栈自动释放。大多数时候，暴力终止程序是简单有效的。
    noexcept保证应用程序的安全的同时，提高标准库的性能，解决了throw修饰符异常扩散问题。
    C++11默认将类析构函数/delete函数/delete[]函数设置成noexcept，因为其不应抛出异常。
    相反，new函数/new[]函数声明成noexcept(false)的，原来是throw(std::bad_alloc)
*/

```

## 类型
1. 支持long long  
之前只加入了C99，被很多编译器支持。至少64位，常量使用LL后缀，无符号亚型使用ULL后缀。

2. 窄字符串转宽字符串
连接宽字符串和窄字符串，C++11标准编译器先把窄字符转成宽字符串，再连接。

## 类
### 快速出初始化成员变量
C++11允许类/结构体的非静态成员变量使用=或者{}进行就地初始化（头文件中初始化），类似构造函数使用()的初始化列表，后者作用在后面。非常量的静态成员变量需要在头文件外初始化。
### 非静态成员变量
C++11允许sizeof作用于类的非静态成员，像静态成员一样访问。
C++98中需要使用`sizeof(((Class*)0)->menber)`来计算。
### friend
C++11声明类的友元，不再需要class关键字，也可以使用typedef的别名
扩展了泛型编程中，类的友元的使用。也方便地把测试类当作目标类的友元。
### final/override
C++11（向Java学习的？）使用final关键字阻止基类虚函数被重写。
C++11使用override描述符修饰派生类重载函数，指示编译器该派生类是否有对应的基类虚函数（检查函数名/原型/是否虚函数，不检查参数变量名），如果没有则报错。虽然不能避免重载了另一个基类虚函数而不是程序员期望的，依然具有实际意义。
只用于函数名后，仍可做变量名，最好避免使用。
### using继承
子类中声明`using Base::Base;`和`using Base::func;`用于继承基类的构造函数和普通函数，就不用再透传基类的构造函数了，而且C++11隐式声明了继承构造函数，比原来更节省代码。
带有默认参数的构造函数，不会继承其默认值，但会继承多个形式的构造函数。
当派生类的多个基类的构造函数冲突时，需要显式声明解决了。
私有构造函数不会被继承。
使用了using继承构造函数，编译器不会再为派生类生成默认构造函数，如果基类没有显式的默认构造函数，则派生类没有默认构造函数，`Class c;`声明就编译报错。
### 委托构造函数
`Class(int val) : Class() { _val = val; };`，C++11可以在构造函数的初始化列表位置委派另一个构造函数。不能同时使用委派构造函数和初始化列表。
委托构造函数会形成链状委托构造，不能形成环形。
这样，可以使用构造模板函数产生目标构造函数，使构造函数的泛型编程成为可能。
委派构造函数可以使用try包裹，并catch捕获异常，只做跟踪，更外面不被处理的话，仍会终止运行。


## 泛型
### 函数模板的默认模板参数
可以用=提供类模板/函数模板的默认模板参数及默认值。
模板参数不是由模板函数的默认形参推导出来的，而是由函数的实参推导的。
### 外部模板
同一函数模板在不同源文件中被实例化，链接在一起的时候报重名错误。以前，编译器需要去除重复的模板实例化。类似于extern声明全局变量一样，如`extern template void fun<int>(int);`（一般放在头文件里），使用其他源文件里的函数模板的显式实例化（所有源文件里唯一）。这样链接器的工作变得简单，缩短了编译时间（模板展开的开销十分可观，在某些大型项目）。
外部模板声明不能用于静态函数，可用用于类静态成员函数。
### 局部/匿名类型做模板实参
C++11支持了局部类型/匿名类型做模板实参。如匿名结构体/联合体/枚举。
但不支持匿名结构体直接嵌在模板实参位置。

**类型收窄问题**

## 类型推导
### auto
auto [var name] = [var value];

好处：
+ 简化迭代器书写（之前迭代器都很长）
+ 泛型编程扩展性提高（之前不同的模板类的函数只能返回相同的类型）

限制：
+ auto推导，必须是对变量初始化
+ 不能用于函数参数
+ 不能用于类的非静态成员变量
+ 不能用于数组
+ 不能用于模板参数

## decltype
decltype([exp]) [varname] = [value];

## 智能指针