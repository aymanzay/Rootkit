#include<unordered_map>
#include<cassert>
#include<cstdio>
#include<cstring>
#include<map>
#include<iostream>
#include<string>
#include<vector>
#include<unistd.h>
#include<iomanip>
#include<fcntl.h>
#include<sys/wait.h>

using namespace std;


struct file{
	int fd;
	char * filename;
	int balance;
};

vector<size_t> *malloc_data = NULL;
int bobOpen = 0;
int aliceOpen = 0;

//struct file files[8]; 
struct file *bobFile = (struct file *)malloc(sizeof(struct file));
struct file *aliceFile = (struct file *)malloc(sizeof(struct file));

int task = -1;

int prev_sender= 0;
int prev_receiver =0;
int transfer_value = 0;

/* declare the following functions to have "C" linkage, so that
 * we can see them from C code without doing name demangling. */
extern "C"
{
	void so_deallocate()
	{
		delete malloc_data;
	}

	void so_allocate()
	{
		if(malloc_data == NULL)
		{
			malloc_data = new vector<size_t>();
		}
	}

    /* dump() - output the contents of the malloc_data */
    void malloc_dump()
	{
		for(auto& data : *malloc_data)
		{
			printf("Allocations made: %ld.\n", data);
		}
	}

	/* insert() - when malloc() is called, the interpositioning library
     *            stores the size of the request.	*/
	int malloc_insert(size_t size)
	{
		so_allocate();
		malloc_data->push_back(size);
	}


	void setTask(int t){
		task = t;
	}

	int getTask(){
		return task;
	}

	void setDesc3(int val){
		prev_sender = val;
	}
	int getDesc3(){
		return prev_sender;
	}
	void setDesc4(int val){
		prev_receiver = val;
	}
	int getDesc4(){
		return prev_receiver;
	}
	void setTransfer(int val){
		transfer_value =val;
	}
	int getTransfer(){
		return transfer_value;
	}
	void openBob(int fd,const char *filename){
		bobOpen =1;
		bobFile->fd = fd;
		bobFile->filename = (char *)malloc(sizeof(char)*15);
		strcpy(bobFile->filename, filename);
		bobFile->balance =-1;
	}

	int getBobFd(){
		if(bobOpen){
			return bobFile->fd;
		}
		else{
			return -1;
		}
	}

	void closeBob(int fd){
		if(bobOpen && (bobFile->fd == fd)){
			bobOpen = 0;
			free(bobFile->filename);
			free(bobFile);
		}
	}

	void openAlice(int fd,const char *filename){
		aliceOpen =1;
		aliceFile->filename = (char *)malloc(sizeof(char)*15);
		aliceFile->fd = fd;
		strcpy(aliceFile->filename, filename);
		aliceFile->balance = -1;
	}


	void closeAlice(int fd){
		if(aliceOpen && (aliceFile->fd == fd)){
			aliceOpen = 0;
			free(aliceFile->filename);
			free(aliceFile);
		}
	}

	
	int getAliceFd(){
		if(aliceOpen){
			return aliceFile->fd;
		}
		else{
			return -1;
		}
	}

	int getAliceBalance(){
		return aliceFile->balance;
	}
	int getBobBalance(){
		return bobFile->balance;
	}

	void setAliceBalance(int balance){
		aliceFile->balance = balance;
	}
	void setBobBalance(int balance){
		bobFile->balance = balance;
	}
		

}

