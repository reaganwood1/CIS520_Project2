#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void syscall0xff(struct intr_frame *f)
{

printf("%c",f->ebx);

}

void syscall0x00(void)
{

thread_exit();

}// function to exit the current thread use echo.c ass a reference

void
syscall_init(void){
  intr_register_int (0x30,3,INTR_ON,syscall_handler,"syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{  uint32_t * esp = (uint32_t *)f-> esp;
  printf("System call: <%d> file descripter: <%x> Pointer <%d> Size: <%d>!\n",*esp,*(esp+4),*(esp+8), *esp+12);
/* switch(f->eax){
    case 0xff:
  syscall0xff(f);
  break;
    case 0x00:
    syscall0x00();
     break;
  default:
  printf("System Call!\n");
  break;
  }// end case for eax parameter
  */

}// end of syscall handler
