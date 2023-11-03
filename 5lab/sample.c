#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

/*
  Написать программу, позволяющую читать сообщения из очереди и выводить их на экран.
  Идентификатор очереди и тип сообщения передается через аргументы командной строки.
*/

/* Func headers */
void Err_Handler(int line);
/* Queue struct */
typedef struct {
  long type;
  char data[16];
} Message;

int main(int argc, char const *argv[])
{
  int msqid, res;
  Message msg;
  msqid = msgget((key_t) 0xdeadc0de, 0666 | IPC_CREAT);
  if (msqid == -1) Err_Handler(__LINE__);
  for (int i = 1; i <= 10; i++) {
    msg.type = i;
    snprintf(msg.data, sizeof(msg.data), "Message num [%d]", i);
    res = msgsnd(msqid, &msg, sizeof(msg.data), 0);
    if (res == -1) Err_Handler(__LINE__);
  }
  printf("Очередь создана!\n");
  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
