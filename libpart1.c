#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "support.h"

/* hello() - print some output so we know the shared object loaded.
 *
 * The function signature takes an untyped parameter, and returns an untyped
 * parameter. In this way, the signature *could* support any behavior, by
 * passing in a struct and returning a struct. */
void *hello(void *param)
{
	printf("hello from a .so\n");
	return NULL;
}

void toUpper(char *c) {
	while(*c != '\0') {
		*c = toupper(*c);
		c++;
	}
}

void *ucase(void *param) {

	struct team_t *in = param;
	struct team_t *uc = malloc(sizeof(struct team_t));

	uc->name1 = malloc(strlen((*in).name1)+1);
	uc->email1 = malloc(strlen((*in).email1)+1);
	uc->name2 = malloc(strlen((*in).name2)+1);
	uc->email2 = malloc(strlen((*in).email2)+1);
	uc->name3 = malloc(strlen((*in).name3)+1);
	uc->email3 = malloc(strlen((*in).email3)+1);

	strcpy(uc -> name1, (*in).name1);
	strcpy(uc -> email1, (*in).email1);
	strcpy(uc -> name2, (*in).name2);
	strcpy(uc -> email2, (*in).email2);
	strcpy(uc -> name3, (*in).name3);
	strcpy(uc -> email3, (*in).email3);

	toUpper(uc -> name1);
	toUpper(uc -> email1);
	toUpper(uc -> name2);
	toUpper(uc -> email2);
	toUpper(uc -> name3);
	toUpper(uc -> email3);

	return uc;
}
