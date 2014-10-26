#include <stdio.h>   
#include <unistd.h>   
#include <time.h>   
#include <sys/times.h>   
#include <stdlib.h>   
   
#define HZ  100   
   
void cpuio_bound(int last, int cpu_time, int io_time);   
   
int main(int argc, char * argv[])   
{   
    int i;
    /*Output*/
    printf("parent_pid=%d\n",getpid());
    for(i=0;i<5;i++)  
    { 	

	/*fork() has two rets,we use child forks*/
        if(!fork())  
        {  
            printf("child_process%d=%d\n",i+1,getpid());
            cpuio_bound(10,4-i,i);
	    /*cut*/
	    exit(5); 
        }  
    }     
    return 0;   
}   

void cpuio_bound(int last, int cpu_time, int io_time)
{
	struct tms start_time, current_time;
	clock_t utime, stime;
	int sleep_time;

	while (last > 0)
	{
		times(&start_time);
		do
		{
			times(&current_time);
			utime = current_time.tms_utime - start_time.tms_utime;
			stime = current_time.tms_stime - start_time.tms_stime;
		} while ( ( (utime + stime) / HZ )  < cpu_time );
		last -= cpu_time;

		if (last <= 0 )
			break;

		sleep_time=0;
		while (sleep_time < io_time)
		{
			sleep(1);
			sleep_time++;
		}
		last -= sleep_time;
	}
}
