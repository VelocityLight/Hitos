
/*
#include <unistd.h>
#include <string.h>
#include <const.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <termios.h>
#include <utime.h>
#include <fcntl.h>
#include <linux/config.h>
#include <linux/fdreg.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <linux/head.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sys.h>
#include <linux/tty.h>  
#include <sys/types.h>  
#include <sys/stat.h>   
#include <asm/segment.h>  
*/

#include <linux/sched.h>  
#include <linux/tty.h>  
#include <linux/kernel.h>  
#include <asm/segment.h>  
#include <string.h>
#include <errno.h>

char OsName[23];
int Nlen;

int sys_iam(const char* name){
	int i=0;
	for(i=0;get_fs_byte(&name[i])!='\0';i++)
	{
		if(i>22){
			return (-EINVAL);
		}
	/*	printk("%c",name[i]);*/
	}
	Nlen=i;
	/*看测试文件会发现超过长度时OsName不变...*/
	for(i=0;i<Nlen;i++)
		OsName[i]=get_fs_byte(&name[i]);
	OsName[i]='\0';
	return Nlen;
}

int sys_whoami(char* name,unsigned int size){
	int i=0;
	if(size<Nlen){
		return (-EINVAL);
	}
	for(i=0;i<Nlen;i++)
		put_fs_byte(OsName[i],&name[i]);
	/*printk("name=%s\n",name);*/
	name[i]='\0';
	return Nlen;
}
