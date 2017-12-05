#ifndef _INTERRUPT_H
#define _INTERRUPT_H
#include <sys/defs.h>

#define PF_P	0x1
#define PF_W	0x2
#define PF_U	0x4

typedef struct registers
{
    uint64_t ds;                  // Data segment selector
    uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax; // Pushed by pusha.
    uint64_t int_no, err_code;    // Interrupt number and error code (if applicable)
    uint64_t rip, cs, rflags, userrsp, ss; // Pushed by the processor automatically.
} registers_t;


extern void register_interrupt_handler(registers_t *reg);
extern void page_fault_handler(registers_t *reg);
#endif
