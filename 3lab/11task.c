#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Написать программу, позволяющую использовать sigaction для реализации
  примера синхронизации процессов. Выполнить эту программу и объяснить ее поведение.
  Использовать sigsuspend и sigprocmask.
*/

/* Func headers */
void Err_Handler(int line);
void signal_handler(int sig);
void info_handler(int sig, siginfo_t *siginfo, void *ucontext);
/* Global vars */


int main(int argc, char const *argv[])
{
  int res = 0;
  struct sigaction sigInitStruct;
  sigset_t procmask, susmask;

  sigInitStruct.sa_sigaction = info_handler;
  //sigInitStruct.sa_handler = signal_handler;
  sigInitStruct.sa_flags = SA_SIGINFO;
  sigfillset(&sigInitStruct.sa_mask); // игнор всех сигналов обработчиком
  sigfillset(&procmask);  //  игнор всех сигналов процессом
  if (sigprocmask(SIG_SETMASK, &procmask, NULL) < 0) Err_Handler(__LINE__);

  sigfillset(&susmask);
  sigdelset(&susmask, SIGUSR1);
  sigdelset(&susmask, SIGQUIT);  // игнор всех сигналов, кроме SIGUSR1 и SIGQUIT
  /* Смена диспозиции на SIGQUIT, но его получится поймать только во время sigsuspend() */
  //if (sigaction(SIGQUIT, &sigInitStruct, NULL) < 0) Err_Handler(__LINE__);

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок   ----------------------------------------------
    int pid = getppid();
    if (sigaction(SIGUSR1, &sigInitStruct, NULL) < 0) Err_Handler(__LINE__);
    while (1) {
      sigsuspend(&susmask);
      kill(pid, SIGUSR1);
    }
  } else {    //  Родитель  ----------------------------------------------
    int pid = res;
    if (sigaction(SIGUSR1, &sigInitStruct, NULL) < 0) Err_Handler(__LINE__);
    sleep(1); //  Ждем смены диспозиции потомка
    while (1) {
      
      kill(pid, SIGUSR1);
      sigsuspend(&susmask);
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
void signal_handler(int sig)  //  Сигналы KILL(9) и SIGSTOP(19) перехватить нельзя!
{
  static unsigned int cnt = 0;
  sleep(1);
  printf("Получен сигнал %d [%d]\n", sig, cnt++);
}
/*-----------------------------------------------------------------------*/
void info_handler(int sig, siginfo_t *siginfo, void *ucontext)
{
  sleep(1);
  printf("Получен сигнал %d от процесса %d\n", siginfo->si_signo, siginfo->si_pid);
}