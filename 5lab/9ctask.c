#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

/*
  ============================== Задание 9 ==============================
  Модифицировать программы п. 6 так, чтобы использовались две очереди:
  одна для запросов к серверу и одна для всех ответов от сервера к любому клиенту.
  Теперь обе очереди будут создаваться при старте сервера и удаляться при завершении
  работы сервера.
*/

/* Func Headers */
void Err_Handler(int line);
void signal_handler(int sig);
/* Global defs */
/* Структура для обмена сообщениями с клиентами */
typedef struct {
  long m_type;    //  тип сообщения
  pid_t m_pid;    //  идентификатор процесса, который прислал сообщение
  char m_data[64];//  данные для передачи
} Message;
/* Структура сообщения для работы с системными вызовaми */
typedef struct {
  long rm_type;
  char rm_data[64 + sizeof(pid_t)];
} RawMessage;
/* Объединение для удобства передачи данных */
typedef union {
  Message msg_struct;
  RawMessage rmsg_struct;
} Msg;

int main(int argc, char const *argv[])
{
  int msqid_out, res, msqid_in;
  pid_t pid = getpid();
  key_t key;
  Msg msg = {0}; //  Структуры сообщений
  /*
  if (argc != 2) {
    printf("Укажите нужное число аргументов!\n\t %s [message for server]\n", argv[0]);
    exit(0);
  }*/
  /* Инит очереди для отправки сообщений серверу */
  key = ftok("6stask", 'a');  //  Получение ключа
  if (key < 0) Err_Handler(__LINE__);
  msqid_out = msgget(key, 0666);
  if (msqid_out == -1) Err_Handler(__LINE__);
  /* Инит очереди для получения сообщений от сервера */
  key = ftok("6stask", 'b');
  if (key < 0) Err_Handler(__LINE__);
  msqid_in = msgget(key, 0666);
  if (msqid_in == -1) Err_Handler(__LINE__);
  /* Инит структуры сообщения для отправки */
  msg.msg_struct.m_pid = pid;
  msg.msg_struct.m_type = 1;
  if (argc == 2)
    strncpy(msg.msg_struct.m_data, argv[1], sizeof(msg.msg_struct.m_data));
  else
    msg.msg_struct.m_data[0] = '\0';
  /* Отправка сообщения серверу */
  printf("Отправка сообщения: %s\n", msg.msg_struct.m_data);
  res = msgsnd(msqid_out, &msg.rmsg_struct, sizeof(msg.rmsg_struct), 0);
  if (res == -1) Err_Handler(__LINE__);
  /* Ожидание ответа от сервера */
  res = msgrcv(msqid_in, &msg.rmsg_struct, sizeof(msg.rmsg_struct), pid, 0);
  if (res == -1) Err_Handler(__LINE__);
  printf("Ответ от сервера: %s\n", msg.msg_struct.m_data);
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
