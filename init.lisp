(define (length x) 
	(if (nil? x)
		0
		(+ 1 (length (cdr x)))))

(define (list . x) x)

(define (list* . lst)
	(if (nil? (cdr lst))
		(car lst)
        (cons (car lst) (list*-helper (cdr lst)))))

(define (list*-helper lst)
	(if (nil? (cdr lst))
		(car lst)
		(cons (car lst) (list*-helper (cdr lst)))))
		
		
(define (last x)
	(if (nil? x)
		()
		(if (cons? (cdr x))
			(last (cdr x))
			x)))

(defun max (x y) 
	(if (> x y)
		x
		y))

(defun min (x y)
	(if (> x y)
		y
		x))

(defun caar (lst)
	(car (car lst)))

(defun cadr (lst)
	(car (cdr lst)))

(defun cddr (lst)
	(cdr (cdr lst)))

(defun cdar (lst)
	(cdr (car lst)))

(defun avg (lst)
	(/ (apply + lst) (length lst)))

(defun anti-sum (lst)
	(apply - 0 lst))

(defun merge-list (x y)
	(if (nil? x)
		y
		(cons (car x) (merge-list (cdr x) y))))
		
(defun string-copy (x) (substring x 0))
