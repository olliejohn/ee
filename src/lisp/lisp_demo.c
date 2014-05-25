#include "lisp.h"

void lisp_output_deleg(wchar_t *fmt, ...)
{
	wprintf(fmt);
}

int main(int argc, char **argv)
{
	lisp_init();
	lisp_set_out_function(lisp_output_deleg);

	//lisp_execute(L"(+ (* 3 4) (max 3 76))");
	//lisp_execute(L"(+ 3 4)");
	//lisp_execute(L"(+ 3 (* 5 3) 3 (max 3 76) (y-or-n-p \"Hello World\"))");
	lisp_execute(L"(+ (max 3 76) (* (- 54 3) 5) 4)");

	lisp_destroy();

	return 0;
}
