#include <linux/kernel.h>
#include <stdarg.h>
#include <unistd.h>
#include <asm/segment.h>  
#include <linux/fs.h>  
#include <linux/sched.h>  
#include <sys/stat.h>  
#include <fcntl.h>
#include <sys/types.h>  
#include <string.h>
#include <linux/config.h>
#include <errno.h>
#define BufferSize 1024

#define set_bit(bitnr,addr) ({ \
register int __res ; \
__asm__("bt %2,%3;setb %%al":"=a" (__res):"a" (0),"r" (bitnr),"m" (*(addr))); \
__res; })

char *procbuffer;

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args; int i;
	va_start(args, fmt);
	i=vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}

int read_psinfo(char* buf, int count, off_t* f_pos){
	struct task_struct *p;
	int i,j,k;
	procbuffer = (char*)malloc(BufferSize*sizeof(char));
	/*printk("buf=%s  count=%d  f_pos=%d\n",buf,count,(*f_pos));*/
	if(!(*f_pos)){
		k = sprintf(procbuffer,"%s","pid state father counter start_time\n");
		for(i=0; i<64; i++){
			p = task[i];
			if(p!=NULL){
	j = sprintf(procbuffer+k, "%d\t%d\t%d\t%d\t%d\n", p->pid,p->state,p->father,p->counter,p->start_time);
				k = k+j;
			}
		}
	}
	for(j=0;j<count;j++)  
	{  
	    if(procbuffer[j+(*f_pos)] == '\0')  
	      break;  
	    put_fs_byte(procbuffer[j+(*f_pos)],buf+j+(*f_pos));     
	}  
	(*f_pos) = (*f_pos)+j; 
	free(procbuffer);
	return j;  							
}

int read_hdinfo(struct m_inode* inode,char* buf, int count, off_t* f_pos){
	struct super_block * sb;
	int i,j,k,free=0,total_blocks,total_inodes;
	/*printk("i_dev=%d\n",inode->i_dev);*/
	sb=get_super(inode->i_dev);
	procbuffer = (char*)malloc(BufferSize*sizeof(char));
	total_blocks = sb->s_nzones;
	total_inodes = sb->s_ninodes;  
	k=sprintf(procbuffer,"total_blocks:%d\n",total_blocks);
	i = total_blocks;
	while(--i>=0)
		if(!set_bit(i&8191,sb->s_zmap[i>>13]->b_data))
			free++;
	j=sprintf(procbuffer+k,"free_blocks:%d\n",free);
	k=k+j;
	j=sprintf(procbuffer+k,"used_blocks:%d\n",total_blocks-free);
	k=k+j;
	j=sprintf(procbuffer+k,"total_inodes:%d\n",total_inodes);
	k=k+j;
	sprintf(procbuffer+k,"...\n");
	for(j=0;j<count;j++)  
	{  
	    if(procbuffer[j+(*f_pos)] == '\0')  
	      break;  
	    put_fs_byte(procbuffer[j+(*f_pos)],buf+j+(*f_pos));     
	}
	(*f_pos) = (*f_pos)+j;  
	free(procbuffer); 
	return j;
}

int read_inodeinfo(struct m_inode* inode,char* buf, int count, off_t* f_pos){	
	struct super_block* sb;
	struct m_inode* mi;
	int i,j,k=0;
	sb=get_super(inode->i_dev);
	procbuffer = (char*)malloc(BufferSize*sizeof(char));	
	for(i=1; i<=10; i++){
		mi = iget(sb->s_dev,i);
		j = sprintf(procbuffer+k, "inr:%d;zone[0]:%d\n" ,i , mi->i_zone[0]);
		k = k+j;
		iput(mi);
	}
	/*
	struct super_block* sb;
	struct m_inode* mi;
	int i,j,k=0;	
	sb=get_super(inode->i_dev);
	i=sb->s_ninodes+1;
	procbuffer = (char*)malloc(BufferSize*sizeof(char));	
	while(--i >= 0)
		if(set_bit(i&8191,sb->s_imap[i>>13]->b_data)){
			mi=iget(inode->i_dev,sb->s_ninodes+1-i);
			j = sprintf(procbuffer+k, "inr:%d;zone[0]:%d\n" ,sb->s_ninodes+1-i , mi->i_zone[0]);
			k = k+j;
			iput(mi);
		}
	*/
	for(j=0;j<count;j++)  
	{  
	    if(procbuffer[j+(*f_pos)] == '\0')  
	      break;  
	    put_fs_byte(procbuffer[j+(*f_pos)],buf+j+(*f_pos));     
	}
	(*f_pos) = (*f_pos)+j; 
	free(procbuffer);  
	return j;
}

int proc_read(struct m_inode* inode, char* buf, int count, off_t* f_pos){
	/*printk("dev=%d buf=%s count=%d f_pos=%d\n",dev,buf,count,(*f_pos));*/
	switch (inode->i_zone[0]){
		case 0:
			return read_psinfo(buf,count,f_pos);
			break;
		case 1:
			return read_hdinfo(inode,buf,count,f_pos);
			break;
		case 2:
			return read_inodeinfo(inode,buf,count,f_pos);
			break;
		default:
			break;
	}
	return 0;
}
