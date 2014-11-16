#pragma once  
#define __LIBRARY__  
#include <unistd.h>  
#include <linux/mm.h>  
#include <linux/sched.h>  
#include <asm/system.h>  
#include <linux/kernel.h> 

#define PAGESIZE 4096
#define ENOMEM      12  
#define EINVAL      22

int pagelist[22];

int sys_shmget(int key, int size){
	int phyc_mem_addr=0;
	if(pagelist[key]!=0) return pagelist[key];
	else{
		if(size > PAGESIZE) return (-EINVAL);
		if(!(phyc_mem_addr=get_free_page())){
			/*printk("phyc_addr = %d\n",phyc_mem_addr);*/			
			return (-ENOMEM);
		}
		pagelist[key]=phyc_mem_addr;
		return phyc_mem_addr;
	}
}

void *sys_shmat(int shmid){
	/*printk("shmid=%d\n",shmid);*/
	if(!shmid) return (-EINVAL);
	put_page(shmid,current->start_code + current->brk);
	return current->brk;
}

