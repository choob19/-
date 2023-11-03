#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Написать программу, иллюстрирующую способ блокирования дополнительных сигналов
  на время работы обработчика сигнала. Что произойдет, если во время обработки
  некоторого сигнала в процесс поступит несколько однотипных заблокированных сигналов?
  (Количество пришедших однотипных сигналов не будет учитываться, потому что все приходящие
  сигналы заносяттся в маску ожидающих сигналов, которая может хранить не больше 1 значения
  каждого сигнала) (Запускать в фоне!)
*/

/* Func headers */
void Err_Handler(int line);
void signal_handler(int signum);
void one_type_signal(int signum);
/* Global vars */
static unsigned int sig_num[32] = {0};  // NSIG - кoл-во сигналов, но лучше захардкодить
static int SIGINT_flag = 0;

/* Для реализации этой задачи используется сигнальная маска процесса */
int main(int argc, char const *argv[])
{
  sigset_t block_mask;

  /* Установка одного обработчика для всех сигналов */
  for (int i = 1; i < 32; i++)
    if (signal(i, signal_handler) < 0) Err_Handler(__LINE__);

  /* Добавление в маску игнорируемого сигнала */
  if (sigaddset(&block_mask, 1) < 0) Err_Handler(__LINE__);
  /* Инит маски процесса */
  if (sigprocmask(SIG_SETMASK, &block_mask, NULL) < 0) Err_Handler(__LINE__);

  /* Ожидание сигнала, который вызовет завершение процесса (SIGINT) */
  while (!SIGINT_flag)
    continue;
  /* После этого цикла при разрешении всех сигналов выполнятся отложенные сигналы */
  /* Разрешение сигнала */
  if (sigemptyset(&block_mask) < 0) Err_Handler(__LINE__);
  if (sigprocmask(SIG_SETMASK, &block_mask, NULL) < 0) Err_Handler(__LINE__);

  printf("\n===== Полученные сигналы =====\n");
  for (int i = 1; i < 32; i++) {
    if (sig_num[i])
      printf("%s (%d): %d\n", strsignal(i), i, sig_num[i]);
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
void signal_handler(int signo)  //  Сигналы KILL(9) и SIGSTOP(19) перехватить нельзя!
{
  if (signo == SIGINT)
    SIGINT_flag = 1;
  else
    sig_num[signo]++;
  printf("Процесс получил сигнал %s(%d)\n", strsignal(signo), signo);
}
