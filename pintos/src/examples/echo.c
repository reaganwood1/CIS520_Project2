#include <stdio.h>
#include <syscall.h>
// passing parameter to the register before invoking the funciton

int
main (int argc, char **argv)
{
  int i;
printf("hello world!");
for(;;);
/*
  for (i = 0; i < argc; i++)
  // this is the user space printf
  printf ("%s ", argv[i]);
  printf ("\n");
*/
  return EXIT_SUCCESS;
}
