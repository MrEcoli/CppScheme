


(define (sq n) (* n n))
(define (even? n) (= (remainder n 2) 0))
(define (odd? n) (= (remainder n 2) 1))

(define square sq)

;支持高阶函数
(define (accumulate op initial sequence)
    (if (null? sequence)
        initial
        (op (car sequence)
            (accumulate op initial (cdr sequence)))))

(define (map p sequence)
    (accumulate (lambda (g h) (cons (p g) h)) '() sequence))

;(map even? (list 1 2 3 4 5 6 7))
;(map odd? (list 1 2 3 4 5 6 7))


(define (flatmap proc seq)
  (accumulate append '() (map proc seq)))

(define (remove seq a)
  (if (= (car seq) a)
    (cdr seq)
    (cons (car seq) (remove (cdr seq) a))))

(define (permutations s)
  (begin
    ;(display s)
    ;(newline)
    (if (null? s)
      (list '())
      (flatmap 
        (lambda (x)
           ;(display x)
           ;(newline)
            (map 
                (lambda (p) 
                    ;(display x) 
                    ;(newline) 
                    (cons x p))
                    (permutations (remove s x))))
         s))))


(permutations '(1 2 3))

(length (permutations '(1 2 3)))

;(flatmap (lambda (x) (cdr x)) (list (list 1 2 3) (list 3 4 5)))



; ex1.16
(define (f x)
      (cond ((= x 0) 0)
            ((even? x) (f (/ x 2)))
          (else (f (- x 1)))))
(define (fast-pow b n a)
  (cond ((= n 0) a)
        ((even? n) (fast-pow (sq b) (/ n 2) a))
        (else (fast-pow b (- n 1) (* a b)))))

(fast-pow 2 1 1)
(fast-pow 2 9 1)
(fast-pow 2 10 1)
(fast-pow 2 11 1)


; ex1.17

(define (double n) (+ n n))
(define (halve n) (/ n 2))


; ex 1.18
(define (fast-mul a b res)
  (cond ((or (= a 0) (= b 0)) 0)
        ((= b 1) (+ res a))
        ((even? b) (fast-mul (double a) (halve b) res))
        (else
         (fast-mul a (- b 1) (+ res a)))))


(define (fib n)
  (fib-iter 1 0 0 1 n))
 
(define (fib-iter a b p q count)
  (cond ((< count 1) b)
        ((even? count)
         (fib-iter a
                   b
                   (+ (square p) (square q))  ; p'
                   (+ (* 2 p q)  (square q))  ; q'
                   (/ count 2)))
        (else (fib-iter (+ (* b q) (* a q) (* a p))
                        (+ (* b p) (* a q))
                        p
                        q
                        (- count 1)))))


(fib 1)
(fib 2)
(fib 3)
(fib 4)
(fib 5)
(fib 6)

; gcd

(define (gcd a b)
  (if (< b 1)
      a
      (gcd b (remainder a b))))

(gcd 11 121)

(gcd 123 9)

(gcd 123123 123)

; fermat test

(define (expmod base e m)
  (cond ((= e 0) 1)
    ((even? e)
      (remainder (expmod base (/ e 2) m) m))
    (else
      (remainder (* base (expmod base (- e 1) m)) m))))

(expmod 2 5 1)

(expmod 2 11 11)

(expmod 2 11 3)


(define (fermat-test n)
  (define (try-it a)
    (= (expmod a n n) a))
  (try-it (+ 1 (random (- n 1)))))

(fermat-test 10)

(define (compose f g)
  (lambda (x) (f (g x))))

(define (mul x y) (* x y))

(mul 112 123)

(define (inc x) (+ x 1))
(inc 11)
(square 11)

(define func (compose square inc))

func

(func 111)



(define (gcd a b)
  (if (= b 0)
    a
    (gcd b (remainder a b))))


(define (sum term a nxt b)
  (if (> a b)
      0
      (+ (term a) (sum term (nxt a) nxt b))))


;支持高阶函数
(define (accumulate op initial sequence)
    (if (null? sequence)
        initial
        (op (car sequence)
            (accumulate op initial (cdr sequence)))))

(define (map p sequence)
    (accumulate (lambda (x y) (cons (p x) y)) '() sequence))

(define (flatmap proc seq)
  (accumulate append `() (map proc seq)))

(define (remove seq x)
  (if (= (car seq) x)
    (cdr seq)
    (cons (car seq) (remove (cdr seq) x))))

(define (permutations s)
  (if (null? s)
    (list `())
    (flatmap (lambda (x)
           (map (lambda (p) (cons x p))
            (permutations (remove s x))))
         s)))

;(permutations '(1 2 3))




;integral function

(define (integral f a b dx)
  (define (integral-next x) ;
    (+ x dx))
  (* dx (sum f (+ a (/ dx 2.0)) integral-next b)))


; ex 1.29

(define (integral-simpson f a b n)
  (define (h)
    (/ (- b a) n))
  (define (simpson-term a)
    (+ (* 2 (f a)) (* 4 (f (a+h)))))
  (define (simpson-next a)
    (+ a (* 2 h)))
  (* (+ (f a) (f b) (* 4 (f (+ a h))) (sum simpson-term (+ a (* 2 h)) simpson-next b) (/ h 3))))




(define us-coins (list 50 25 10 5 1))
(define uk-coins (list 100 50 20 10 5 2 1))

(define a (list 1 2 3))
(or (null? a) (> (car a) 0))


(define (nomore coin) (null? coin))
(define (except-first coin) (cdr coin))
(define (fisrt-coin coin) (car coin))

(define (cc amount coin-val)
  (begin ;(display amount)
    ;(newline)
    ;(display coin-val)
    ;(newline)
  (cond ((= amount 0) 1)
    ((or (< amount 0) (nomore coin-val)) 0)
    (else
      (+
        (cc amount (except-first coin-val))
        (cc (- amount (fisrt-coin coin-val)) coin-val))))))


(cc 50 us-coins)
;50

(cc 40 us-coins)
;31


(define (filter proc seq)
  (cond ((null? seq) '())
        ((proc (car seq))
         (cons (car seq) (filter proc (cdr seq))))
        (else
         (filter proc (cdr seq)))))



(filter even? (list 1 2 3 4 5))

;'(2 4)


exit
(define (enumerate-interval x y)
    (if (> x y)
        '()
        (cons x (enumerate-interval (+ x 1) y))))
(define (adjoin-position new-row k rest-of-queens)
    (cons new-row rest-of-queens))

(define (queens board-size)
    (define (queen-cols k)
        (if (= k 0)
            (list '())
            (filter
                (lambda (positions) (safe? k positions))
                (flatmap
                    (lambda (rest-of-queens)
                        (map (lambda (new-row)
                            (adjoin-position new-row k rest-of-queens))
                        (enumerate-interval 1 board-size)))
                    (queen-cols (- k 1))))))
    (queen-cols board-size))

(define (safe? k positions)
    (let ((val (car positions)))
        (define (iter x others)
            (cond ((null? others) #t)
                ((or
                    (= (car others) val)
                    (= (abs (- x k)) (abs (- val (car others)))))
                #f)
                (else
                    (iter (+ x 1) (cdr others)))))
    (iter (+ k 1) (cdr positions))))





exit








(define (expmod base exp m)
    (cond ((< exp 1) 1)
        ((even? exp)
            (remainder (expmod base (/ exp 2) m) m))
        (else
            (remainder (* base (expmod base (- exp 1) m)) m))))
(expmod 2 10 11)


(define (fermat-test n) 
  (define (try-it a)
    (= (expmod a n n) a))
  (try-it (+ 1 (random (- n 1)))))

(fermat-test 10)




(define (fib n)
  (fib-iter 1 0 0 1 n))
 
(define (fib-iter a b p q count)
  (cond ((< count 1) b)
        ((even? count)
         (fib-iter a
                   b
                   (+ (square p) (square q))  ; p'
                   (+ (* 2 p q)  (square q))  ; q'
                   (/ count 2)))
        (else (fib-iter (+ (* b q) (* a q) (* a p))
                        (+ (* b p) (* a q))
                        p
                        q
                        (- count 1)))))

;(define (gcd a b) (if (< b 1) a (gcd b (remainder a b))))

;(define (f x) (define (g n) (+ n n)) (g x))

;(define (f x)  (cond ((< x 3) (display 3) (+ 3 3)) ((> x 10) (display 10) (+ 10 10)) (else (display x) (+ x x))))
