(define (f x)
	(define (g a) (+ a a))
	(g x))


(define (new-if ifexp thenexp elseexp)
	(cond (ifexp thenexp)
		(else elseexp))

(define (pyth& x y k)
 (*& x x (lambda (x2)
          (*& y y (lambda (y2)
                   (+& x2 y2 (lambda (x2py2)
                              (sqrt& x2py2 k))))))))


(define (fact-cps n k)
  (if (= n 0)
      (k 1)
      (fact-cps (- n 1)
                (lambda (v)
                  (k (* n v))))))

(define retry #f)

(define factorial
  (lambda (x)
    (if (= x 0)
        (call/cc
         (lambda (k)
           (set! retry k)
           1))
        (* x (factorial (- x 1))))))