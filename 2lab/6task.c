#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Изменить программу п. 3 так, чтобы оба процесса выполняли системный вызов
  pause после вывода идентификаторов. Запустить программу в фоновом режиме
  и посмотрите (с помощью ps), какие процессы существуют для данного пользователя.
  Выполнить kill -9 pid для родительского процесса. Что произойдет с порожденным?
  (Порожденный процесс останется открытым в ожидании сигнала.)
  После очистки текущего сеанса, вновь выполнить программу, но уже в интерактивном режиме.
  Нажать на терминале клавишу CTRL-C. Что произойдет с родительским и порожденным процессами?
  (Системный вызов pause() дождётся сигнала и процессы завершатся)
  Можно ли получить такой же результат, если нажать клавишу прерывания после того,
  как родительский процесс завершится? (Можно, если послать дочернему процессу сигнал
  при помощи kill или htop)
*/

void Err_Handler(int line);

extern char **environ;

int main(int argc, char const *argv[])
{
  int res = 0;

  res = fork();
  if (res == -1)
    Err_Handler(__LINE__);
  if (!res) { //  Потомок
    printf("Процесс потомок. PID: %d, PPID: %d\n", getpid(), getppid());
    if (pause() < 0)
      Err_Handler(__LINE__);
    exit(1);
  } else {    //  Родитель
    printf("Процесс родитель. PID: %d, PPID: %d\n", getpid(), getppid());
    if (pause() < 0)
      Err_Handler(__LINE__);
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
