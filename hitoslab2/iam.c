#define __LIBRARY__
#include <unistd.h>
_syscall1(int, iam, const char*, name);
/*宏展开，宏序号至eax，参数至ebx，结果存入_res，通过其判断其API返回*/

void main(int argc,char* argv[]){
	iam(argv[1]);
}
