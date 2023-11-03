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
  ============================== Задание 6 ==============================
  Написать программы для работы с очередями сообщений в соответствии с моделью
  клиент-сервер: каждый процесс использует собственную очередь сообщений.
  Процесс-сервер читает запросы из своей очереди сообщений и посылает ответ
  процессам-клиентам в их очереди. Процессы-клиенты читают ответ и выводят его
  в выходной поток. Процессы-клиенты должны передавать процессу-серверу информацию
  о своих очередях сообщений (куда записывать ответ).
  Программа-сервер ждет сообщений от клиентов и отправляет ответ в их очередь,
  дескриптор которой они передают серверу вместе с сообщениями. Если сообщение
  ненулевой длины, то обрабатывает строку сообщения и отправляет ответ, иначе отправляет
  в ответ стандартную строку. (Запускать в фоне)
  ============================== Задание 7 ==============================
  Осуществить при помощи программ п. 6. обмен сообщениями между несколькими пользователями,
  при условии, что каждому пользователю предназначены сообщения любого или определенного
  типа (по договоренности). Будут ли примеры с очередями сообщений "приводить себя в порядок"?
  Что произойдет, если прервать процесс-сервер с помощью клавиши CTRL-C?
  Если прервать работу сервера, то очередь сообщений не удалится, при этом с ней можно
  будет продолжать работать.
  ============================== Задание 8 ==============================
  Модифицировать программы п. 6 так, чтобы они завершались корректно и в случае прерывания по CTRL-C.
  (Для корректного завершения с удалением очереди изменить диспозицию у сигнала SIGINT(2))
*/

/* Func Headers */
void Err_Handler(int line);
void signal_handler(int sig);
void arr(char buf[], int len);
/* Global defs */
/* Структура для обмена сообщениями с клиентами */
typedef struct {
  long m_type;    //  тип сообщения
  int m_msqid;    //  идентификатор очереди
  pid_t m_pid;    //  идентификатор процесса, который прислал сообщение
  char m_data[64];//  данные для передачи
} Message;
/* Структура сообщения для работы с системными вызовaми */
typedef struct {
  long rm_type;
  char rm_data[64 + sizeof(int) + sizeof(pid_t)];
} RawMessage;
/* Объединение для удобства передачи данных */
typedef union {
  Message msg_struct;
  RawMessage rmsg_struct;
} Msg;
/* Global variables */
int msqid_in;

int main(int argc, char const *argv[])
{
  int res, msqid_out;
  key_t key;
  Msg msg = {0};
  /* Смена диспозиции сигнала для корректного выхода с удалением очереди */
  if (signal(SIGINT, signal_handler) < 0) Err_Handler(__LINE__);
  /* Получение ключа */
  key = ftok("6stask", 'q');
  if (key < 0) Err_Handler(__LINE__);
  /* Создание очереди сообщений */
  msqid_in = msgget(key, 0666 | IPC_CREAT); //  0444 потому что из этой очереди сервер только считывает сообщения
  if (msqid_in == -1) Err_Handler(__LINE__);
  /* Начало работы сервера. Ожидание сообщений, обработка, отправка ответа в другую очередь */
  while (1) {
    res = msgrcv(msqid_in, &msg.rmsg_struct, sizeof(msg), 0, 0);  // блокируемся в ожидании любых сообщений
    if (res == -1) Err_Handler(__LINE__);
    msqid_out = msg.msg_struct.m_msqid; // получили идентификатор очереди для отправки ответа
    if ((res = strlen(msg.msg_struct.m_data))) {  //  обработка сообщения ненулевой длины
      printf("Сервер получил сообщение от процесса %d\n", msg.msg_struct.m_pid);
      printf("\t%s\n", msg.msg_struct.m_data);
      arr(msg.msg_struct.m_data, res);
      res = msgsnd(msqid_out, &msg.rmsg_struct, sizeof(msg), 0);
      if (res == -1) Err_Handler(__LINE__);
    } else {  //  обработка сообщения нулевой длины
      const char str[] = "Answer from server";
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
  printf("Сервер завершил работу\n");
  exit(0);
}
/*-----------------------------------------------------------------------*/
void arr(char buf[], int len)
{
  for (int i = 0; i < len; i++)
    buf[i] = toupper(buf[i]);
}
