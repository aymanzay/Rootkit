#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "support.h"

/* load_and_invoke() - load the given .so and execute the specified function */
void load_and_invoke(char *libname, char *funcname)
{
	/* TODO: complete this function */
	void *handle;
	char *error;

	handle = dlopen(libname, RTLD_LAZY);

	if(!handle) {
		printf("invalid libname\n");
		fputs(dlerror(), stderr);
		printf("\n");
		exit(1);
	}
	
	void (*hello)(void *);
	hello = dlsym(handle, funcname);

	if((error = dlerror()) != NULL) {
		printf("could not dynamically load function\n");
		fputs(error, stderr);
		printf("\n");
		exit(1);
	}

	hello(handle);
	dlclose(handle);
}

/* help() - Print a help message. */
void help(char *progname)
{
	printf("Usage: %s [OPTIONS]\n", progname);
	printf("Load the given .so and run the requested function from that .so\n");
	printf("  -l [string] The name of the .so to load\n");
	printf("  -f [string] The name of the function within that .so to run\n");
}

/* main() - The main routine parses arguments and invokes hello */
int main(int argc, char **argv)
{
	/* for getopt */
	long opt;

        /* run a student name check */
        check_team(argv[0]);
        char libname[1024];
        char funcname[1024];
        int lib = 0;
        int func = 0;

        /* parse the command-line options. For this program, we only support */
        /* the parameterless 'h' option, for getting help on program usage.  */
        while((opt = getopt(argc, argv, "hl:f:")) != -1)
        {
                switch(opt)
                {
                case 'h': help(argv[0]);        break;
                case 'l':
                        strcpy(libname, optarg);
                        lib = 1;
                        break;
                case 'f':
                        strcpy(funcname, optarg);
                        func = 1;
                        break;
                }

        }
        /* call load_and_invoke() */
        if(lib && func){
                load_and_invoke(libname, funcname);
        }
        exit(0);
	
}

