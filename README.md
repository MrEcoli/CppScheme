# CppScheme
Implemention of Scheme Interpreter with C++

------

###代码清单

####1. Lexer.h
包含了TOKEN类型，词法分析函数，可以通过**DEBUG_TOKEN**开启TOKEN分析结果提示

####2.Parser.h
词法分析

####3. slist.h
`slist`包含了一个指向`_list_node`的指针，`_list_node`是单链表结构的一个节点为，CppScheme的应用场景中，我们以**`std::map<std::string, Object*>`**保存局部环境的所有变量，`_list_node`中的成员`ptr_to_tree`指向一个`std::map<std::string, Object*>`。

**注:** 由于我们表示局部环境的slist结构只含有一个指针，对它的直接拷贝的效率与拷贝指针是一样的，因而CppScheme中所有的解释环境的传递都是通过传值传递完成的。

    template<typename T>
    class _list_node{
    public:
        T* ptr_to_tree;
        _list_node<T>* next;
    };

![Enviroment list](https://github.com/MrEcoli/CppScheme/blob/master/Data/Enviroment.jpg)


#### 4. Object.h

CppScheme中定义的对象都继承自Object. CppScheme实现的对象有

名称|描述 
:---------------|:---------------
**DoubleValue**|浮点数
**IntegerValue**|整型数
**BoolValue**|布尔值，字面表示为`#t`, `#f`
**StringValue**|字符串
**SymbolValue**|标示符
**VectorValue**|数组,以`std::vector<Object*>`实现
**Procedure**|过程/函数
**NullValue**|一个单例，用于模拟`'()`
**CharcterValue**|单个字符
**Pair**|序对，含有两个`Object*`指针


#### 5. ExpAST.h

抽象语法树的节点，`ExpAST`为基类，其他节点继承自`ExpAST`,所有基层ExpAST的类必须实现`eval`,`deep_copy`以及`factory`。`eval`的原型`virtual Object* eval(EnvTreeList* )`。`deep_copy`实现对象的深拷贝，而`factory`则是静态函数，它生成一个新的对象，并将指针注册到`ExpAST_pool`中。

名称|描述 
:---------------|:---------------
**ExpAST**|所有节点的抽象基类
**SimpleExp**|表示`DoubleValue`,`IntegerValue`的节点
**NullExp**|表示`'()`的
**StrExp**|表示字符串
**SymbolExp**|表示一个标示符
**VaraiableExp**|表示一个变量
**ProcedureExp**|表示`Lambda`表达式的表达式
**CallProcedureExp**|调用过程的表达式
**DefineVariableExp**|定义变量的表达式
**DefineProcedureExp**|定义过程的表达式
**CondExp**|表示`Cond`表达式
**IfElseExp**|表示`if`表达式


#### 6. gc.h
实现了CppScheme的垃圾回收。CppScheme的垃圾回收通过标记清除方式实现。程序中所有的`Object`,`ExpAST`都通过它们的factory函数生成，生成后，`Object`的指针被注册到`std::map<Object*, bool> Obejct_pool`，ExpAST的指针被注册到`std::map<ExpAST*, bool> ExpAST_pool`。程序通过DFS`GlobalVariable`所能访问到的对象，并标记，之后清除所有不会访问到的内存块。

#### 7. built_in.h
包括了所有的内置函数，`built_in`对象继承自Object。

#### 8. Debug.h
定义了多个调试宏，开启后可以显示`token`分析，内存分配和清除情况

#### 9. miniScheme.cpp
主程序所在

---
###Scheme的范式

Scheme的范式比较简单，主要的范式包括

范式|描述 
:---------------|:---------------
**number**|常量数值
**variable**|对象，可以是变量,过程或是标示符
**(if else expr)**|条件语句
**(define v expr)**|定义语句，组成一个对象到当前环境
**(proc arg ...)**|过程调用语句

延伸出来的语法包括

语法|描述 
:---------------|:---------------
**(lambda (arg ...) [expr])**|                  定义一个匿名函数
**(let ((v1 exp1) (v2 exp2) ...) (expr)**|定义一个过程，并且立刻调用
**(cond (c1 r1) (c2 r2) ... (else rx))**|条件语句


各类quote的定义

语法|描述 
:---------------|:---------------
**'**|常量引用 **'**(**datum ...**)与(**quote datum ...**)的语法是一致的
**`**|准引用 **`**(**datum ...**)与(**quasiquote datum...**)一致
**,**|与准引用一同使用
**\#**|取决于随后的字符
**.**|表示点对

Scheme的内置的对象类型

名称|描述 
:---------------|:---------------
**pair**|点对/序对，可用谓词pair?判断
**number**|数值，可用谓词number?判断
**boolean**|bool值，#t和#f,可用boolean?判断
**charctor**|字符，可用谓词char?判断
**symbol**|常量标示符，可用谓词symbol?判断
**vector**|数组，可用谓词vector?判断
**port?**|CppScheme不支持
**procedure**|过程/函数，可用谓词procedure?判断

