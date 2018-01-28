;Question 1:
;test cases:
;(xmember '1 '(1))
;(xmember '1 '( (1) 2 3))
;(xmember '(1) '((1) 2 3))
;(print (eq '(1) (car '((1) 2 3))))

(defun xmember  (X Y )
	(cond 		
		((equal Y nil) nil)
		((null Y) nil)
		((equal (car Y) X) T)
		(t (xmember X (cdr Y)))
                )
        
)
;Test Cases:

(print(xmember nil nil))
(print(xmember nil '(nil)))
(print(xmember nil '((nil)) ))
(print(xmember '(nil) '(1 2 3 (nil))))
(print(xmember '(nil) '(nil)))

;Question 2:
;Test Cases:

(print(flatten '(a (b c) d)))
(print(flatten '((((a))))))
(print(flatten '(a (b c) (d ((e)) f))))

(defun flatten (x)
    (cond
        ((null x) x)
        ((atom (car x)) (cons (car x) (flatten (cdr x))))
        (t (append (flatten (car x)) (flatten (cdr x))))
    )
)


;Qustion 3:


(defun compare(x y)
	(cond
		((null y) nil)
		((equal x (car y)) print"T" )
		(t (compare x  (cdr y)))
	)
)
(defun remove-duplicate (x)
	(cond
		((null x) x)
		( (equal (xmember (car x) (cdr x)) T)  (remove-duplicate (cdr x)))
		(t (cons (car x) (remove-duplicate (cdr x))))
	)
       
)

;Test case
(print(remove-duplicate '(a b c a d b)))

;(print(remove-duplicate '(a b c a d b)))

;Question 4:	 

(defun mix(x y)
	(cond
		((equal nil x) y)
		((equal nil y) x)
		((null x)  y)
		((null y)  x)
		(t (cons (car x) (mix y (cdr x))))
                )
)
(print(mix '(a b c) '(d e f)))
(print(mix '(1 2 3) '(a)))
(print(mix '((a) (b c)) '(d e f g h)))
(print(mix '(1 2 3) nil))
(print(mix '(1 2 3) '(nil)))
			
;Qustion 5:
(defun filter(x y)
        ;(print x)
        ;(print y)
        (cond
                ;(t (print 2222))
		((null y) y)
		((equal nil (cdr y)) (list x (append x y)))
		(t (append (filter (append x (list(car y))) (cdr y)) (filter x (cdr y))))
                )
)
(defun subsets(x)
	(cond
               
                ;((equal nil x) (list nil))
		((equal nil (cdr x)) (list x))
		( t (append  (subsets (cdr x)) (filter (list(car x)) (cdr x))))
                )
)
(defun allsubsets (x)
  (cond
        ((equal nil x) (list nil))
        (t (append  (subsets x) (list nil)))
        )
)
;Test cases:
(print(allsubsets nil))
;(print(equal nil (cdr '(a))))
(print(allsubsets '(a)))
(print(allsubsets '(a b c d)))
(print(cdr '(1 2)))

;Question 6
;order not matter
;self refer self is not useful