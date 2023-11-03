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
  ============================== Задание 10 ==============================
  Еще раз модифицировать эти программы так, чтобы использовалась только одна
  очередь сообщений и для запросов к серверу, и для всех ответов.
  Программа-сервер ждет сообщений от клиентов и отправляет ответ. Если сообщение
  ненулевой длины, то обрабатывает строку сообщения и отправляет ответ, иначе отправляет
  в ответ стандартную строку. (Запускать в фоне)
*/

/* Func Headers */
void Err_Handler(int line);
void signal_handler(int sig);
void arr(char buf[], int len);
/* Global defs */
/* Структура сообщения с нужными полями */
typedef struct {
  long m_type;      //  Тип сообщения
  pid_t m_pid;      //  PID клиента, отправившего сообщение
  char m_data[64];  //  Данные
} Message;
/* Структура сообщения для работы с системными вызовaми */
typedef struct {
  long rm_type;
  char rm_data[64 + sizeof(pid_t)];
} RawMessage;

typedef union {
  Message Msg_struct;
  RawMessage Rmsg_struct;
} msgUnion;
/* Global variables */
int msqid;

int main(int argc, char const *argv[])
{
  int res;
  key_t key;
  msgUnion msg = {0}; //  Структуры сообщений
  /* Смена диспозиции сигнала для выхода с удалением очереди */
  if (signal(SIGINT, signal_handler) < 0) Err_Handler(__LINE__);
  /* Получение ключа */
  key = ftok("10stask", 'q');
  if (key < 0) Err_Handler(__LINE__);
  /* Создание очереди сообщений */
  msqid = msgget(key, 0666 | IPC_CREAT);
  if (msqid < 0) Err_Handler(__LINE__);

  /* Начало работы сервера. Ожидание сообщений, обработка и отправка ответа */
  while (1) {
    res = msgrcv(msqid, &msg.Rmsg_struct, sizeof(msg), 1, 0); //  Блокируемся в ожидании сообщений
    if (res == -1) Err_Handler(__LINE__);
    msg.Rmsg_struct.rm_type = msg.Msg_struct.m_pid; //  подготовка к ответу, отправка с типом PID клиента
    if ((res = strlen(msg.Msg_struct.m_data)) != 0) {  //  обработка сообщения ненулевой длины
      printf("Сервер получил сообщение от процесса %d\n", msg.Msg_struct.m_pid);
      printf("\t%s\n", msg.Msg_struct.m_data);
      arr(msg.Msg_struct.m_data, res);
      res = msgsnd(msqid, &msg.Rmsg_struct, sizeof(msg.Rmsg_struct), 0);
      if (res == -1) Err_Handler(__LINE__);
    } else {  //  обработка сообщения нулевой длины
      char str[] = "Answer from server";
      printf("Сервер получил сообщение нулевой длины от процесса %d\n", msg.Msg_struct.m_pid);
      strncpy(msg.Msg_struct.m_data, str, sizeof(str));
      res = msgsnd(msqid, &msg.Rmsg_struct, sizeof(msg.Rmsg_struct), 0);
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
  if (msgctl(msqid, IPC_RMID, 0) == -1) Err_Handler(__LINE__);
  exit(0);
}
/*-----------------------------------------------------------------------*/
void arr(char buf[], int len)
{
  for (int i = 0; i < len; i++)
    buf[i] = toupper(buf[i]);
}
