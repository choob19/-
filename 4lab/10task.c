#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Создать два параллельных процесса, разделяющие ввод с клавиатуры и вывод на экран.
  Что получится при исполнении этих процессов?
  (Процессы находятся в состоянии гонки и конкурируют за одинаковые файловые дескрипторы,
  из-за чего данные в потоке вывода будут искажены. Это зависит от размера буфера чтения,
  если писать меньше чем размер буфера - то операция записи и чтения будет атомарной. Но если
  нужно писать больше размера буфера, то будут искажения.
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res, status, pid;
  char buf[2];

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок   ---------------------------------------------
    pid = getpid();
    printf("Потомок запущен [%d]\n", pid);
    res = 1;
    while (res > 0) {
      res = read(STDIN_FILENO, buf, sizeof(buf));
      //printf("[%d] %c", pid, buf);
      write(1, buf, res);
    }
  } else {    //  Родитель  ---------------------------------------------
    pid = getpid();
    printf("Родитель запущен [%d]\n", pid);
    res = 1;
    while (res > 0) {
      res = read(STDIN_FILENO, buf, sizeof(buf));;
      write(1, buf, res);
    }
    res = wait(&status);
  printf("Потомок завершил работу статусом выхода %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))  //  Если 1 - значит потомок завершился по сигналу
      printf("Сигнал, завершивший потомка: %s(%d)\n", strsignal(WTERMSIG(status)), WTERMSIG(status));
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
