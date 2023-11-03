#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

/*
  ============================== Задание 10 ==============================
  Еще раз модифицировать эти программы так, чтобы использовалась только одна
  очередь сообщений и для запросов к серверу, и для всех ответов.
  Программа-клиент отправляет серверу сообщение и ждет ответа. В зависимости от
  содержимого сообщения мб получен разный ответ.
*/

/* Func Headers */
void Err_Handler(int line);
void signal_handler(int sig);
/* Global defs */
/* Структура сообщения с нужными полями */
typedef struct {
  long m_type;      //  Тип сообщения
  pid_t m_pid;      //  PID клиента, отправившего сообщение
  char m_data[64];  //  Данные
} Message;
/* Структура сообщения для работы с системным вызовом */
typedef struct {
  long rm_type;
  char rm_data[64 + sizeof(pid_t)];
} RawMessage;

typedef union {
  Message Msg_struct;
  RawMessage Rmsg_struct;
} msgUnion;

int main(int argc, char const *argv[])
{
  int msqid, res;
  pid_t pid = getpid();
  key_t key;
  msgUnion msg = {0}; //  Структуры сообщений
  /*
  if (argc != 2) {
    printf("Укажите нужное число аргументов!\n\t %s [message for server]\n", argv[0]);
    exit(0);
  }*/
  /* Получение ключа */
  key = ftok("10stask", 'q');
  if (key < 0) Err_Handler(__LINE__);
  /* Создание очереди сообщений */
  msqid = msgget(key, 0666);
  if (msqid < 0) Err_Handler(__LINE__);
  /* Инит структуры сообщения для отправки */
  msg.Msg_struct.m_pid = pid;
  msg.Msg_struct.m_type = 1;
  if (argc == 2)
    strncpy(msg.Msg_struct.m_data, argv[1], sizeof(msg.Msg_struct.m_data));
  else
    msg.Msg_struct.m_data[0] = '\0';
  /* Отправка сообщения серверу */
  printf("Отправка сообщения: %s\n", msg.Msg_struct.m_data);
  res = msgsnd(msqid, &msg.Rmsg_struct, sizeof(msg.Rmsg_struct), 0);
  if (res == -1) Err_Handler(__LINE__);
  /* Ожидание ответа от сервера */
  res = msgrcv(msqid, &msg.Rmsg_struct, sizeof(msg), pid, 0);
  if (res == -1) Err_Handler(__LINE__);
  printf("Ответ от сервера: %s\n", msg.Msg_struct.m_data);
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
void signal_handler(int sig)
{

}
