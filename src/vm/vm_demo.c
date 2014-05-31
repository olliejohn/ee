#include "vm.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	vm_init();

	int program[] = {
		OP_MOV, EAX, 10,
		OP_MOV, EBX, 10,
		OP_CMPR, EAX, EBX,
		OP_JE, 13,
		OP_ECHO, EFLAGS,
		OP_END
	};

	if (vm_execute(program) == 0)
		printf("Success\n");
	else
		printf("Error at %d\n", vm_errpos);

	vm_destroy();

	return 0;
}
