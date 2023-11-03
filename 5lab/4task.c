#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

/*
  ============================== Задание 4 ==============================
  Модифицировать программу п. 2, позволив ей принимать первые
  несколько байтов сообщений произвольных размеров.

  Создание очереди:             ipcmk -Q -p 0666
  Просмотр доступных очередей:  ipcs -q
  Удаление очереди:             ipcrm -q [msqid]
  Флаг MSG_NOERROR позволяет считать из очереди сообщение, даже если размер буфера
  приема меньше самого сообщения. При этом лишние данные будут отброшены, а само
  сообщение удалится из очереди, что вызовет потерю данных.
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
  long m_type;            //  Тип сообщения
  Message msg = {0};      //  Структура сообщения
  struct msqid_ds m_info; //  Структура с инфой об очереди
  int res, msqid, m_size;         //  Идентификатор сообщения
  if (argc != 4) {
    printf("Укажите правильное число аргументов!\n\t%s [msqid] [m_type] [m_size]\n", argv[0]);
    exit(0);
  }
  /* Считывание идентификатора очереди */
  msqid = strtol(argv[1], NULL, 10);
  if (msqid == -1) Err_Handler(__LINE__);
  /* Считывание типа сообщения */
  m_type = strtol(argv[2], NULL, 10);
  if (m_type == -1) Err_Handler(__LINE__);
  /* Считывание размера сообщения */
  m_size = strtol(argv[3], NULL, 10);
  if (m_size < 0) {
    printf("Размер должен быть больше или равен 0!\n");
    return 0;
  }
  printf("\nСчитывание сообщения типа %ld из очереди id:%d\n", m_type, msqid);
  /* Проверка что очередь не пуста */
  if (msgctl(msqid, IPC_STAT, &m_info) == -1) Err_Handler(__LINE__);
  if (!m_info.msg_qnum) {
    printf("Очередь пуста!\n");
    return 0;
  }
  /* Считывание сообщения */
  res = msgrcv(msqid, &msg, m_size, m_type, MSG_NOERROR | IPC_NOWAIT); //  неблокирующий вариант считывания
  if (res == -1) Err_Handler(__LINE__);
  printf("\nСчитано сообщение\n");
  printf("Тип:\t%ld\nТекст:\t%s", msg.type, msg.data);
  printf("\nОсталось сообщений: %lu\n", m_info.msg_qnum - 1);
  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
