#include "vm.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	vm_init();

	int program[] = {
		OP_MOV, EAX, 10,
		OP_JMP, 8,
		OP_ADD, EAX, 10,
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
