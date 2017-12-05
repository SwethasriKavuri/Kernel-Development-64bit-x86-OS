#ifndef _TERMIMAL_H
#define _TERMINAL_H
#include<sys/vfs.h>
#include<sys/defs.h>

struct file *t_open(void);
int t_read(struct file *fp,  void *buf, uint64_t count);
int t_write(struct file *fp,  void *buf, uint64_t count);
int t_close(struct file* fp);


#endif
