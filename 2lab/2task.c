#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Кто выполняется первым после fork: отец или сын? Написать программу,
  которую можно использовать для проверки данной ситуации.
  Первым выполняется родитель, но при большой загрузке системы могут быть
  случаи, когда потомок выполнится первым.
*/
/* Func prototypes */
void Err_Handler(int line);


int main(int argc, char const *argv[])
{
  int res = 0;

  res = fork();
  if (res == -1)
    Err_Handler(__LINE__);
  if (!res) { //  дочерний процесс
    printf("Потомок запущен\n");
    } else {    //  родительский процесс
    printf("Родитель запущен\n");
  }

  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
