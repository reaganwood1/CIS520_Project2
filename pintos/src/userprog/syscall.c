#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void syscall_SYS_WRITE(struct intr_frame *f)
{
void * esp = (uint32_t *)f->esp;
uint32_t fd  = *(uint32_t *)(esp+4);
char * buffer = (char *)*(uint32_t *)(esp+8);
uint32_t size = *(uint32_t *)(esp+12);
uint32_t char_cnt = 0;

	while(size--) {
		printf("%c", buffer[char_cnt++]);
	}

}

void syscall_SYS_EXIT(void)
{

thread_exit();

}// function to exit the current thread use echo.c as a reference

void
syscall_init(void){
  intr_register_int (0x30,3,INTR_ON,syscall_handler,"syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{ 
   uint32_t syscall_num = *(uint32_t *)f->esp;

  switch(syscall_num) {
	  case SYS_WRITE:
	   syscall_SYS_WRITE(f);
	   break;
          
	  case SYS_EXIT:
                 syscall_SYS_EXIT();
	  default:   
            printf("System call: <%d>!\n",syscall_num);
	    break;
  }

}// end of syscall handler
