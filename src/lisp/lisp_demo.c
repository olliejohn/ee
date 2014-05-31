#include "lisp.h"

#include <stdarg.h>

void lisp_output_deleg(wchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vwprintf(fmt, args);
	va_end(args);
}

int main(int argc, char **argv)
{
	lisp_init();
	lisp_set_out_function(lisp_output_deleg);

	//lisp_execute(L"(+ (* 3 4) (max 3 76))");
	//lisp_execute(L"(+ 3 (* 5 3) (max 3 76) (y-or-n-p \"Hello World\"))");
	//lisp_execute(L"(+ (+ 3 76) (* (- 54 3) 5) 4)");
	//lisp_execute(L"(load stdlib/stdlib.lisp)");
	//lisp_execute(L"(+ 3 (- 10 4)) (= 3 3 4)");
	lisp_execute(L"(cons 300 (45 32))");
	lisp_execute(L"(cons 45 32)");

	lisp_destroy();

	return 0;
}
