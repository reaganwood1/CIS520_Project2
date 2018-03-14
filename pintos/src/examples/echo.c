#include <stdio.h>
#include <syscall.h>

void prints(const char *s){
    char  next_char ;
  for(;  *s; s++){
    next_char  = *s;
    __asm__ __volatile__ \
      (" movl $65, %ebx\n\
      int $0x30");
      /*
         :  no out put 
        : "b" (next_char)
        :"%ebx"

      );*/
  }

}
int
main (int argc, char **argv)
{
  int i;
//__asm__ __volatile__("int $0x30");
prints("hello world");
for(;;);
/*
  for (i = 0; i < argc; i++)
  // this is the user space printf
  printf ("%s ", argv[i]);
  printf ("\n");
*/
  return EXIT_SUCCESS;
}
