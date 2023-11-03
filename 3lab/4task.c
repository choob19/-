#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Создать длительный параллельный процесс (в порожденном процессе выполнять вложенные циклы,
  каждый на 100 или больше итераций, и выводить на экран номер итерации для внешнего цикла).
  Дождаться нормального завершения порожденного процесса. Посмотреть,
  какой статус будет передан в родительский процесс.
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res = 0, status = 0;

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок
    for (int i = 0; i <= 100; i++) {
      for (int j = 0; j < 0x00ffffff; j++) {

      }
      if (!(i % 10)) printf("Итерация [%d]\n", i);
    }
    exit(5);
  } else {    //  Родитель
    res = wait(&status);
    printf("Потомок завершил работу статусом %d\n", WEXITSTATUS(status));
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
