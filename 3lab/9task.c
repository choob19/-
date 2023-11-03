#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Включить в порожденный процесс вместо системного вызова kill системный вызов
  alarm с перехватом и без перехвата сигнала. Что изменится?
  (Если сигнал перехватить, то потомок завершится со статусом без сигнала. Если
  сигнал не перехватывать, то потомок завершится по сигналу с соотв. статусом)
*/

void Err_Handler(int line);
void signal_handler(int signum);

int main(int argc, char const *argv[])
{
  int res = 0, status = 0;

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок ------------------------------------------------
    /* Изменение диспозиции сигнала (раскоментить для проверки 2 случая) */
    //if (signal(SIGALRM, signal_handler) == SIG_ERR) Err_Handler(__LINE__);
    alarm(1);
    pause();
  } else {    //  Родитель  ----------------------------------------------

    res = wait(&status);  // status <= 255
    printf("Потомок завершил работу статусом выхода %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status)) {  //  Если 1 - значит потомок завершился по сигналу
      printf("Сигнал, завершивший потомка: %s(%d)\n", strsignal(WTERMSIG(status)), WTERMSIG(status));
    }

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
void signal_handler(int signum)
{
  printf("Процесс потомок получил сигнал %d\n", signum);
}
