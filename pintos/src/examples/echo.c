#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
  int i;
//__asm__ __volatile__("int $0x30");
  for(;;);
/*
  for (i = 0; i < argc; i++)
  // this is the user space printf
  printf ("%s ", argv[i]);
  printf ("\n");
*/
  return EXIT_SUCCESS;
}
