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
  Модифицировать программу п. 1 таким образом, чтобы родительский процесс
  читал из канала в цикле, пока не встретится конец файла (Ctrl+D) (порожденный
  процесс может записывать в канал информацию, читаемую им из стандартного
  входного потока).
  Kак избежать бесконечного ожидания родительским процессом чтения из пустого канала?
  (Использовать неблокирующее чтение, при ините пайпа юзать флаг O_NONBLOCK)
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res, fd[2], status; //  [0] - дескриптор чтения, [1] - дескриптор записи
  char buf;
  if (pipe2(fd, 0) < 0) Err_Handler(__LINE__);  //  O_NONBLOCK

  res = fork();
  if (res < 0) Err_Handler(__LINE__);
  if (!res) {   //  Потомок   -------------------------------------------
    close(fd[0]);
    while (read(STDIN_FILENO, &buf, 1) > 0) {
      write(fd[1], &buf, 1);
    }
    close(fd[1]);
    exit(0);
  } else {      //  Родитель  -------------------------------------------
    close(fd[1]);
    while (read(fd[0], &buf, 1) > 0) {
      write(STDOUT_FILENO, &buf, 1);
    }
    close(fd[0]);
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
