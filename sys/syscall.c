#include <sys/syscall.h>
#include <sys/kprintf.h>
#include <sys/vmmu.h>
#include <sys/task.h>

extern struct task_struct *current_task;
int sys_fork_funtion(){

        return (int)sys_fork();
}



void syscall_handler(registers_t *r){

	uint64_t sys_num  = r->rax;
	uint64_t reg_val1 = r->rdi;
	uint64_t reg_val2 = r->rsi;
	uint64_t reg_val3 = r->rdx;

	//__asm__ __volatile__("movq %%rbx, %0;":"=a"(reg_val1):);
	//__asm__ __volatile__("movq %%rsi, %0;":"=a"(reg_val2):);
	//__asm__ __volatile__("movq %%rdx, %0;":"=a"(reg_val3):);

	kprintf("\nSYSCALL han %x  %x %x ",reg_val1, reg_val1, reg_val1 );

	// kprintf("\n %x", r);
	uint64_t call_result =0;
	switch(sys_num){
	
	case s_read:  call_result = sys_read(reg_val1,(void *) reg_val2, reg_val3); 
			 __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
	break;
	case s_write:  kprintf("\nWrite system call");
			call_result = sys_write(reg_val1, (void *)reg_val2, reg_val3);
			 __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
	break;
        case s_open: call_result = sys_open((const char *)reg_val1, reg_val2);
                         __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
        break;
        case s_close: call_result = sys_close(reg_val1);
                         __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");

	break;
        case s_fork: call_result = sys_fork_funtion();
                         __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
	
        break;
	default: kprintf("Syscall error");
		
	}	

	r->rax = call_result;

}


int sys_read(uint64_t fd,void *buf,uint64_t len)
{
    struct file *filep;
    if(Invalid_FD(fd))
        return -9;
   // struct task_struct *curr_task;
    filep = NULL;//(struct file *)curr_task->files[fd];
    if(!filep)
        return -9;
    return 6;// filep->f_op->read(filep, buf, len, &filep->f_pos);
}

int sys_open(const char *file_name, int flags)
{
	kprintf("In sys write");
	return 1;
}

int sys_close(uint64_t fd){
//kprintf("In sys write");
//	current_task->fd[fd] = NULL;
	return 1;
}

int sys_write(uint64_t fd, void *buf, int len){
//kprintf("\n%s , %len", buf, len);

              kprintf((char *)buf);
               // return len;
        
        return -1;
}


// init msr registers
void syscall_init() 
{
    uint32_t hi, lo;
   // uint32_t msr;
    
   
    __asm__ __volatile__("rdmsr;" "or $0x1, %%rax;" "wrmsr":: "c"(MSR_EFER));
    
    hi = 0x00130008; 
    lo = 0x00000000; 
   
    //
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(MSR_STAR));
    
    //function to enter syscalls	
    hi = (uint32_t)((uint64_t) syscall_handler >> 32); //lo target RIP
    lo = (uint32_t) (uint64_t) syscall_handler; //high target RIP
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(MSR_LSTAR));
   
    //remove Interrupts from the kernel     
    hi = 0x00000000; //reserved
    lo = 0x00000000; //set EFLAGS Mask
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(MSR_CSTAR));

}



/*   File functions  */
int read(int ilep, int buf, int len, int filep)
{
  
return 1;
 
}

/* process functions */

//fork function call
/*int sys_fork_funtion(){
   
	return (int)sys_fork();
}
*/

/*int sys_close(uint64_t fd){


}*/
