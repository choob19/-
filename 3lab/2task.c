#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Модифицировать программу п.1 так, чтобы вместо signal использовать sigaction.
*/

void Err_Handler(int line);
void signal_handler(int signum);

/* Вариант надежной функции signal() */
void (*new_signal (int signum, void (*signal_handler)(int)))(int);

int main(int argc, char const *argv[])
{
  struct sigaction act, old_act;
  act.sa_handler = signal_handler;

  /* Инит набора сигналов пустым значением */
  sigemptyset(&act.sa_mask);
  act.sa_flags |= SA_RESETHAND; //  Флаг сброса диспозиции в дефолт после 1-го сигнала
  printf("Ожидание сигнала...\n");

  if (sigaction(SIGINT, &act, &old_act) < 0) Err_Handler(__LINE__);

  while(1) {
    pause();
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
  static unsigned int cnt = 0;

  //new_signal(SIGINT, signal_handler);

  printf("[%d]Ouch! (signum = %d)\n", cnt++, signum);
}
/*-----------------------------------------------------------------------*/
void (*new_signal (int signum, void (*signal_handler)(int)))(int)
{
  struct sigaction act, old_act;
  act.sa_handler = signal_handler;
  /* Инит набора сигналов пустым значением */
  sigemptyset(&act.sa_mask);
  if (signum != SIGALRM)
    act.sa_flags |= SA_RESTART;
  /* Установка диспозиции */
  if (sigaction(signum, &act, &old_act) < 0) Err_Handler(__LINE__);
  return (old_act.sa_handler);
}
