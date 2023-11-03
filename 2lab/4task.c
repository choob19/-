#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Изменить программу п. 3 так, чтобы родительский процесс завершился
  раньше порожденного. Какой процесс становится родительским для порожденного
  после того, как исходный родительский процесс завершился?
  После завершения родительского процесса родителем дочернего процесса становится
  процесс с идентификатором (PID) 1
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res = 0;

  res = fork();
  if (res == -1)
    Err_Handler(__LINE__);
  if (!res) { //  Потомок
    printf("Процесс потомок. PID: %d, PPID: %d\n", getpid(), getppid());
    printf("Выполнение потомка\n");
    sleep(3);
    printf("\nЗавершение потомка с кодом 1. PPID = %d\n", getppid());
    exit(1);
  } else {    //  Родитель
    printf("Процесс родитель. PID: %d, PPID: %d\n", getpid(), getppid());
    printf("Завершение процесса родителя\n");
    exit(0);
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
