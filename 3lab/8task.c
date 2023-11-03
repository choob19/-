#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Включить в порожденный процесс системный вызов signal,
  переопределяющий стандартную реакцию на сигнал (для внешнего цикла
  установить в функции максимальное значение параметра цикла). Что изменится?
  (Прога завершится не зайдя в обработчик сигнала??? При этом, если послать сигнал отдельно,
  то обработчик срабатывает)
*/

void Err_Handler(int line);
void signal_handler(int signum);

int main(int argc, char const *argv[])
{
  int res = 0, status = 0;

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок
    /* Изменение диспозиции сигнала */
    if (signal(SIGINT, signal_handler) == SIG_ERR) Err_Handler(__LINE__);

    for (int i = 0; i < 100; i++) {
      for (int j = 0; j < 0x0000ffff; j++) {

      }
      if (!(i % 100)) printf("Шаг %d\n", i);
    }
    while (1) { }
  } else {    //  Родитель
    //sleep(1); // Если подождать, то сигнал будет перехватываться??????
    if (!kill(res, 0)) {  //  Посылка тестового сигнала для проверки, в res лежит PID потомка
      res = kill(res, SIGINT);
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
/*-----------------------------------------------------------------------*/
void signal_handler(int signum)
{
  printf("Процесс потомок получил сигнал %d\n", signum);
}
