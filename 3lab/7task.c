#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Повторить выполнение предыдущих пунктов задания, используя в порожденном процессе
  вместо вложенных циклов системный вызов pause. Что изменится?
  (Процесс потомок теперь не сможет завершиться, пока не дождется сигнала)
  Как завершить процесс в случае выполнения с pause п. 4?
  (Отправить ему сигнал командой kill вручную)
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res = 0, status = 0;

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок
    pause();
  } else {    //  Родитель
    if (!kill(res, 0)) {  //  Посылка тестового сигнала для проверки, в res лежит PID потомка
      res = kill(res, 2);
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
