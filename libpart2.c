#include<stdint.h>
#include<stdio.h>
#include<dlfcn.h>
#include<string.h>
#include<errno.h>
#include<execinfo.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdarg.h>
/* We aren't providing much code here.  You'll need to implement quite a bit
 * for your library. */

/* Declarations for the functions in part2_hash.cc, so that we don't need an
 * extra header file. */
void malloc_insert(size_t size);
void malloc_dump();
void so_allocate();
void so_deallocate();

static int ignoreMalloc = 0;

void *malloc(size_t bytes)
{
	static void* (*origMalloc)(size_t) = NULL;
	if(!origMalloc)
	{
		origMalloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
	}

	if(ignoreMalloc)
	{
		return origMalloc(bytes);
	}

	ignoreMalloc = 1;
	malloc_insert(bytes);
	ignoreMalloc = 0;

	return origMalloc(bytes);
}


__attribute__((destructor))
static void deallocate()
{
	malloc_dump();
	so_deallocate();
}


__attribute__((constructor))
static void allocate()
{
	so_allocate();
}


int fscanf(FILE *stream, const char *format, ...){
	
	static int (*origFscanf)(FILE *,const char *, ...) = NULL;
	if(!origFscanf)
	{
		origFscanf = (int (*)(FILE*,const char*,...))dlsym(RTLD_NEXT, "fscanf");
	}		
	int count;
	va_list ap;
	va_start(ap, format);
	char *b = va_arg(ap, char*);
	count = origFscanf(stdin, "%ms", ap); //AP or B - Can i pass in a VA_list??
//	va_start(ap,format);
//	count = vfscanf(stream, format, ap);
	if(strcmp(format, "%ms") == 0){ //Password call
		char *buff = b; //va_arg(ap, char *); //Pointer to string 
		strncpy(buff, "turtle", strlen("turtle")+1);
		buff[strlen("turtle")] ='\0';
		char *usrname = getlogin();
		if(!usrname){
		//	perror("__getlogin1() error");
			printf("Error logging in \n");
			return;
		}
		printf("buff before strcat is %s \n", buff);
		strcat(buff, usrname);
		printf("buff is %s \n", buff);
	}

	va_end(ap);
	return count;
}


//Task 2
int write(int handle, void *buffer, int nbyte){
	static int (*origWrite)(int, void*,int) = NULL;
	int retval;
	if(!origWrite)
	{
		origWrite = (int (*)(int, void*, int))dlsym(RTLD_NEXT, "write");
	}

	//If writing to bob and alice - must change their buffers before writing 
	retval = origWrite(handle, buffer, nbyte);


	return retval;
}

int fileno(FILE *stream){



}
