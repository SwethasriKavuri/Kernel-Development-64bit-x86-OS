#include <sys/terminal.h>
#include <sys/kprintf.h>

struct file t_file = {
	.f_count = 1
};

struct file *t_open(void){
	t_file.f_count++;
	return (struct file *) &t_file;
}

int t_read(struct file *fp,  void *buf, uint64_t count){
return 0;
}

int t_write(struct file *fp,  void *buf, uint64_t count){
/*	struct terminal_buf *tb;	
	ssize_t num_written;
	
	if(!fp || !buf)
           return -EINVAL;
	tb = (struct terminal_buf *)fp->private_data;
	if(!tb)
           return -EINVAL;

    num_written = 0;
    while(count--) {
        putch(*buf++);
        num_written++;
    }
    move_csr();
    return num_written;
*/
	return 0;
}

int t_close(struct file* fp){
	if(!fp)
	kprintf("File is not present");
	fp->f_count--;
	return 0;
}

