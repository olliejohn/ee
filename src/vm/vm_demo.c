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
		OP_MOV, EBX, 4,
		OP_DEC, EAX,
		OP_ECHO, EAX,
		OP_MULR, EAX, EBX,
		OP_ECHO, EAX,
		OP_DIV, EBX, 2,
		OP_DIVR, EAX, EBX,
		OP_ECHO, EAX,
		OP_END
	};

	if (vm_execute(program) == 0)
		printf("Success\n");
	else
		printf("Error at %d\n", vm_errpos);

	vm_destroy();

	return 0;
}
