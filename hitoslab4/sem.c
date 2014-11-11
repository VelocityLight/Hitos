#include <unistd.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <errno.h>
#include <asm/system.h>
#include <asm/segment.h>

#define SEM_NUM 10
sem_t sems[SEM_NUM];
char SemName[SEM_LEN];

void sem_init(){
	int i;
	for(i=0; i<SEM_NUM;i++){
		sems[i].free=0;
		sems[i].sem_value=0;
		sems[i].waittt=NULL;
	}
}

sem_t *sys_sem_open(const char* name,unsigned int value){
	cli();
	int i=0,length=0;
	char c;
	int isfull=-1;
	for(i=0; i<SEM_LEN; i++)
		SemName[i]='\0';
	while((c=get_fs_byte(name+(length++)))!='\0');
	if(length>SEM_LEN){
		sti();
		return NULL;
	}
	for(i=0;i<length;i++)
		SemName[i]=get_fs_byte(name+i);
	/*printk("SemName = %s\n",SemName);*/
	/*查询，若信号已存在，则输出*/
	for(i=0; i<SEM_NUM;i++){
		if(!strcmp(sems[i].sem_name,SemName)){
			sti();
			/*printk("pos=%d\n",i);*/
			return sems+i;
		}
		if(sems[i].free==0 && isfull==-1){
			isfull=i;
			break;
		}
	}
	/*信号数目满了，返回空*/
	/*printk("isfull=%d\n",isfull);*/
	if(isfull==-1){
		sti();
		return NULL;
	}
	
	sems[isfull].free=1;
	sems[isfull].sem_value=value;
	sems[isfull].waittt=NULL;
	
	for(i=0; i<length;i++)
		sems[isfull].sem_name[i]=SemName[i];
	/*printk("%d %d %s\n",sems[isfull].free,sems[isfull].sem_value,sems[isfull].sem_name);*/
	sti();
	return sems+isfull;
}

int sys_sem_wait(sem_t *sem){
	cli();
	sem->sem_value--;
	/*printk("sem->name=%s sem->value=%d\n",sem->sem_name,sem->sem_value);*/
	if(sem->sem_value<0)
		sleep_on(&(sem->waittt));
	sti();
	return 0;
}

int sys_sem_post(sem_t *sem){
	cli();
	sem->sem_value++;
	printk("postsem->value=%d\n",sem->sem_value);
	if(sem->sem_value<=0)
		wake_up(&(sem->waittt));
	sti();
	return 0;
}

int sys_sem_unlink(const char* name){
	cli();
	int i,length=0,j;
	char c;
	for(i=0; i<SEM_LEN; i++)
		SemName[i]='\0';
	while((c=get_fs_byte(name+(length++)))!='\0');
	if(length>SEM_LEN){
		sti();
		return -1;
	}
	for(i=0; i<length; i++)
		SemName[i]=get_fs_byte(name+i);
	for(i=0; i<SEM_NUM; i++){
		if(!strcmp(sems[i].sem_name,SemName)){
			/*printk("same = %d",i);*/
			sems[i].free=0;
			sems[i].sem_value=0;
			sems[i].waittt=NULL;
			for(j=0; j<length; j++)
				sems[i].sem_name[j]='\0';
			sti();
			return 0;
		}
	}
	sti();
	return -1;
}
