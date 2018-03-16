#include "userprog/syscall.h"
#include <stdio.h>
#include "threads/vaddr.h"
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/pagedir.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

/***  	EVERYWHERE "ERROR" IS WRITTEN IN RYANTIMWILSON PASS IN -1 SAME WITH CLOSE_ALL 
REMOVE THIS COMMENT BEFORE SUBMISSION*/
#define MAX_ARGS 3
#define USER_VADDR_BOTTOM ((void *) 0x08048000)

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

struct lock filesystem_lock;

void syscall_SYS_EXIT(int kernelStatus)
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
	int arguments[3];
	/* Stores the physical page pointer. */
    void * physical_page_pointer;
	
  switch(syscall_num) {//exec
	  case SYS_WRITE:
	   syscall_SYS_WRITE(f);
	   break;
          
	  case SYS_EXIT:
                 syscall_SYS_EXIT(arguments[0]);
				 break;
	  case SYS_HALT:
		systemHalt();
		break;
	  case SYS_WAIT: 
	  get_stack_arguments(f, &arguments[0], 1);
	  f->eax = process_wait(arguments[0]);
		break;
	  case SYS_CREATE:
		break;
	  case SYS_REMOVE:
		break;
	  case SYS_OPEN:
		/*get_stack_arguments(f, &arguments[0], 1);
		arguments[0] = user_to_kernel_pointer((const void *) arguments[0]);
		f->eax = open((const char *) arguments[0]);*/
		break;
	  case SYS_FILESIZE:
		break;
      case SYS_READ:
		break;
	  case SYS_SEEK:
		break;
	  case SYS_TELL:
		break;
	  case SYS_CLOSE:
		break;
	  default:   
            printf("System call: <%d>!\n",syscall_num);
	    break;
  }

}// end of syscall handler

void systemHalt (void) {
	shutdown_power_off();
}

/* This code is copied from Ryantimwilson repository that is linked the Design2.txt. It gets the arguments for the frame and sets them to the 3 positions in
in args array. */
void get_stack_arguments (struct intr_frame *f, int *args, int num_of_args)
{
  int i;
  int *ptr;
  for (i = 0; i < num_of_args; i++)
    {
      ptr = (int *) f->esp + i + 1;
      check_valid_addr((const void *) ptr);
      args[i] = *ptr;
    }
}

//checks to make sure the passed in pointer is valid
void check_valid_addr(const void *vaddr)
{
		if(!is_user_vaddr(vaddr) || vaddr < USER_VADDR_BOTTOM)
		{
			syscall_SYS_EXIT(-1);
		}
}
/* Remove the file from the file system, and return a boolean indicating
   the success of the operation. */
bool removeFile (const char *file)
{
  lock_acquire(&filesystem_lock);
  bool was_file_removed = filesys_remove(file);
  lock_release(&filesystem_lock);
  return was_file_removed;
}

/* Inspiration from Ryantimwilson Repo */
int user_to_kernel_pointer(const void *validAddress)
{
  check_valid_addr(validAddress);
  void *pointer = pagedir_get_page(thread_current()->pagedir, validAddress);
  if (!pointer) {
      syscall_SYS_EXIT(-1);
  }
  return (int) pointer;
}

/* Inspiration from RyanTWilson Repo 
int open (const char *file)
{
  lock_acquire(&filesystem_lock);
  struct file *f = filesys_open(file);
  if (!f)
    {
      lock_release(&filesystem_lock);
      return -1;
    }
  int fd = process_add_file(f);
  lock_release(&filesystem_lock);
  return fd;
}

int process_add_file (struct file *f)
{
  struct process_file *pf = malloc(sizeof(struct process_file));
  pf->file = f;
  pf->fd = thread_current()->fd;
  thread_current()->fd++;
  list_push_back(&thread_current()->file_list, &pf->elem);
  return pf->fd;
}
*/
