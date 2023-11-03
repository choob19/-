#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

/*
  Программа добавления сообщения в очередь.
  Создание очереди:             ipcmk -Q -p 0666
  Просмотр доступных очередей:  ipcs -q
  Удаление очереди:             ipcrm -q [msqid]
*/

/* Func headers */
void Err_Handler(int line);
/* Queue struct */
typedef struct {
  long type;    //  Первые 4 байта структуры должны быть типом сообщения
  char data[64];
} Message;

int main(int argc, char const *argv[])
{
  Message msg = {0};    //  Структура сообщения
  int res, msqid; //  Идентификатор сообщения
  if (argc != 4) {
    printf("Укажите правильное число аргументов!\n\t%s [msqid] [m_type] [data]\n", argv[0]);
    exit(0);
  }
  /* Считывание идентификатора очереди */
  msqid = strtol(argv[1], NULL, 10);
  if (msqid == -1) Err_Handler(__LINE__);
  /* Считывание типа сообщения */
  msg.type = strtol(argv[2], NULL, 10);
  if (msg.type == -1) Err_Handler(__LINE__);
  strncpy(msg.data, argv[3], sizeof(msg.data));
  /* Отправка сообщения */
  res = msgsnd(msqid, &msg, sizeof(msg), 0);
  if (res == -1) Err_Handler(__LINE__);
  printf("В очередь записано сообщение\n");
  printf("Тип:\t%ld\nДанные\t%s\n", msg.type, msg.data);
  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
