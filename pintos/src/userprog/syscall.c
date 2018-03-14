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

void
syscall_init(void){
  intr_register_int (0x30,3,INTR_ON,syscall_handler,"syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  syscall0xff(f);

//  thread_exit ();
}
