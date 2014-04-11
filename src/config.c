#include "config.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_CFG_NAME ".ee"
#define DEFAULT_CFG "usetabs 1\ntabsize 8\nlinnums 1"

void parse_config(char *cfg)
{

}

int load_config(char *path, char **cfg)
{
	FILE *f = fopen(file, "r");

	if (f == NULL)
		return -1;

	char ch;
	while ((ch = fgetc(f)) != EOF) {

	}

	fclose(f);

	return 0;
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

void load_init_config()
{
	struct passwd *pws = getpwuid(geteuid());

	int pathlen = 8 + strlen(DEFAULT_CFG_NAME) + strlen(pws->pw_name);
	char *path = calloc(pathlen, sizeof(char));

	strcpy(path, "/home/");
	strcat(path, pws->pw_name);
	strcat(path, "/");
	strcat(path, DEFAULT_CFG_NAME);

	char *cfg = malloc(sizeof(char) * 64);
	load_config(path, &cfg);

	free(cfg);
}

