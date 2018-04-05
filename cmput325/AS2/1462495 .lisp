; Bowei Wang 1462495
;cmput325 assignment 2


;check the name of the function 
(defun check_func_name (x y)
  (cond 
  	((eq x (caar y)) (car y))
        ((null y) nil)
	(t (check_func_name x (cdr y)))
  )
)

;function in assignment 1 to check member
(defun xmember  (X Y)
	(cond 		
		((equal Y nil) nil)
		((null Y) nil)
		((equal (car Y) X) T)
		(t (xmember X (cdr Y)))
                )
        
)

;function in assignment 1 to flatten list
(defun flatten (x)
    (cond
        ((null x) x)
        ((atom (car x)) (cons (car x) (flatten (cdr x))))
        (t (append (flatten (car x)) (flatten (cdr x))))
    )
)
;namelist of P
(defun name_list (P) 
  (cond 
        ((null P) nil)
        (t (cons (caar P) (name_list (cdr P))))))
;function that takes replace the variables in E
;x is the name of argument, y is corresponding value
(defun e_replace (x y E P)
  (cond
  	((null E) nil)
        ((atom (car E)) (if (eq x (car E))  (cons   (fl-interp y P) (e_replace x y (cdr E) P)) (cons   (fl-interp (car E) P)  (e_replace x y (cdr E) P))))
	
	(t (cons (e_replace x y (car E) P)  (e_replace x y (cdr E) P)))
  )
)

;function defined to check whether EL has the name in PL
(defun exist_def (EL PL)
  (cond
        ((null EL) nil)
        ((xmember (car EL) PL) T)
        (t (exist_def (cdr EL) PL))))


;function to check userdefine functions that not yet been transfered
(defun check_loop (E P)
  
  (cond
        ((null E) nil)
        ((atom E) E)
        ((xmember (car E) (name_list P)) (cons (handle_user_def E P) (check_loop (cdr E) P)))
        ((atom (car E)) (cons (car E) (check_loop (cdr E) P)))
        (t (cons (check_loop (car E) P) (check_loop (cdr E) P)))))


;function to put element in x and y one by one into function e_replace to repalce the 
;corresponding variable in L
(defun putin_value(x L y P)
  (cond 
        ((null x) L)
        (t (putin_value (cdr x) (e_replace (car x) (car y) L P) (cdr y) P))))

;this function is defined to handle user define case and make a loop
(defun handle_user_def (E P)
  (if (not( exist_def (flatten E) (name_list P))) (fl-interp E nil)  
     (let((cur_function (check_func_name (car E) P)))
        (let ((func (putin_value (cadr cur_function) (cadddr cur_function) (cdr E) P)))
                                 
                                 (fl-interp (check_loop (fl-interp func P) P )nil)))))


(defun fl-interp (E P)
  (cond 
	((atom E) E)  ; %this includes the case where E is nil or a number
        (t
           (let ( (f (car E))  (arg (cdr E)) )
	      (cond 
                ; handle built-in functions
							
               
                ((eq f 'first)  (car (fl-interp (car arg) P)))
		((eq f 'rest)  (cdr (fl-interp  (car arg) P)))
		((eq f 'atom) (atom (fl-interp (car arg) P)))
		((eq f 'null) (null (fl-interp (car arg) P)))
		((eq f 'eq) (eq (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'equal) (equal (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'numberp) (numberp(fl-interp (car arg) P)))
		((eq f 'append) (append (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'car)  (car (fl-interp (car arg) P)))
		((eq f 'cdr) (cdr (fl-interp  (car arg) P)))
		((eq f 'cons) (cons (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'if) (if (fl-interp (car arg) P) (fl-interp (cadr arg) P) (fl-interp (caddr arg) P)))
		((eq f 'cond) (if (fl-interp (caar arg) P) (fl-interp (cadar arg) P) (fl-interp (cons f (cdr arg)) P)))
		((eq f 'let) (let (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'let*) (let* (fl-interp (car arg) P) (fl-interp (cadr arg) P)))

		((eq f 'mapcar) (mapcar (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'reduce) (reduce(fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'number) (numberp(fl-interp (car arg) P)))
		((eq f '+) (+ (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f '-) (- (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f '*) (* (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f '>) (> (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f '<) (< (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f '=) (= (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'and) (if (fl-interp (car arg) P) (if
                                                             (fl-interp (cadr arg) P) T nil) nil))
                
		((eq f 'or)  (if (fl-interp (car arg) P) T (if  
                                                               (fl-interp (cadr arg) P) T  nil)))
		((eq f 'not) (not (fl-interp (car arg) P)))
                ;handle unregular situations
		((null P) E)
                ;handle user define functions
		(t (handle_user_def E P)))))))


;; Example tests A2

;; PRIMITIVES (P1-P18)
(print (if (eq (fl-interp '(+ 10 5) nil) '15) 'P1-OK 'P1-error))
(print (if (eq (fl-interp '(- 12 8) nil) '4) 'P2-OK 'P2-error))
(print (if (eq (fl-interp '(* 5 9) nil) '45) 'P3-OK 'P3-error))
(print (if (not (fl-interp '(> 2 3) nil)) 'P4-OK 'P4-error))
(print (if (eq (fl-interp '(< 1 131) nil) 'T) 'P5-OK 'P5-error))
(print (if (eq (fl-interp '(= 88 88) nil) 'T) 'P6-OK 'P6-error))
(print (if (not(fl-interp '(and nil true) nil)) 'P7-OK 'P7-error))
(print (if (eq (fl-interp '(or 10 nil) nil) 'T) 'P8-OK 'P8-error))
(print (if (not(fl-interp '(not true) nil)) 'P9-OK 'P9-error))
(print (if (eq (fl-interp '(number 354) nil) 'T) 'P10-OK 'P10-error))
(print (if (eq (fl-interp '(equal (3 4 1) (3 4 1)) nil) 'T) 'P11-OK 'P11-error))
(print (if (eq (fl-interp '(if nil 2 3) nil) '3) 'P12-OK 'P12-error))
(print (if (eq (fl-interp '(null ()) nil) 'T) 'P13-OK 'P13-error))
(print (if (not(fl-interp '(atom (3)) nil)) 'P14-OK 'P14-error))
(print (if (eq (fl-interp '(eq x x) nil) 'T) 'P15-OK 'P15-error))
(print (if (eq (fl-interp '(first (8 5 16)) nil) '8) 'P16-OK 'P16-error))
(print (if (equal (fl-interp '(rest (8 5 16)) nil) '(5 16)) 'P17-OK 'P17-error))
(print (if (equal (fl-interp '(cons 6 3) nil) (cons 6 3)) 'P18-OK 'P18-error))
;; more complex (P19-P22)
(print (if (eq (fl-interp '(+ (* 2 2) (* 2 (- (+ 2 (+ 1 (- 7 4))) 2))) nil) '12) 'P19-OK 'P19-error))
(print (if (eq (fl-interp '(and (or (< 3 (* 2 2))) 1) nil) 'T) 'P20-OK 'P20-error))
(print (if (not (fl-interp '(or (= 5 (- 4 2)) (and (not (> 2 2)) (< 3 2))) nil)) 'P21-OK 'P21-error))
(print (if (equal (fl-interp '(if (not (null (first (a c e)))) (if (number (first (a c e))) (first (a c e)) (cons (a c e) d)) (rest (a c e))) nil) (cons '(a c e) 'd)) 'P22-OK 'P22-error))


;USER-DEFINED (U1-U5)
(print (if (eq (fl-interp '(greater 3 5) '((greater (x y) = (if (> x y) x (if (< x y) y nil))))) '5) 'U1-OK 'U1-error))
(print (if (eq (fl-interp '(square 4) '((square (x) = (* x x)))) '16) 'U2-OK 'U2-error))
(print (if (eq (fl-interp '(simpleinterest 4 2 5) '((simpleinterest (x y z) = (* x (* y z))))) '40) 'U3-OK 'U3-error))
(print (if (eq (fl-interp '(xor true nil) '((xor (x y) = (if (equal x y) nil true)))) 'true) 'U4-OK 'U4-error))
(print (if (eq (fl-interp '(cadr (5 1 2 7)) '((cadr (x) = (first (rest x))))) '1) 'U5-OK 'U5-error))
;; more complex (U6-U11)
(print (if (eq (fl-interp '(last (s u p)) '((last (x) = (if (null (rest x)) (first x) (last (rest x)))))) 'p) 'U6-OK 'U6-error))
(print (if (equal (fl-interp '(push (1 2 3) 4) '((push (x y) = (if (null x) (cons y nil) (cons (first x) (push (rest x) y)))))) '(1 2 3 4)) 'U7-OK 'U7-error))
(print (if (equal (fl-interp '(pop (1 2 3)) '((pop (x) = (if (atom (rest (rest x))) (cons (first x) nil) (cons (first x)(pop (rest x))))))) '(1 2)) 'U8-OK 'U8-error))
(print (if (eq (fl-interp '(power 4 2) '((power (x y) = (if (= y 1) x (power (* x x) (- y 1)))))) '16) 'U9-OK 'U9-error))
(print (if (eq (fl-interp '(factorial 4) '((factorial (x) = (if (= x 1) 1 (* x (factorial (- x 1))))))) '24) 'U10-OK 'U10-error))
(print (if (eq (fl-interp '(divide 24 4) '((divide (x y) = (div x y 0)) (div (x y z) = (if (> (* y z) x) (- z 1) (div x y (+ z 1)))))) '6) 'U11-OK 'U11-error))

