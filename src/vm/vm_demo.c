#include "vm.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	vm_init();

	int program[] = {
		OP_JMP, 27,

		OP_PUSH, EBP,
		OP_MOVR, EBP, ESP,
		OP_ADD, ESP, 2,
		OP_POP, ECX,
		OP_ADD, ECX, 5,
		OP_ECHO, ECX,
		OP_SUB, ESP, 3,
		OP_MOVR, ESP, EBP,
		OP_POP, EBP,
		OP_RETP, 1,

		OP_MOV, EAX, 10,
		OP_PUSH, EAX,
		OP_ECHO, EAX,
		OP_CALL, 2,
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
