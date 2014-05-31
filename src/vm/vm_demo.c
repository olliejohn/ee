#include "vm.h"

int main(int argc, char **argv)
{
	vm_init();

	unsigned int program[] = {
		OP_PUSH, 34
	};

	vm_execute(program);

	vm_destroy();
	return 0;
}
