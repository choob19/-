#include <ctype.h>
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
  Модифицировать программу п. 2 таким образом, чтобы процессы реализовали двунаправленную связь (диалог).
  (Для фулл дуплекса создаются два неименованных канала)
  Потомок принимает поток  байтов от родителя, модифицирует принятые данные и отправляет в ответ
*/

void Err_Handler(int line);
void arr(char buf[], int len);

int main(int argc, char const *argv[])
{
  int res, fd[2], fdd[2], status; //  [0] - дескриптор чтения, [1] - дескриптор записи
  char buf[32];

  if (pipe(fd) < 0) Err_Handler(__LINE__);
  if (pipe(fdd) < 0) Err_Handler(__LINE__);

  res = fork();
  if (res < 0) Err_Handler(__LINE__);
  if (!res) {   //  Потомок   -------------------------------------------
    close(fd[0]); //  Пишет в fd
    close(fdd[1]);//  Читает из fdd
    while (1) {
      res = read(fdd[0], buf, sizeof(buf));
      printf("[П] Потомок принял сообщение из %lu байт: %s\n", strlen(buf), buf);
      arr(buf, strlen(buf));
      res = write(fd[1], buf, sizeof(buf));
      printf("[П] Потомок отправил в ответ %lu байт\n", strlen(buf));
    }
    close(fdd[0]);
    close(fd[1]);
    exit(0);
  } else {      //  Родитель  -------------------------------------------
    close(fd[1]); //  Читает из fd
    close(fdd[0]);//  Пишет в fdd
    while (1) {
      printf("[Р] Введите сообщение для потомка:\n");
      fgets(buf, sizeof(buf), stdin);
      res = write(fdd[1], buf, sizeof(buf));
      printf("[Р] Отправка %lu байт потомку\n", strlen(buf));
      //sleep(1);
      res = read(fd[0], buf, sizeof(buf));
      printf("[Р] Ответ от потомка (%lu байт): %s\n", strlen(buf), buf);
    }

    close(fdd[1]);
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
/*-----------------------------------------------------------------------*/
void arr(char buf[], int len)
{
  for (int i = 0; i < len; i++) {
    buf[i] = toupper(buf[i]);
  }
}
