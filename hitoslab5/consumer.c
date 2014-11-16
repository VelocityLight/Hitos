#define __LIBRARY__
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>
#include <sys/stat.h>

#define NR_BUFFERS 10  
#define NR_Produces 500  
#define NR_Consumes 10

#define KEYPAGE 10
#define PAGESIZE 4096
#define ENOMEM      12  
#define EINVAL      22

_syscall2(int,sem_open,const char *,name, unsigned int,value);  
_syscall1(int ,sem_wait , sem_t *,sem);  
_syscall1(int ,sem_post , sem_t *,sem);
_syscall1(int ,sem_unlink,const char *,name);
_syscall2(int ,shmget ,int ,key ,int ,size);
_syscall1(void * ,shmat ,int ,shmid);

int main(){
	int *logic_addr,phyc_addr,i;
	sem_t *empty,*full,*mutex;
	mutex=(sem_t *)sem_open("mmmmm", 1);
        empty=(sem_t *)sem_open("eeeee", NR_BUFFERS);
        full=(sem_t *)sem_open("fffff", 0);
	phyc_addr=shmget(KEYPAGE,PAGESIZE);
	/*printf("phycSuc=%d\n",phyc_mem_addr);*/
	logic_addr=(int *)shmat(phyc_addr);
	/*printf("logicSuc=%d\n",logic_addr);*/
	for(i=0; i<NR_Produces; i++)  
	    	{  
			/*sleep(1); */ 
		   	sem_wait(full);  
			sem_wait(mutex);
			printf("%d\n",logic_addr[i%NR_BUFFERS]);
			fflush(stdout);
			sem_post(mutex);  
			sem_post(empty);  
	    	}
	sem_unlink("eeeee");  
   	sem_unlink("fffff");  
        sem_unlink("mmmmm");
	return 0;
}
