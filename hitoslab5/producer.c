#define __LIBRARY__
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>
#include <sys/stat.h>

_syscall2(int,sem_open,const char *,name, unsigned int,value);  
_syscall1(int ,sem_wait , sem_t *,sem);  
_syscall1(int ,sem_post , sem_t *,sem);
_syscall1(int ,sem_unlink,const char *,name);
_syscall2(int ,shmget ,int ,key ,int ,size);
_syscall1(void * ,shmat ,int ,shmid);
  
#define NR_BUFFERS 10  
#define NR_Produces 500  
#define NR_Consumes 10

#define KEYPAGE 10
#define PAGESIZE 4096
#define ENOMEM      12  
#define EINVAL      22

sem_t *empty,*full,*mutex;

void semsopen(){
	mutex=(sem_t *)sem_open("mmmmm", 1);
        empty=(sem_t *)sem_open("eeeee", NR_BUFFERS);
        full=(sem_t *)sem_open("fffff", 0);
}

void semsclose(){
	fflush(stdout);
	sem_unlink("eeeee");  
   	sem_unlink("fffff");  
        sem_unlink("mmmmm");
}

int main(){
	int i,*log_addr,phyc_addr;
	semsopen();	
	phyc_addr=shmget(KEYPAGE,PAGESIZE);
	/*printf("addr= %d\n",phyc_mem_addr);*/
	if(phyc_addr==(-EINVAL)){
		printf("Too Large.\n");
		semsclose();
		return 0;
	}else if(phyc_addr==(-ENOMEM)){
		printf("No free page.\n");
		semsclose();
		return 0;
	}else{
		log_addr = (int *)shmat(phyc_addr);		
	}
	for(i=0; i<NR_Produces; i++)  
	    	{  
			/*sleep(1); */ 
		   	sem_wait(empty);  
			sem_wait(mutex);
			log_addr[i%NR_BUFFERS]=i;
			/*printf("logicnum=%d addr=%d\n",i+1,log_addr+i);*/
			sem_post(mutex);  
			sem_post(full);  
	    	}
	semsclose();
	return 0;
}		
