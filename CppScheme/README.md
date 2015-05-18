
#miniScheme文档

----------

##基本的语言特性


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

	

----------


##代码清单

####1. Parser.h
包含了TOKEN类型，词法分析函数，可以通过**DEBUG_TOKEN**开启TOKEN分析结果提示



define语句的eval不生成一个新的Object对象，它的返回一个nullptr指针，因而可以，在parseExpAST时，如果接受了一个define语句，应该进行继续的parse.

	(define f (lambda (x) (+ x x)))

以上的语句返回一个DefineExp指针，调用它的eval指针返回指向nullptr的Object*指针。

