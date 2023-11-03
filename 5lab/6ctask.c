#include <errno.h>
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
  ============================== Задание 7 ==============================
  Осуществить при помощи программ п. 6. обмен сообщениями между несколькими пользователями,
  при условии, что каждому пользователю предназначены сообщения любого или определенного
  типа (по договоренности). Будут ли примеры с очередями сообщений "приводить себя в порядок"?
  Что произойдет, если прервать процесс-сервер с помощью клавиши CTRL-C?
  ============================== Задание 8 ==============================
  Модифицировать программы п. 6 так, чтобы они завершались корректно и в случае прерывания по CTRL-C.
  Программа-клиент отправляет серверу сообщение и ждет ответа. В зависимости от
  содержимого сообщения мб получен разный ответ.
*/

/* Func Headers */
void Err_Handler(int line);
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
  /* Получение ключа */
  key = ftok("6stask", 'q');
  if (key == -1) Err_Handler(__LINE__);
  /* Получение дескриптора очереди сообщений сервера */
  msqid_out = msgget(key, 0666);
  if (msqid_out == -1) Err_Handler(__LINE__);
  /* Получение дескриптора очереди сообщений клиента */
  /* Хотя тут и есть флаг IPC_PRIVATE, но дескриптор очереди все равно можно передать другому неродственному процессу */
  msqid_in = msgget(IPC_PRIVATE, 0666);
  if (msqid_out == -1) Err_Handler(__LINE__);
  /* Инит структуры сообщения для отправки */
  msg.msg_struct.m_pid = pid;
  msg.msg_struct.m_type = 1;
  msg.msg_struct.m_msqid = msqid_in;
  if (argc == 2)
    strncpy(msg.msg_struct.m_data, argv[1], sizeof(msg.msg_struct.m_data));
  else
    msg.msg_struct.m_data[0] = '\0';
  /* Отправка сообщения серверу */
  printf("Отправка сообщения: %s\n", msg.msg_struct.m_data);
  res = msgsnd(msqid_out, &msg.rmsg_struct, sizeof(msg.rmsg_struct), 0);
  if (res == -1) Err_Handler(__LINE__);
  /* Ожидание ответа от сервера */
  res = msgrcv(msqid_in, &msg.rmsg_struct, sizeof(msg.rmsg_struct), 0, 0);
  if (res == -1) Err_Handler(__LINE__);
  printf("Ответ от сервера: %s\n", msg.msg_struct.m_data);
  /* Удаление временной очереди сообщений клиента */
  if (msgctl(msqid_in, IPC_RMID, 0) == -1) Err_Handler(__LINE__);
  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
