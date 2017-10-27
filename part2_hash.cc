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
	char *filename;
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

//Task 3
int numOpenFiles =0;
struct file *senderFile = (struct file *)malloc(sizeof(struct file));
struct file *receiverFile = (struct file *)malloc(sizeof(struct file));

struct file *files = (struct file *)malloc(sizeof(struct file)*8);
int num_open_files =0;

//[0] -> bob
//[1] -> alice
//[2] -> rick
//[3] -> morty
//[4] -> picard
//[5] -> kirk
//[6] -> student
//[7] -> hacker




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

	//Task 3 and 4
	void setTask(int t){
		task = t;
	}

	int getTask(){
		return task;
	}

	//Task 4
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

	//Task 2
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
		

/**
	//Task 3
	void setNumOpenFiles(int num){
		numOpenFiles = num;
	}
	int getNumOpenFiles(){
		return numOpenFiles;
	}
	void openSenderFile(char *pathname, int fd){
		senderFile->fd = fd;
		senderFile->filename = (char *)malloc(sizeof(char)*20);
		strcpy(senderFile->filename, pathname);	
		senderFile->balance = -1; //Balance starts at -1 until read in
	}
	char *getSenderFileName(){
		return senderFile->filename;;
	}
	int getSenderFD(){
		return senderFile->fd;
	}
	void setSenderBalance(int b){
		senderFile->balance = b;
	}
	int getSenderBalance(){
		return senderFile->balance;
	}

	void closeSenderFile(){
		free(senderFile->filename);
		free(senderFile);
	}

	void openReceiverFile(char *pathname, int fd){
		receiverFile->fd = fd;
		receiverFile->filename =(char *)malloc(sizeof(char)*20);
		strcpy(receiverFile->filename,pathname);		
		receiverFile->balance =-1; //Balance -1 until set
	}
	char *getReceiverFileName(){
		return receiverFile->filename;;
	}
	int getReceiverFD(){ //File descriptor
		return receiverFile->fd;
	}
	void setReceiverBalance(int b){
		receiverFile->balance = b;
	}
	int getReceiverBalance(){
		return receiverFile->balance;
	}
	void closeReceiverFile(){
		free(receiverFile->filename);
		free(receiverFile);
	}
*/
	//Task 3
	void openFile(char *pathname, int fd,int balance){
		num_open_files++;
		if(num_open_files > 8){
			return;
		}
		int index = fd;
		fd = 3+(2*fd); //fd+4;
		files[index].fd = fd;
		files[index].filename = (char *)malloc(sizeof(char)*20);
		strcpy(files[index].filename,pathname);
		files[index].balance= balance;
	}
	int getOpenFileFD(int fd){ //FD will be the index
		int index = fd-4;
		return files[index].fd;
	}
	char *getOpenFileFilename(int fd){
		int index = fd-4;
		return files[index].filename;
	}
	void setOpenFileBalance(int fd, int bal){
		int index = fd-4;
		files[index].balance = bal;
	}
	int getOpenFileBalance(int fd){
		int index = fd-4;
		return files[index].balance;
	}
	void closeFile(int fd){
		int index = fd-4;
		num_open_files--;
		free(files[index].filename);
		if(num_open_files == 0){
			free(files);
		}
	}
}

