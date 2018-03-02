;Bowei Wang 1462495
;cmput325 assignment 2



(defun check_func_name (x y)
  (cond 
  	((eq x (caar y)) (car y))
	(t (check_func_name x (cdr y)))
  )
)
(defun replace (x y Expression P)
  (cond
  	((null Expression) nil)
	((eq x (car Expression)) (cons (fl-interp (y P)) (replace x y (cdr Expression) P)))
	(t (cons (fl-interp ((car Expression) P)) (replace x y (cdr Expression) P)))
  )
)
(defun putin_value (L Expression y P)
  (cond 
  	((null L) Expression)
	(t (putin_value (cdr L) (replace (car L) (car y) Expression) (cdr y) P))
  	)
)
(defun handle_user_def (x y P)
  (let (cur_function (check_func_name x P)))
  (putin_value (cadr cur_function) (cadddr cur_function) y P)

  )

(defun fl-interp (E P)
  (cond 
	((atom E) E)   %this includes the case where E is nil or a number
        (t
           (let ( (f (car E))  (arg (cdr E)) )
	      (cond 
                ; handle built-in functions
                ((eq f 'first)  (car (fl-interp (car arg) P)))
		((eq f 'rest)  (cdr (fl-interp  (car arg) P)))
		((eq f 'atom) (atom (fl-interp (car arg) P))
		((eq f 'null) (null (fl-interp (car arg) P)))
		((eq f 'eq) (eq (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'equal) (equal (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'numberp) (numberp(fl-interp (car arg) P))
		((eq f 'append) (append (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'car)  (car (fl-interp (car arg) P)))
		((eq f 'cdr) (cdr (fl-interp  (car arg) P)))
		((eq f 'cons) (cons (fl-interp (car arg) P) (fl-interp (cadr arg) P)))
		((eq f 'if) (if (fl-interp (car arg) P) (fl-interp (cadr arg) P) (fl-interp (caddr arg) P)))
		((eq f 'cond) (if (fl-interp (caar arg) P) (fl-interp (cadar arg) P) (fl-interp (cons f (cdr arg)) P)))
		((eq f 'cond)
		((eq f 'cond)
		((eq f 'cond)
		((eq f 'cond)
		((eq f 'cond)
		((eq P nil) (cons f arg))
		
	        ; if f is a user-defined function,
		(t (handle_user_def f arg P))

                ;    then evaluate the arguments 
                ;         and apply f to the evaluated arguments 
                ;             (applicative order reduction) 
                .....

                ; otherwise f is undefined (not intended to be a function),
                ; the E is returned, as if it is quoted in lisp 

