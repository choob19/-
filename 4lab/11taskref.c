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
  Повторить выполнение п. 10, включив в процессы для синхронизации работы с
  терминалом использование режима слабой блокировки.
  Как изменится поведение процессов?
  Благодаря блокировкам файлы синхронизируются и искажений не будет

  Блокировок чтения мб сколько угодно, блокировка записи мб только у одного процесса
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res, status, pid;
  char buf[2];
  struct flock lock;
  lock.l_start = 0;
  lock.l_whence = SEEK_SET;
  lock.l_len = 0;

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок   ---------------------------------------------
    pid = getpid();
    printf("Потомок запущен [%d]\n", pid);
    res = 1;
    while (res > 0) {
      lock.l_type = F_WRLCK;
      if (fcntl(0, F_SETLKW, &lock) == -1) Err_Handler(__LINE__);
      res = read(0, buf, sizeof(buf));
      //printf("[%d] %s\n", pid, buf);
      write(1, buf, res);
      lock.l_type = F_UNLCK;
      if (fcntl(0, F_SETLK, &lock) == -1) Err_Handler(__LINE__);
    }
  } else {    //  Родитель  ---------------------------------------------
    pid = getpid();
    printf("Родитель запущен [%d]\n", pid);
    res = 1;
    while (res > 0) {
      lock.l_type = F_WRLCK;
      if (fcntl(0, F_SETLKW, &lock) == -1) Err_Handler(__LINE__);
      res = read(0, buf, sizeof(buf));
      usleep(100000);
      write(1, buf, res);
      lock.l_type = F_UNLCK;
      if (fcntl(0, F_SETLK, &lock) == -1) Err_Handler(__LINE__);
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
