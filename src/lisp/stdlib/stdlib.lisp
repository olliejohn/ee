; predicates.lisp
(defun zerop (x)
	(if (= x 0) t nil))

(defun plusp (x)
	(if (> x 0) t nil))

(defun minusp (x)
	(if (< x 0) t nil))

(defun evenp (x)
	(if (= 0 (mod x 2)) t nil))

(defun oddp (x)
	(if (= 1 (mod x 2)) t nil))

; math.lisp
(defun max (x y)
	(if (> x y) x y))

(defun min (x y)
	(if (< x y) x y))

(defun abs (x)
	(if (minusp x)
		(- x) x))

(defun signum (x)
	(if (zerop x) x (/ x (abs x))))

; logic.lisp
(defun null (x)
	(not x))



; carcdr.lisp
(defun caar (x)
	(car (car x)))






