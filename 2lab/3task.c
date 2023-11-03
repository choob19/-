#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Написать программу, в которой процесс порождает новый и позволяет
  порожденному процессу завершиться. Оба процесса должны выводить свои
  идентификаторы (процесса, родительского процесса, группы процессов).
  Проверить идентификатор и статус завершения порожденного процесса.
  Выполнить данную программу и посмотреть ее результаты.
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res = 0, status = 0;

  res = fork();
  if (res == -1)
    Err_Handler(__LINE__);
  if (!res) { //  Потомок
    printf("Процесс потомок. PID: %d, PPID: %d\n", getpid(), getppid());
    printf("Выполнение потомка\n");
    sleep(1);
    printf("\nЗавершение потомка с кодом 1\n");
    exit(7);
  } else {    //  Родитель
    printf("Процесс родитель. PID: %d, PPID: %d\n", getpid(), getppid());
    printf("Процесс родитель ожидает завершения потомка...\n");
    res = wait(&status);
    if (res < 0)
      Err_Handler(__LINE__);
    printf("Дочерний процесс завершился. res = %d, status = %d\n", res, WEXITSTATUS(status));
    /* Родительскому процессу возвращается status & 0377 */
  }
  return 0;
}
/*
  Макросы для wait() (из <sys/wait.h>)
  int WIFEXITED (status);
  int WIFSIGNALED (status);
  int WIFSTOPPED (status);
  int WIFCONTINUED (status);
  int WEXITSTATUS (status);
  int WTERMSIG (status);
  int WSTOPSIG (status);
  int WCOREDUMP (status);
*/
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
