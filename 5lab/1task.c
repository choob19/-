#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/*
  ============================== Задание 1 ==============================
  Создать очередь сообщений или получить доступ к уже существующей очереди.
  Вывести на экран статистическую информацию об очереди сообщений.
  Поместить несколько сообщений различных типов в очередь.

  Существует два способа взаимодействия с IPC - это POSIX IPC и System5 IPC.
  Интерфейс POSIX IPC больше соответствует традиционной для UNIX файловой
  модели. IPC-объекты идентифицируются с помощью имен и управляются вызовами
  open, close и unlink, которые похожи на аналогичные системные вызовы для работы
  с файлами.
  Интерфейс, предоставляемый POSIX IPC, во многих отношениях превосходит
  IPC-интерфейс System V, хотя и уступает ему в плане портируемости на другие системы.
  Главное отличие этих двух стандартов - POSIX IPC потокобезопасный, в отличие от System5 IPC,
  что является критичным при задаче синхронизации процессов.
*/

/* Func headers */
void Err_Handler(int line);
/*  */
typedef struct {
  long type;
  char data[64];
} Message;

int main(int argc, char const *argv[])
{
  Message s_msg;  //  Структура, определяющая тип сообщений и их содержимое
  struct msqid_ds q_stat;//  Структура с инфой о запрошенной очереди
  int msqid, len, res;
  time_t rawtime; //  время в формате epoch
  struct tm time; //  время в формате структуры
  char timebuf[64];
  struct passwd *user;
  struct group *grp;

  if (argc != 2) {
    printf("Укажите правильное число аргументов!\n\t%s [msqid]\n", argv[0]);
    return 0;
  }
  msqid = strtol(argv[1], NULL, 10);
  if (msqid == -1) Err_Handler(__LINE__);
  /* Заполняем структуру q_stat */
  res = msgctl(msqid, IPC_STAT, &q_stat);
  if (res == -1) Err_Handler(__LINE__);
  /* Печатаем всю инфу об очереди */
  rawtime = q_stat.msg_stime;   //	получение времени в формате epoch
  time = *localtime(&rawtime);  //	преобразование времени в структуру
  /* Форматирование времени и преобразование его в строку */
	strftime(timebuf, sizeof(timebuf), "%d-%m-%Y %H:%M:%S %Z", &time);
  printf("Время последнего msgsnd: %s\n", timebuf);
  rawtime = q_stat.msg_rtime;
  time = *localtime(&rawtime);
  strftime(timebuf, sizeof(timebuf), "%d-%m-%Y %H:%M:%S %Z", &time);
  printf("Время последнего msgrcv: %s\n", timebuf);
  rawtime = q_stat.msg_ctime;
  time = *localtime(&rawtime);
  strftime(timebuf, sizeof(timebuf), "%d-%m-%Y %H:%M:%S %Z", &time);
  printf("Время последнего изменения: %s\n", timebuf);
  printf("Текущее количество байт очереди: %lu\n", q_stat.__msg_cbytes);
  printf("Текущее количество сообщений в очереди: %lu\n", q_stat.msg_qnum);
  printf("Максимальное количество байт для очереди: %lu\n", q_stat.msg_qbytes);
  printf("Максимальное количество сообщений для очереди: %lu\n", q_stat.msg_qbytes / sizeof(s_msg));
  printf("PID последнего msgsnd: %d\n", q_stat.msg_lspid);
  printf("PID последнего msgrcv: %d\n", q_stat.msg_lrpid);
  printf("Ключ очереди: %x\n", q_stat.msg_perm.__key);
  user = getpwuid(q_stat.msg_perm.uid);
  printf("Владелец: %s [%d]\n", user->pw_name, user->pw_uid);
  grp = getgrgid(q_stat.msg_perm.gid);
  printf("Группа владелец: %s [%d]\n", grp->gr_name, grp->gr_gid);
  user = getpwuid(q_stat.msg_perm.cuid);
  printf("Создатель: %s [%d]\n", user->pw_name, user->pw_uid);
  grp = getgrgid(q_stat.msg_perm.cgid);
  printf("Группа создателя: %s [%d]\n", grp->gr_name, grp->gr_gid);
  /* Заполнение очереди сообщениями */
  printf("Отправка 5 сообщений...\n");
  for (int i = 5; i > 0; i--) {
    /* Инит структуры сообщения */
    len = snprintf(s_msg.data, sizeof(s_msg.data), "Сообщение [%d]\n", i+1);
    s_msg.type = rand() % 10;
    /* Отправка сообщения в очередь */
    if (msgsnd(msqid, (void *) &s_msg, len, 0)) Err_Handler(__LINE__);
  }
  printf("Сообщения отправлены\n");

  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
