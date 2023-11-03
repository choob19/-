#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Изменить реакцию процесса на некоторый сигнал при помощи signal.
  В функции обработчике переустановить реакцию на сигнал на значение по умолчанию.
  Запустить программу и объяснить наблюдаемый результат.
  (Сначала обработчик сигнала обработает установленный сигнал, но потом установит
  обратно диспозицию по дефолту)
*/

void Err_Handler(int line);
void signal_handler(int signum);

int main(int argc, char const *argv[])
{
  printf("Ожидание сигнала...\n");
  if (signal(SIGINT, signal_handler) == SIG_ERR)
    Err_Handler(__LINE__);
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
  /* Текущее действие при получении сигнала называется диспозицией сигнала */
  /* (при получении надежных сигналов восстанавливать диспозицию не обязательно) */
  if (signal(signum, SIG_DFL) == SIG_ERR) //  Восстановление диспозиции обязательно, но работает и без нее
    Err_Handler(__LINE__);
  else
    printf("[%d]Ouch! (signum = %d)\n", cnt++, signum);
}
