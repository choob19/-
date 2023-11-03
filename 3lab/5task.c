#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Из родительского процесса послать в порожденный процесс сигнал (SIGUSR1).
  Посмотреть, какой статус будет передан в родительский процесс в этом случае.
  Сколько итераций выполнится в порожденном процессе?
  (Нисколько, родитель пошлёт сигнал SIGUSR1, действие по умолчанию которого -
  завершение процесса. Потомок завершится по сигналу, что отразится на его статусе завершения)
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res = 0, status = 0, i;

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок
    for (i = 0; i <= 100; i++) {
      for (int j = 0; j < 0x00ffffff; j++) {

      }
      if (!(i % 10)) printf("Итерация [%d]\n", i);
    }
    exit(i);
  } else {    //  Родитель
    //sleep(1);
    if (!kill(res, 0)) {  //  Посылка тестового сигнала для проверки, в res лежит PID потомка
      res = kill(res, SIGUSR1);
      if (res == -1) Err_Handler(__LINE__);
    }
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
