

;支持comment

;在";"和它的下一个换行符之间的字符作为comment，解释器不进行处理

;支持define语句
;define获得的variable会被注册到map中
;全局变量的格式为std::map<std::string, Object*> GloableVariable;
;数据的类型被定为Procedure，Variable，Boolean类
;为了简化这个程序，数值只支持Double类型
;Object是Procedure和Variable类的基类

;Boolean类以
#t
#f
;表示


(define v 3)

(define (v) 3)	;v是一个procedure


;下面二者的定义应该是一致的,f被定义为一个procedure
(define f (lambda (x) (+ x x)))

(define (f x) (+ x x))

(f 3)
;6


;支持if语句,if语句的格式为if (cond) (then) (else)
;3个模块组成，分别为cond,then和else

(define (fib x) (if (< x 3) 1 (+ (fib (- x 1)) (fib (- x 2)))))

(define x (+ 3 36 3 5))

(define (x) (+ 3 36 3 5))






;hello world
(display fib)
;#<procedure:f>



;支持let语句
;let语句获得变量是局部的，但是同时他是能够访问全局变量
(let ((loop (lambda (x) (+ x x)))) (loop 10))


;支持内置函数display
(display "hello world")

