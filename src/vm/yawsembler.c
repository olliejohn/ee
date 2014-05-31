/*
 * yawsembler.c
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

#include "yawsembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME		"yawsembler"
#define VERSION 	"0.0.1"
#define AUTHOR		"Ollie Etherington"

unsigned int *assemble(char *data)
{
	unsigned int *code = malloc(16 * sizeof(int));
	return code;
}

#ifdef FREESTANDING_ASSEMBLER

void version()
{
	printf("%s - Version %s\n", NAME, VERSION);
}

void about()
{
	version();
	printf("By %s\n", AUTHOR);
	printf("Available as free software under the GNU GPLv2\n");
}

void help()
{
	printf("\nUsage: yawsembler [ FILE | FLAG ]\n\n\
Flags:\n\
--about		Information about yawsembler\n\
--help		Help about how to use yawsembler\n\
--version	Version information about yawsembler\n\
	\n");
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Yawsembler: Invalid Arguments\n");
		help();
		return 0;
	}

	if (strcmp(argv[1], "--version") == 0) {
		version();
		return 0;
	}

	if (strcmp(argv[1], "--about") == 0) {
		about();
		return 0;
	}

	if (strcmp(argv[1], "--help") == 0) {
		help();
		return 0;
	}

	char *buffer = NULL;
	long length;
	FILE *f = fopen(argv[1], "rb");

	if (f == NULL) {
		printf("yawsembler: Error opening file %s\n", argv[1]);
		return 0;
	}

	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = malloc(length);

	if (buffer == NULL) {
		printf("yawsembler: Error allocating memory for output");
		return 0;
	}

	fread(buffer, 1, length, f);

	fclose (f);

	if (buffer == NULL) {
		printf("yawsembler: Error assembling");
		return 0;
	}

	unsigned int *code = assemble(buffer);

	free(buffer);

	char *fout_path = strtok(argv[1], ".");

	FILE *fout = fopen(fout_path, "wb");

	if (fout != NULL) {
		printf("yawsembler: Error opening outfile %s", fout_path);
		return 0;
	}

	fwrite(code, sizeof(unsigned int), sizeof(code), fout);

	fclose(fout);

	return 0;
}

#endif /* FREESTANDING_ASSEMBLER */
