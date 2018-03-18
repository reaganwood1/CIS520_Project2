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
bool create (const char * file, unsigned initial_size);
struct file* process_get_file (int fd);

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

/* This lock ensures only one process is interacting with the file system at a given time. */
struct lock filesystem_lock;

/* Inspiration from RyantTWilson repository. The process file is used as a structure for a file that is added to being used by a thread. It has a file descriptor, a file pointer, and a list element pointer  */
struct process_file {
	  struct file *file;
	    int file_descriptor;
	      struct list_elem elem;
};

void syscall_SYS_EXIT(int kernelStatus)
{
	struct thread *current = thread_current();
	printf ("%s: exit(%d)\n", current->name, kernelStatus);
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
		  /* Must check for vali buffer and then must get the actual physical pointer that will be written to*/
		get_stack_arguments(f, &arguments[0], 3);
		check_valid_buffer((void *) arguments[1], (unsigned) arguments[2]);
		arguments[1] = user_to_kernel_pointer((const void *) arguments[1]);
		f->eax = write(arguments[0], (const void *) arguments[1], (unsigned) arguments[2]);
	   break;
          
	  case SYS_EXIT:
	   /* exit just needs access to the first argument so exit can print properly */
		get_stack_arguments(f, &arguments[0], 1);
                 syscall_SYS_EXIT(arguments[0]);
				 break;
	  case SYS_HALT:
				 // All that needs to be done is to shutdown the power for system halt.
		systemHalt();
		break;
	  case SYS_WAIT: 
		// get the arguments off of the stack
	  get_stack_arguments(f, &arguments[0], 1);
	  f->eax = process_wait(arguments[0]);
		break;
	  case SYS_CREATE:
		// gets an argument to the file descriptor and then begins creating the file.
		get_stack_arguments(f, &arguments[0], 1);
		arguments[0] = user_to_kernel_pointer((const void *) arguments[0]);
		f->eax = create((const char *) arguments[0], (unsigned) arguments[1]);
		break;
	  case SYS_REMOVE:
		break;
	  case SYS_OPEN:
		// gets a pointer to the file descriptor and then tries to open the file.
		get_stack_arguments(f, &arguments[0], 1);
		arguments[0] = user_to_kernel_pointer((const void *) arguments[0]);
		f->eax = open((const char *) arguments[0]);
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
	// check that the valid address is being used. If it's not, exit the thread.
		if(!is_user_vaddr(vaddr) || vaddr < USER_VADDR_BOTTOM)
		{
			syscall_SYS_EXIT(-1);
		}
}
/* Remove the file from the file system, and return a boolean indicating
   the success of the operation. */
bool removeFile (const char *file)
{
	// use the file system to remove the file. Manage locks. Return whether the file was successfully removed.
  lock_acquire(&filesystem_lock);
  bool was_file_removed = filesys_remove(file);
  lock_release(&filesystem_lock);
  return was_file_removed;
}

/* Inspiration from Ryantimwilson Repo */
int user_to_kernel_pointer(const void *validAddress)
{
	// check that the address is valid. 
  check_valid_addr(validAddress);
  void *pointer = pagedir_get_page(thread_current()->pagedir, validAddress); // get a pointer to the page
  if (!pointer) { // if the pointer could not be retrieved, exit
      syscall_SYS_EXIT(-1);
  }
  return (int) pointer; // return the valid pointer
}

/* Inspiration from RyanTWilson Repo */
int open (const char *file)
{
  lock_acquire(&filesystem_lock);
  struct file *f = filesys_open(file); // open the file using the file system functionality
  if (!f) // if the file doesn't eist, release the lock and return an invalid integer
    {
      lock_release(&filesystem_lock);
      return -1;
    }
  int fileDescriptor = process_add_file(f); // add the file signifying an open and then return the fileDescriptor number
  lock_release(&filesystem_lock);
  return fileDescriptor;
}

/* Inspiration from RyanTWilson Repo. This adds a file to a process. Each thread keeps track of the files its accessing. This takes a file and utilizes those fields. */
int process_add_file (struct file *f)
{
  struct process_file *pf = malloc(sizeof(struct process_file)); // get a pointer to the process file
  pf->file = f; // set the file
  pf->file_descriptor = thread_current()->file_descriptor; // set the file descriptor for the file
  thread_current()->file_descriptor++; // 
  list_push_back(&thread_current()->file_list, &pf->elem); // push the new file to the back of the file list for the thread
  return pf->file_descriptor;
}

// Inspiration from RyanTWilson repository
bool create (const char * file, unsigned initial_size) {
	lock_acquire(&filesystem_lock);
	// pretty simple here, use the filesystem function to create
	bool success = filesys_create(file, initial_size);
	lock_release(&filesystem_lock);
	return success;
}

/* Inspiration from RyantTWilson */
int write (int fd, const void *buffer, unsigned size) {
	if (fd == STDOUT_FILENO)
    {
      putbuf(buffer, size);
      return size;
    } 
  lock_acquire(&filesystem_lock);
  struct file *f = process_get_file(fd); // get the file from the file desriptor, check if it's valid.
  if (!f)
    {
      lock_release(&filesystem_lock);
      return -1;
    } 
  int bytes = file_write(f, buffer, size); // pass the file, the buffer, and the size and the bytes written will be returned.
  lock_release(&filesystem_lock);
  return bytes;
}

/* Inspiration from RyanTWilson */
void check_valid_buffer (void* buffer, unsigned size)
{
  unsigned i;
  char* local_buffer = (char *) buffer;
  // loop through each address, check if they are valid and then check the next position
  for (i = 0; i < size; i++)
    {
      check_valid_addr((const void*) local_buffer);
      local_buffer++; // local buffer keeps track of the next buffer location to check. 
    }
}

/* Inspiration from RyanTWilson repo */
struct file* process_get_file (int fd)
{
  struct thread *t = thread_current();
  struct list_elem *e;

  // loops through the files and gets the one which matches the file descriptor for the given processes. 
  for (e = list_begin (&t->file_list); e != list_end (&t->file_list);
       e = list_next (e))
        {
		// checks for a matching file descriptor
          struct process_file *pf = list_entry (e, struct process_file, elem);
          if (fd == pf->file_descriptor)
	    {
	      return pf->file;
	    }
        }
  // if no file descriptor is found, return
  return NULL;
}
