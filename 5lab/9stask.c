#include <ctype.h>
#include <errno.h>
#include <signal.h>
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
void arr(char buf[], int len);
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
/* Global variables */
int msqid_in, msqid_out;

int main(int argc, char const *argv[])
{
  int res;
  key_t key;
  Msg msg = {0};
  /* Смена диспозиции сигнала для корректного выхода с удалением очереди */
  if (signal(SIGINT, signal_handler) < 0) Err_Handler(__LINE__);
  /* Создание очереди для получения сообщений */
  key = ftok("6stask", 'a');  //  Получение ключа
  if (key < 0) Err_Handler(__LINE__);
  msqid_in = msgget(key, 0666 | IPC_CREAT);
  if (msqid_in == -1) Err_Handler(__LINE__);
  /* Создание очереди для отправки сообщений */
  key = ftok("6stask", 'b');
  if (key < 0) Err_Handler(__LINE__);
  msqid_out = msgget(key, 0666 | IPC_CREAT);
  if (msqid_out == -1) Err_Handler(__LINE__);
  /* Начало работы сервера. Ожидание сообщений, обработка, отправка ответа в другую очередь */
  while (1) {
    res = msgrcv(msqid_in, &msg.rmsg_struct, sizeof(msg), 0, 0);  // блокируемся в ожидании любых сообщений
    if (res == -1) Err_Handler(__LINE__);
    msg.msg_struct.m_type = msg.msg_struct.m_pid;
    if ((res = strlen(msg.msg_struct.m_data))) {  //  обработка сообщения ненулевой длины
      printf("Сервер получил сообщение от процесса %d\n", msg.msg_struct.m_pid);
      printf("\t%s\n", msg.msg_struct.m_data);
      arr(msg.msg_struct.m_data, res);
      res = msgsnd(msqid_out, &msg.rmsg_struct, sizeof(msg), 0);
      if (res == -1) Err_Handler(__LINE__);
    } else {  //  обработка сообщения нулевой длины
      char str[] = "Answer from server";
      printf("Сервер получил сообщение нулевой длины от процесса %d\n", msg.msg_struct.m_pid);
      strncpy(msg.msg_struct.m_data, str, sizeof(str));
      res = msgsnd(msqid_out, &msg.rmsg_struct, sizeof(msg), 0);
      if (res == -1) Err_Handler(__LINE__);
    }
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
void signal_handler(int sig)
{
  /* Удаление очереди сообщений и завершение процесса */
  if (msgctl(msqid_in, IPC_RMID, 0) == -1) Err_Handler(__LINE__);
  if (msgctl(msqid_out, IPC_RMID, 0) == -1) Err_Handler(__LINE__);
  printf("Сервер завершил работу\n");
  exit(0);
}
/*-----------------------------------------------------------------------*/
void arr(char buf[], int len)
{
  for (int i = 0; i < len; i++)
    buf[i] = toupper(buf[i]);
}
