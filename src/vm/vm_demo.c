#include "vm.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	vm_init();

	int program[] = {
		OP_MOV, EAX, 134,
		OP_PUSH, EAX,
		OP_POP, EAX,
		OP_INC, EAX,
		OP_INC, EAX,
		OP_DEC, EAX,
		OP_END
	};
/*
	int program[] = {
		OP_MOV, EAX, 134, OP_END
	};
*/
	if (vm_execute(program) == 0)
		printf("Success\n");
	else
		printf("Error at %d\n", vm_errpos);

	vm_destroy();

	return 0;
}
