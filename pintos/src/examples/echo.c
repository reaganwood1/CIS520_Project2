#include <stdio.h>
#include <syscall.h>
// passing parameter to the register before invoking the funciton
void prints(const char *s){
  char  next_char ;
  for(;  *s; s++){
    next_char  = *s;
    __asm__ __volatile__ \
      (" int $0x30"
        :/*no out put from the rgister */
        : "b" (next_char)
        :
        );
    }

}
int
main (int argc, char **argv)
{
  int i;
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
