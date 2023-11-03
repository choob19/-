#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Модифицировать программу, включив в нее setpgrp в порожденный процесс до того,
  как он будет выполнять pause. Повлияет ли на порожденный процесс нажатие
  клавиши прерывания в тот момент, когда родительский процесс "спит"?
  (Не повлияет)
  Будет ли показан порожденный процесс в выводе команды ps?
  (Будет)
  Вызов setpgrp() явл устаревшим! В наст время используется только для переносимости.
  Hачиная с glibc 2.19, BSD-функция setpgrp() была удалена  из  <unistd.h>;  вызовы
  должны быть заменены на вызов setpgid()
*/

void Err_Handler(int line);

extern char **environ;

int main(int argc, char const *argv[])
{
  int res = 0;

  res = fork();
  if (res == -1) Err_Handler(__LINE__);

  if (!res) { //  Потомок
    printf("Процесс потомок. PID: %d, PPID: %d, GRPID: %d\n", getpid(), getppid(), getpgid(getpid()));
    if (setpgrp() < 0) Err_Handler(__LINE__);
    printf("Новая группа потомка. PID: %d, PPID: %d, GRPID: %d\n", getpid(), getppid(), getpgid(getpid()));
    if (pause() < 0) Err_Handler(__LINE__);
    exit(2);
  } else {    //  Родитель
    printf("Процесс родитель. PID: %d, PPID: %d, GRPID: %d\n", getpid(), getppid(), getpgid(getpid()));
    sleep(3);
    printf("Родитель ждет сигнала...\n");
    if (pause() < 0) Err_Handler(__LINE__);
    //exit(0);
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
