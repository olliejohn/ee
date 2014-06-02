/*
 * yawrun.c
 * Part of the VM subsystem in the Yaw text editor
 *
 * Copyright 2014 Ollie Etherington.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifdef FREESTANDING_RUNTIME

#include "vm.h"

#include <getopt.h>
#include <stdio.h>

#define NAME		"yawrun"
#define VERSION 	"0.0.1"
#define AUTHOR		"Ollie Etherington"

#define FILENAME_MAX_SIZE 128

int run_version()
{
	printf("%s - Version %s\n", NAME, VERSION);
	return 0;
}

int run_about()
{
	run_version();
	printf("Frontend for the Yaw Virtual Machine by %s\n", AUTHOR);
	printf("Available as free software under the GNU GPLv2\n");
	return 0;
}

int run_help()
{
	printf(
"\nUsage: yawrun [ FILE | FLAG ]\n\n\
Flags:\n\
-a  --about        Information about yawrun\n\
-h  --help         Help dialog for command line options\n\
-v  --version      Version information\n\
-s  --stack        Print the stack to stdout after execution\n\
-r  --regs         Print the end value of all registers after execution\n\
-f  --flags        Print the end value of the flags after execution\n\
-p  --print        Print stack, registers and flags (equivalent to -srf)\n\
\n");
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Yawrun: Too few arguments\nUse yawrun -h for help\n");
		return 0;
	}

	const struct option opts[] = {
		{ "about",	no_argument,		NULL,	'a'	},
		{ "help",	no_argument,		NULL,	'h'	},
		{ "version",	no_argument,		NULL,	'v'	},
		{ "stack",	no_argument,		NULL,	's'	},
		{ "regs",	no_argument,		NULL,	'r'	},
		{ "flags",	no_argument,		NULL,	'f'	},
		{ "print",	no_argument,		NULL,	'p'	}
	};

	char file[FILENAME_MAX_SIZE];
	unsigned int file_set = 0;

	int arg, flags = 0;
	while ((arg = getopt_long(argc, argv, "-:ahvsrfp", opts, NULL)) != -1) {
		switch (arg) {
		case 'a':
			return run_about();
		case 'h':
			return run_help();
		case 'v':
			return run_version();
		case 's':
			flags |= 1 << VM_PRINT_STACK;
			break;
		case 'r':
			flags |= 1 << VM_PRINT_REGS;
			break;
		case 'f':
			flags |= 1 << VM_PRINT_FLAGS;
			break;
		case 'p':
			flags |= (1 << VM_PRINT_STACK) +
				 (1 << VM_PRINT_REGS) +
				 (1 << VM_PRINT_FLAGS);
			break;
		case ':': /* Missing argument */
			printf("Yawrun: Missing argument to '%s'\n",
				argv[optind - 1]);
			return 0;
		case 1:	  /* Not a "-x" or "--xyz" argument */
		case '?':
			if (file_set) {
				printf("Yawrun: Too many input files\n");
				return 0;
			}

			snprintf(file, FILENAME_MAX_SIZE, "%s", argv[optind-1]);
			file_set++;
			break;
			/* No break here so we fall through to default */
		default:
			printf("Yawrun: Invalid Arguments");
			run_help();
			return 0;
		}
	}

	vm_init();

	if (vm_validate_header(file) != 0) {
		printf("Error: File is not a Yaw executable\n");
		return 0;
	}

	if (vm_execute_file(file, flags) != 0)
		printf("Error at %d\n", vm_errpos);

	vm_destroy();

	return 0;
}

#endif /* FREESTANDING_RUNTIME */
