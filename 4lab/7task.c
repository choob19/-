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
  Создать два параллельных процесса, в каждом из которых осуществляется работа
  с одним и тем же файлом. Каждый процесс открывает файл (имя файла передается
  через список аргументов командной строки). Один процесс пишет в файл, другой
  - читает из него. Что произойдет без включения блокировок?
  (Процессы будут постоянно прерывать друг друга при записи и чтении в файл, поэтому
  некоторые данные в файл могут не записаться)
*/

void Err_Handler(int line);
void arr(char buf[], int len);

int main(int argc, char const *argv[])
{
  int res, status, fd;
  char buf[64];

  if (argc != 2) {
    printf("Укажите имя файла! Пример: %s [filename]\n", argv[0]);
    exit(0);
  }
  res = fork();
  if (res < 0) Err_Handler(__LINE__);
  if (!res) {   //  Потомок   -------------------------------------------
    fd = open(argv[1], O_RDONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) Err_Handler(__LINE__);
    printf("Потомок открыл файл (%s)[%d] на чтение\n", argv[1], fd);
    for (int i = 0; i < 100000; i++) {
      if (read(fd, buf, sizeof(buf)) == -1) Err_Handler(__LINE__);
      if (lseek(fd, SEEK_SET, 0) < 0) Err_Handler(__LINE__);
    }
    printf("buf = %d\n", atoi(buf));
    exit(0);
  } else {      //  Родитель  -------------------------------------------
    fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) Err_Handler(__LINE__);
    printf("Родитель открыл файл (%s)[%d] на запись\n", argv[1], fd);
    for (int i = 0; i <= 100000; i++) {
      snprintf(buf, sizeof(buf), "%d      ", i);
      if (write(fd, buf, sizeof(buf)) == -1) Err_Handler(__LINE__);
      if (lseek(fd, SEEK_SET, 0) < 0) Err_Handler(__LINE__);
      res = i;
    }
    printf("Записано число %d\n", res);
    res = wait(&status);
    printf("\nПотомок завершил работу статусом выхода %d\n", WEXITSTATUS(status));
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
