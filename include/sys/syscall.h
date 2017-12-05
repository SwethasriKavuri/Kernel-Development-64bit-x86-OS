#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <sys/interrupt.h>

// Intel Machine Specific Registers 
#define MSR_EFER    0xC0000080
#define MSR_STAR    0xC0000081
#define MSR_LSTAR   0xC0000082
#define MSR_CSTAR   0xC0000083
#define MSR_SFMASK  0xC0000084

#define Invalid_FD(fd) ((fd) < 0 || (fd) >= 64)

//file
#define s_read		0
#define s_write		1
#define s_open		2
#define s_close		3
//directory
#define s_opendir	4
#define s_readdir	5
#define s_chdir		10
#define s_getcwd	11

//process
#define s_fork		6
#define s_execve	7
#define s_waitpid	8
#define s_sleep		9
  





void syscall_handler(registers_t *r);
void syscall_init();

int sys_read(uint64_t fd_count,void * addr,uint64_t len);
int sys_write(uint64_t fd_count,void * addr, int len);
int sys_open(const char *file_name, int flags);
int sys_close(uint64_t fd);
//int sys_fork_function();

#endif
