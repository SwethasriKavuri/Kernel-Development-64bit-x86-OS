#include "sys/task.h"
#include "sys/kprintf.h"
#include "sys/pmap.h"
#include "sys/gdt.h"
#include <sys/syscall.h>
#include <sys/str.h>
#include <sys/idt.h>
#include <sys/irq.h>
#include <sys/elf64.h>
#include <sys/vmmu.h>

static struct task_struct *runningTask;
static struct task_struct mainTask;
static struct task_struct otherTask;
struct task_struct *current;
//static uint64_t u_rsp;
 int processid[50];
static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) 
{
    
    uint64_t a;
    __asm__ volatile("int $0x80" : "=a" (a) : "0" (n), "D" ((uint64_t)a1), "S" ((uint64_t)a2), "b"((uint64_t)a3));
    return a;


}

void switch_to_ring3();


void init_process_map()
{
	int id = 0;
	for(; id < 50; id++)
		processid[id] = 0;
}



int procid()
{
         int i=0;
          for(;i<50;i++) {
		if(processid[i] == 0) {
			processid[i] = 1;
			return i;
		}
	}
	return -1;
}



/*
static void userMain()
{

char* str = "abcder";
uint64_t * ptr = (uint64_t *)u_rsp;
*ptr = (uint64_t) userMain;
syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t) strlength(str));
kprintf("Hello User world!,%p",&userMain);
while(1);

}
*/

// COW (Copy on Write)
void* copy_on_write(struct task_struct *parent)
{
	struct task_struct *child = (struct task_struct *)kmalloc(sizeof(struct task_struct));

	child->task_state = READY;
	child->pid = procid();
	child->ppid = parent->pid ;
        child->wait_on_child_pid = -1;
	parent->num_child += 1;
	
        strcpy(child->task_name,current->task_name);

	/* set child page tables */
	child->regs.cr3 = (uint64_t)page_alloc();

	setup_child_pagetable(child->regs.cr3);

	/* set the child CR3 to allocate memory to its structures*/
	set_CR3((struct PML4 *)child->regs.cr3);

	/* copy vmas */
	struct vm_area_struct *parent_vma = parent->vma_struct;
	struct vm_area_struct *child_vma = NULL;
	int first = 0;

	while(parent_vma) {

		if(child_vma == NULL)
			first = 1;

		child_vma = (struct vm_area_struct *)kmalloc(sizeof(struct vm_area_struct));

		memcpy(child_vma, parent_vma, sizeof(struct vm_area_struct));

		if(parent_vma->file!=NULL){
			child_vma->file = kmalloc(sizeof(struct file_struct));
			memcpy(child_vma->file,parent_vma->file,sizeof(struct file_struct));
		}

		if(first) {
			child->vma_struct = child_vma;
			first = 0;
		}

		if(child_vma->next)
			child_vma = child_vma->next;
		parent_vma = parent_vma->next;
	}

	/* if no vma was there in parent , mark the
	 * child memory map as null and return
	 */
	if(!child_vma) {
		child->vma_struct = NULL;
		return (void *)child;
	}		

	child_vma->next = NULL;

	return (void *)child;

}


static void otherMain() 
{
      kprintf("Hello multitasking world!"); // Not implemented here...
//    changeTask();
//      initialise_syscalls();  
      struct task_struct *user_proc = create_user_process("bin/sbush"); 
      switch_to_ring3(user_proc);
      kprintf("Back to Hello multitasking world!");
      while(1);

}
 

void initTasking() 
{

   __asm__ __volatile__("movq %%cr3, %%rax; movq %%rax, %0;":"=g"(mainTask.regs.cr3)::"memory"); 
   __asm__ __volatile__("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;":"=g"(mainTask.regs.rflags)::"memory");
    createTask(&otherTask, otherMain,mainTask.regs.rflags, (uint64_t*)mainTask.regs.cr3);
    mainTask.next = &otherTask;
    otherTask.next = &mainTask; 
    runningTask = &mainTask;
    
}
 
void createTask(struct task_struct *task, void (*otherMainIP)(),uint64_t flags, uint64_t *pagedir) 
{
    task->regs.rax = 0;
    task->regs.rbx = 0;
    task->regs.rcx = 0;
    task->regs.rdx = 0;
    task->regs.rsi = 0;
    task->regs.rdi = 0;
    task->regs.rflags = flags;
    task->regs.rip = (uint64_t) otherMainIP;
    task->regs.cr3 = (uint64_t) pagedir;
    task->regs.rsp = (page_alloc() +(uint64_t)0xFA0);
    task->next = 0;
    uint64_t * ptr = ( uint64_t *) (task->regs.rsp + 0x30);
    * ptr = task->regs.rip;
}
 
void switchTask(struct task_struct *from, struct task_struct *to)

{
    __asm__ __volatile__("pushq %rax");
    __asm__ __volatile__("pushq %rbx");
    __asm__ __volatile__("pushq %rcx");
    __asm__ __volatile__("pushq %rdx"); 
    __asm__ __volatile__("pushq %rsi");
    __asm__ __volatile__("pushq %rdi");
    __asm__ __volatile__("movq %%rsp, %0":"=g"(from->regs.rsp)::"memory");
    __asm__ __volatile__("movq %0, %%rsp"::"m"(to->regs.rsp));
    __asm__ __volatile__("popq %rdi");
    __asm__ __volatile__("popq %rsi");
    __asm__ __volatile__("popq %rdx");
    __asm__ __volatile__("popq %rcx");
    __asm__ __volatile__("popq %rbx");
    __asm__ __volatile__("popq %rax");
    __asm__ __volatile__("retq");
  
}


void changeTask()

{
  struct task_struct *last = runningTask;
    runningTask = runningTask->next;
    switchTask(last, runningTask);
}

void switch_to_ring3(struct task_struct *user_process)
{

user_process->task_state = RUNNING;

//uint64_t funcadd = (uint64_t)userMain;

uint64_t funcadd = user_process->regs.rip;

//uint64_t * rsp; 

//u_rsp = (page_alloc() +(uint64_t)0xFA0);

__asm__ __volatile__("cli");
__asm__ __volatile__("movq %%rsp,%0;":"=r"(user_process->kernel_rsp)::"memory");

set_tss_rsp((void *)user_process->kernel_rsp);

__asm__ __volatile__("movq %0,%%cr3;": : "r"(user_process->regs.cr3));
__asm__ __volatile__("movq %0,%%rax;": : "r"(user_process->regs.rsp));
__asm__ __volatile__("pushq $0x23");
__asm__ __volatile__("pushq %rax");
__asm__ __volatile__("pushfq");
/*__asm__ __volatile__("popq %rax");
__asm__ __volatile__("orq $0x200,%rax");
__asm__ __volatile__("pushq %rax");
*/
__asm__ __volatile__("pushq $0x2B");
__asm__ __volatile__("pushq %0"::"r"(funcadd):"memory");
__asm__ __volatile__("sti");
__asm__ __volatile__("iretq");

}

struct task_struct* create_user_process(char *filename)
{
	struct task_struct *newProc = (struct task_struct *)kmalloc(sizeof(struct task_struct));

	newProc->regs.cr3 = (uint64_t)set_user_AddrSpace();
	
        void *kStack = (void *)kmalloc(4096);

	newProc->init_kern = newProc->kernel_stack = ((uint64_t)kStack + 4096 - 8);        

        struct PML4 *curr_CR3 = (struct PML4 *)get_CR3();

	set_CR3((struct PML4 *)newProc->regs.cr3);

	int error = load_exe(newProc,filename);

	if(error)
		return NULL;

	set_CR3((struct PML4 *)curr_CR3);
	return newProc;
}

int sys_fork()
 
 {  
kprintf("Sanjay Reddy\n");  
  struct task_struct *temp, *parent = NULL;	
  
  struct task_struct *child = (struct task_struct *)copy_on_write((struct task_struct*)current);
  
  temp = current->next;
  
  current->next = child;
  
  child->next = temp;
  
  parent = current;
  
  void *kstack = (void *)kmalloc(4096);
  
  child->init_kern = child->kernel_stack = ((uint64_t)kstack + 4096 - 8);
  
  memcpy((void *)(child->init_kern - 0x1000 +8), (void *)(parent->init_kern - 0x1000 +8), 4096-8);
  
  volatile uint64_t stack_pos;   

  
/*
  for(int i=STACK_OFFSET; i<STACK_OFFSET+NUM_REGISTERS_SAVED+1; i++) 
                  {
			child->kstack[i]=parent>kstack[i+1];
		  }

    child->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-6] = 0;  // assigning rax of child to zero;
    
    child->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-5] = parent->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-5];                           // EIP
    
    child->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-4] = parent->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-4];                           // CS
    
    child->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-3] = 0x200286;                           // EFLAGS - 0x200286
    
    child->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-2] = parent->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-2];                           // ESP
    
    child->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-1] = parent->kstack[KERNEL_STACK_SIZE-TSS_OFFSET-1];                           // SS
       
    child->kernel_rsp = (uint64_t *)(&child->kstack[STACK_OFFSET])
*/



set_CR3((struct PML4 *)parent->regs.cr3);

	__asm__ __volatile__(
                "movq $2f, %0;"
		"2:\t"
                :"=g"(child->regs.rip)
        );

	__asm__ __volatile__(
                "movq %%rsp, %0;"
                :"=r"(stack_pos)
        );

	if(current == parent)       
        {
		/* if parent is executing, set the child kernel stack
		 * before leaving the ring 0
		 */
		child->kernel_stack = child->init_kern - (parent->init_kern - stack_pos);
       		return child->pid;
	}
	
        else 
       
        {
       		outb(0x20, 0x20);
        	return 0;
  	}

        return 0;
}


