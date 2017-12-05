//#include <stdio.h>
#include<sys/defs.h>
#include<sys/syscall.h>


static __inline uint64_t syscall_0(uint64_t n) {
        uint64_t ret = -1;
        __asm volatile("movq %1,%%rax;" // move content of 'n' into rax
                   "int $0x80;"  // call interrupt to run system routione
                   "movq %%rax,%0;" // move the result of the system routine into the ret variable to be returned
                   : "=r"(ret) // list of variables that are outputs 
                   : "r"(n) // list of variables that are inputs 
    : "rax", "memory"); // list of clobbered regs

    return ret;
}

    
static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3)
{
    
    uint64_t a = 1;
     __asm__ volatile("int $0x80" : "=a" (a) : "0" (n), "D" ((uint64_t)a1), "S" ((uint64_t)a2), "b"((uint64_t)a3));
    return a++;
}

void exec_command() {

	
        int pid = (int)syscall_0(57);//sys_fork_function();
        char* str = "abcder";
        syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t)6);
	if (pid) 
          
           { 
		 char* str = "abcder";
                 syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t)6);

           }


	}

int main(int argc, char *argv[], char *envp[]) {
char* str = "abcder";
//uint64_t * ptr = (uint64_t *)u_rsp; 
//*ptr = (uint64_t) userMain;
syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t)6);
//exec_command();
//kprintf("Hello User world!,%p",&userMain);
while(1);
//  puts("sbush> ");
  return 0;
}
