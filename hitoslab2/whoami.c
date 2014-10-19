#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>
_syscall2(int, whoami,char*,name,unsigned int,size);
/*调用结果*/
/*宏展开，调用寄存器值*/

void main(int argc,char* argv[]){
	char* outName;
	/*申请内存，调用char类型*/
	outName = (char*)malloc(sizeof(char)*23);
	whoami(outName,23);
	printf("%s\n",outName);
}
