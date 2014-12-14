#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define mem_size 0x1000000 
char* mem;
int thread_num,test_num;
char testdata[5][20]={"0","0xff","0x55","0xAA","0x1c"};
struct test{
	pthread_t th;
	int status;
	int going;
}thrs[50];

void*  memtest(void *p){
	int size = mem_size/thread_num;
	int i,j,k;
	char *in;
	thrs[(int)p].status=1;
	for(i=0; i<size; i++){
		for(j=0; j<5; j++){
			/*
			printf("in is: %s\n",in);
			printf("this going: %d.\n",thrs[(int)p].going);
			*/
						
			for(k=0; k<test_num; k++){
				strcpy(mem+((int)p)*size,testdata[j]);
                       		in = mem+((int)p)*size;
				/*printf("in=%s\n",(*in));*/
				if((*in)!=*(mem+((int)p)*size)){
					printf("memory error\n");
					thrs[(int)p].status=-1;
					break;
				}
			}
		}
		thrs[(int)p].going = in-mem;
		if(thrs[(int)p].status==-1) break;
	}
	thrs[(int)p].status=0;
	pthread_exit((void*)0);
}

void go(){
	int i;
	for(i=0; i<thread_num; i++){
		if(!pthread_create(&thrs[i].th,NULL,memtest,(void *)i))
			printf("thread %d: created.\n",(int)i);
		else
			printf("thread %d: created error.\n",i);
	}
}

void status(){
	int i;
	for(i=0; i<thread_num; i++){
		if(thrs[i].status==1) 
			printf("thread %d: running, in %d.\n",i,thrs[i].going);
		else if(thrs[i].status==-1)
			printf("thread %d: something error, in %d.\n",i,thrs[i].going);
		else if(thrs[i].status==0)
			printf("thread %d: exit. in %d.\n",i,thrs[i].going);
	}
}

void abort(){
	int i;
	for(i=0; i<thread_num; i++){
		if(thrs[i].status==0)
			printf("thread %d: already exit.\n",i);
		else{
			pthread_join(thrs[i].th,NULL);
			printf("thread %d: joins.\n");
		}
	}
}

int main(){	
	char list[20];
	mem = (char*)malloc(mem_size);
	while(scanf("%s",list)!=EOF){
		if(!strcmp("exit",list)) break;
		if(!strcmp("times",list)) scanf("%d",&test_num);
		if(!strcmp("thread",list)) scanf("%d",&thread_num);
		if(!strcmp("go",list)) go();
		if(!strcmp("status",list)) status();
		if(!strcmp("abort",list)) abort();
	}
	return 0;
}
