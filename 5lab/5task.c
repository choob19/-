#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

/*
  ============================== Задание 5 ==============================
  Написать программу, позволяющую удалять очереди сообщений по идентификатору.
  Обработать возможные ошибочные ситуации.

*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int msqid;

  if (argc != 2) {
    printf("Укажите правильное число аргументов!\n\t%s [msqid]\n", argv[0]);
    exit(0);
  }
  /* Считывание идентификатора очереди */
  msqid = strtol(argv[1], NULL, 10);
  if (msqid == -1) Err_Handler(__LINE__);
  /* Удаление очереди */
  if (msgctl(msqid, IPC_RMID, 0) < 0) Err_Handler(__LINE__);
  printf("Очередь %d удалена\n", msqid);

  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
