/*
 * config.c
 * Part of the ee text editor
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

#include "config.h"

#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_CFG_NAME ".ee"
#define DEFAULT_CFG "usetabs 1\ntabsize 8\nlinnums 1\n"
#define CONFIG_BLOCK_SIZE 32

struct Settings *settings_new()
{
	struct Settings *settings = malloc(sizeof(struct Settings *));
	settings->usetabs = -1;
	settings->tabsize = -1;
	settings->linnums = -1;
	return settings;
}

void settings_free(struct Settings *settings)
{
	free(settings);
}

void parse_config(struct Settings *settings, char *cfg)
{
	char *cmd = strtok(cfg, " \n");
	while (cmd != NULL) {
		if (strcmp(cmd, "usetabs") == 0) {
			cmd = strtok(NULL, " \n");

			if (cmd == NULL)
				break;

			if (isdigit(cmd[0]))
				settings->usetabs = atoi(cmd);
		} else if (strcmp(cmd, "tabsize") == 0) {
			cmd = strtok(NULL, " \n");

			if (cmd == NULL)
				break;

			if (isdigit(cmd[0]))
				settings->tabsize = atoi(cmd);
		} else if (strcmp(cmd, "linnums") == 0) {
			cmd = strtok(NULL, " \n");

			if (cmd == NULL)
				break;

			if (isdigit(cmd[0]))
				settings->linnums = atoi(cmd);
		}

		cmd = strtok(NULL, " \n");
	}
}

char *load_config(char *path)
{
	FILE *f = fopen(path, "rb");

	if (f == NULL)
		return NULL;

	fseek(f, 0, SEEK_END);
	long f_len = ftell(f);
	rewind(f);

	char *cfg = malloc((f_len + 1) * (sizeof(char)));

	fread(cfg, sizeof(char), f_len, f);

	fclose(f);

	cfg[f_len] = 0;

	return cfg;
}

int write_default_config(char *path)
{
	FILE *f = fopen(path, "w");

	if (f == NULL)
		return -1;

	fprintf(f, "%s", DEFAULT_CFG);

	fclose(f);

	return 0;
}

void load_init_config(struct Settings *settings)
{
	struct passwd *pws = getpwuid(geteuid());

	int pathlen = 8 + strlen(DEFAULT_CFG_NAME) + strlen(pws->pw_name);
	char *path = calloc(pathlen, sizeof(char));

	strcpy(path, "/home/");
	strcat(path, pws->pw_name);
	strcat(path, "/");
	strcat(path, DEFAULT_CFG_NAME);

	char *cfg;
	while ((cfg = load_config(path)) == NULL)
		write_default_config(path);

	parse_config(settings, cfg);

	free(cfg);
}
