#include "lisp.h"

void lisp_output_deleg(wchar_t *fmt, ...)
{
	wprintf(fmt);
}

int main(int argc, char **argv)
{
	lisp_set_out_function(lisp_output_deleg);
	lisp_execute(L"(+ 3 4)");

	return 0;
}
