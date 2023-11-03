#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Написать программу, исключающую появление зомби для завершающихся порожденных процессов.
  (Процесс потомок при смерти может послать сигнал SIGCHLD родителю, который его обработает)
*/

void Err_Handler(int line);
void signal_handler(int signum);

int main(int argc, char const *argv[])
{
  int res = 0;
  struct sigaction act;
  act.sa_handler = SIG_IGN; //  Если явно игнорить SIGCHLD, то потомок не будет становиться зомби при завершении
  act.sa_flags |= SA_NOCLDWAIT; //  флаг для авто снятия дочерних процессов, не обязателен, если ранее стоит SIG_IGN

  /* SIGCHLD игнорируется по дефолту */
  if (sigaction(SIGCHLD, &act, NULL) < 0) Err_Handler(__LINE__);
  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок
    exit(0);
  } else {    //  Родитель
    sleep(1);
    system("ps");
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
/*-----------------------------------------------------------------------*/
/* Либо можно не игнорить сигнал и вызвать wait в обработчике сигнала */
void signal_handler(int signum)
{
  int status, res;
  res = wait(&status);
  if (res == -1)
    printf("wait завершился с ошибкой\n");
  printf("Процесс потомок завершился сигналом %d и статусом %x\n", signum, status);
}
