#define __LIBRARY__  
#include <unistd.h>  
#include <stdio.h>  
  
_syscall2(sem_t *,sem_open,const char *,name, unsigned int,value);  
_syscall1(int ,sem_wait , sem_t *,sem);  
_syscall1(int ,sem_post , sem_t *,sem);
_syscall1(int ,sem_unlink,const char *,name);

#define NR_BUFFERS 10  
#define NR_Produces 500  
#define NR_Consumes 50


sem_t *empty,*full,*mutex;
FILE *fp = NULL;

void insertNUM(FILE* fps, int ins, int nums){
	fseek(fps, ins*sizeof(int), SEEK_SET);
	fwrite(&nums, 1, sizeof(nums), fps);
	fflush(fps);
}

void removeNUM(FILE* fps, int outs,int* nums){  
    fseek( fps, outs*sizeof(int) , SEEK_SET );  
    fread( nums, sizeof(int),1, fps);    
}

int main()  
{  
    int i,j,in=0,out=0;
    int item;

    mutex=sem_open("mmmmm", 1);  
    empty=sem_open("eeeee", NR_BUFFERS);
    full=sem_open("fffff", 0);
    fp=fopen("out.txt", "wb+");
    if(!fork())
    {  
   	/*Producer(NR_Produces,fp,&in);*/
	for(i=0; i<NR_Produces; i++)  
    	{  
		/*sleep(1); */ 
	   	sem_wait(empty);  
		sem_wait(mutex);
		insertNUM(fp,in,i);
		in = (in+1)%NR_BUFFERS;  
		sem_post(mutex);  
		sem_post(full);  
    	}  
	/*printf("%d\n",getpid());  */
   	exit(5);      
    }
    /*把out指针存到文件缓冲区第十个位置处，非常重要！因为多个消费者，bug了好久啊啊啊*/
    insertNUM(fp, 10, out);
    for(i=0;i<10;i++)  
    {  
    	if(!fork()){ 
        	for(j=0; j<NR_Consumes; j++)  
		{  
			/*sleep(1);*/
		    	sem_wait(full);  
			sem_wait(mutex);
			/*得先取出out指针*/		
			removeNUM(fp, 10, &out);
			fflush(stdout);
			removeNUM(fp, out, &item);
		  	printf("%d : %d\n",getpid(),item); 
			fflush(stdout); 
			out = (out+1)%NR_BUFFERS;
			/*把out指针存到文件缓冲区第十个位置处*/
			insertNUM(fp, 10,out);
			sem_post(mutex);  
			sem_post(empty);  
		}  
		exit(5);
	}
    }	
    sem_unlink("eeeee");  
    sem_unlink("fffff");  
    sem_unlink("mmmmm");
    fclose(fp);
    return 0;  
}
