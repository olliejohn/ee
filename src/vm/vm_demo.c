#include "vm.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	vm_init();

	int program[] = {
		OP_PUSH, 134, OP_POP, EAX, OP_END
	};

	int status = vm_execute(program);

	printf("Program exited with status %d\n", status);

	vm_destroy();

	return 0;
}
