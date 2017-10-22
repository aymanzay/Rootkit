#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include"support.h"

/* help() - Print a help message. */
void help(char *progname)
{
	printf("Usage: %s\n", progname);
	printf("(This is just a demonstration of the traditional way\n");
	printf(" to use .so filed)\n");
}

/* declare the hello() and ucase() function that lives in a shared library */
extern void *hello(void* param);

extern void *ucase(void* param);

/* main() - The main routine parses arguments and invokes hello */
int main(int argc, char **argv)
{
	/* for getopt */
	long opt;

	/* run a student name check */
	check_team(argv[0]);

	/* parse the command-line options. For this program, we only support */
	/* the parameterless 'h' option, for getting help on program usage.  */
	while((opt = getopt(argc, argv, "h")) != -1)
	{
		switch(opt)
		{
		case 'h': 	help(argv[0]); 	break;
		}
	}

	hello(NULL);

	/* TODO: execute the new function "ucase" that you added to libpart1.c */
	struct team_t *uCase = ucase(&team);
	
	printf("Student 1 : %s\n", uCase -> name1);
    printf("Email 1   : %s\n", uCase -> email1);
    printf("Student 2 : %s\n", uCase -> name2);
	printf("Email 2   : %s\n", uCase -> email2);
	printf("Student 3 : %s\n", uCase -> name3);
    printf("Email 3   : %s\n", uCase -> email3);
	printf("\n");
	
	free(uCase->name1);
    free(uCase->email1);
    free(uCase->name2);
	free(uCase->email2);
	free(uCase->name3);
	free(uCase->email3);
    free(uCase);

	exit(0);
}
