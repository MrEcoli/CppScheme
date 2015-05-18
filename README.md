# CppScheme
Implemention of Scheme Interpreter with C++

In Process



## Scheme的范式

Scheme的范式比较简单，主要的范式包括

范式|描述 
:---------------|:---------------
number|常量数值
variable|对象，可以是变量或过程
(if else expr)|条件语句
(define v expr)|定义语句，组成一个对象到当前环境
(proc arg ...)|过程调用语句

延伸出来的一些语法包括
语法|描述 
:---------------|:---------------
(lambda (arg ...) (expr)|					定义一个匿名函数
(let ((v1 exp1) (v2 exp2) ...) (expr)|定义一个匿名函数过程，并且立刻调用
(cond (c1 r1) (c2 r2) ... (else rx))|多条件语句






##CppScheme基本的语言特性

CppScheme支持的语言特性
####1. 支持define命令，变量将加入全局变量中

	(define v 3)
    
    (define (f x) (+ x x))
以上的声明方式中，f与g是等同的
####2. 采用逆波兰序计算

    (+ （+ 1 2） (- 3 4)) ;结果为(+ 3 -1)->2
	

####3. 支持if语句

if语句以if关键词起始，后面跟随3个模块，if (cond) (then) (else)

    (define (fib x) 
    	(if (< x 3)
    	1
    	(+ (fib (- x 1)) (fib (- x 2)))))

####4. 支持lambda表达式的匿名函数

下面的两种声明方式获得的g和f函数时等价的

    (define f (lambda (x) (+ x x)))

	(define (g x) (+ x x))

####5. 支持let语句

let语句等同于直接调用lambda函数的一个语法糖，在变量赋值阶段就是确定lambda函数的变量的值

	(define f (let ((x 1)
			  (y 2))
	          (+ x y )))

####6. 支持cond语句
cond语句依次判定条件，正确则执行判断语句对应的表达式，如果错误则对下一条条件语句进行判定，如果没有值为true的条件，则返回else关键词所在的表达式的执行结果

	(define (* a b)
	  (cond ((or (= a 0) (= b 0)) 0)
            ((= b 1) a)
            ((even? b) (* (double a) (halve b)))
            (else
                  (+ a (* a (- b 1))))))

	

----------


##代码清单

####1. Parser.h
包含了TOKEN类型，词法分析函数，可以通过**DEBUG_TOKEN**开启TOKEN分析结果提示



define语句的eval不生成一个新的Object对象，它的返回一个nullptr指针，因而可以，在parseExpAST时，如果接受了一个define语句，应该进行继续的parse.

	(define f (lambda (x) (+ x x)))

以上的语句返回一个DefineExp指针，调用它的eval指针返回指向nullptr的Object*指针。

####2. slist.h
通过一个_list_node结构体保存指向上一层局部环境的指针，和一个指向当前环境变量组成的map的指针

![Enviroment list](https://github.com/MrEcoli/CppScheme/blob/master/Data/Enviroment.jpg)