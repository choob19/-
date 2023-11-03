#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  ============================== Задание 11 ==============================
  Написать программу, иллюстрирующую возможность синхронизации процессов при
  помощи очередей сообщений.
  В качестве примера используется прога 10 из 4 лабы, но с сихронизацией
*/
/* Func Headers */
void Err_Handler(int line);
void handler(int sig);
/* Global defs */
typedef struct {
  long type;
  char data[1];
} Msg;
/* Global vars */
int msqid;

int main(int argc, char const *argv[])
{
  int res, status;
  char buf[2];
  Msg msg = {0};

  msqid = msgget(IPC_PRIVATE, 0666);
  if (msqid == -1) Err_Handler(__LINE__);
  msg.type = 1;
  res = msgsnd(msqid, &msg, sizeof(msg), 0);
  if (res == -1) Err_Handler(__LINE__);

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок   ---------------------------------------------
    printf("Потомок запущен [%d]\n", getpid());
    res = 1;
    while (res > 0) {
      if (msgrcv(msqid, &msg, sizeof(msg), 2, 0) == -1) Err_Handler(__LINE__);

      res = read(STDIN_FILENO, buf, sizeof(buf));
      write(1, buf, res);

      msg.type = 1;
      if (msgsnd(msqid, &msg, sizeof(msg), 0) == -1) Err_Handler(__LINE__);
    }
  } else {    //  Родитель  ---------------------------------------------
    printf("Родитель запущен [%d]\n", getpid());
    if (signal(2, handler) < 0) Err_Handler(__LINE__);
    res = 1;
    while (res > 0) {
      if (msgrcv(msqid, &msg, sizeof(msg), 1, 2) == -1) Err_Handler(__LINE__);

      res = read(STDIN_FILENO, buf, sizeof(buf));
      usleep(200000);
      write(1, buf, res);

      msg.type = 2;
      if (msgsnd(msqid, &msg, sizeof(msg), 0) == -1) Err_Handler(__LINE__);
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
/*-----------------------------------------------------------------------*/
void handler(int sig)
{
  if (msgctl(msqid, IPC_RMID, 0) == -1) Err_Handler(__LINE__);
  exit(0);
}
