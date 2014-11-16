#include <unistd.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <errno.h>
#include <asm/system.h>
#include <asm/segment.h>

sem_t sems[SEM_NUM];
char thisname[SEM_LEN];


int sys_sem_open(const char *name,unsigned int value){
			
	int i,lenth=0,pos=-1;
	char c;
	while((c=get_fs_byte(name+(lenth++)))!='\0');
	if(lenth>SEM_LEN){
		
		printk("too long\n");
		return -1;
	}
	for(i=0;i<lenth;i++)
		thisname[i]=get_fs_byte(name+i);
	for(i=0;i<SEM_NUM;i++){
		if(!strcmp(sems[i].name,thisname)){
			printk("already existed\n");
			
			return i;
		}
		if(sems[i].free==0 && pos==-1) pos=i;
	}
	if(pos==-1){
		printk("overload\n");
		
		return -1;
	}
	sems[pos].value=value;
	sems[pos].free=1;
	sems[pos].list=NULL;
	for(i=0;i<lenth;i++)
		sems[pos].name[i]=thisname[i];
	/*printk("semname=%s  semvalue=%d pos=%d\n",sems[pos].name,sems[pos].value,pos);*/	
	
	return pos;
}

int sys_sem_wait(int fdd){
	cli();
	/*printk("fdd=%d\n",fdd);*/
	lists *temp;
	temp = (lists *)malloc(sizeof(lists));
	sems[fdd].value--;
	/*printk("semName=%s,semValue=%d\n",sems[fdd].name,sems[fdd].value);*/
	if(sems[fdd].value<0){
		temp->task = current;
		current->state = TASK_UNINTERRUPTIBLE;
		temp->next = sems[fdd].list;
		sems[fdd].list = temp;
		schedule();
	}
	sti();
	return 0;	
}

int sys_sem_post(int fdd){
	cli();
	sems[fdd].value++;
	/*printk("semName=%s,semValue=%d\n",sems[fdd].name,sems[fdd].value);*/
	if(sems[fdd].value<=0){
		sems[fdd].list->task->state=TASK_RUNNING;
		sems[fdd].list=sems[fdd].list->next;
	}
	sti();
	return 0;
}

int sys_sem_unlink(const char* name)
{
 	int i,lenth=0;
	char c;
	while((c=get_fs_byte(name+(lenth++)))!='\0');
	if(lenth>SEM_LEN) return -1;
	if(sems[i].free == 1) 	
     	{
		   if(strcmp(sems[i].name,name)== 0 )
		   {
		   sems[i].free = 0;
		   return 0;
		   }
     	}
	return -1;	
}
