#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Создать параллельный процесс, используя только fork. Организовать обмен
  информацией между процессами через каналы. Предусмотреть однонаправленную
  и двунаправленную связь. При создании двунаправленной связи исключить возможность
  появления «смертельных объятий». (Для избежания дедлока сначала данные пишутся, а потом
  читаются из канала. Для эмуляции дедлока можно поменять местами чтение и запись данных в канал)
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int mode, res, parent_fd[2], child_fd[2]; //  [0] - дескриптор чтения, [1] - дескриптор записи
  char msg[] = "Test message string"; //  данные для отправки в канал
  char read_buf[64] = {0};

  /* Если mode != 0 - режим двунаправленной передачи с использованием доп. канала */
  if (argc < 2) mode = 0;
  else mode = strtol(argv[1], NULL, 10);
  if (mode) {
    res = pipe(child_fd);
    if (res < 0) Err_Handler(__LINE__);
  }

  if (mode) printf("Двунаправленный режим работы\n");
  else printf("Однонаправленный режим работы\n");

  /* Создание неименованного канала */
  res = pipe(parent_fd);
  if (res < 0) Err_Handler(__LINE__);

  /* Создание процесса (для однонаправленной связи) */
  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок   ----------------------------------------------
    close(parent_fd[0]);  //  немедленно закрыли канал для чтения
    sleep(1);
    if (mode) { //  двунаправленный режим (запись в parent_fd, чтение из child_fd)
      close(child_fd[1]); //  закрыли канал для записи

      printf("Потомок пишет в канал...\n");
      res = write(parent_fd[1], msg, sizeof(msg));
      if (res < 0) Err_Handler(__LINE__);
      printf("Потомок читает из канала...\n");
      res = read(child_fd[0], read_buf, sizeof(read_buf));
      if (res < 0) Err_Handler(__LINE__);
      printf("Потомок прочитал сообщение: %s\n", read_buf);
    } else {    //  однонаправленный режим
      res = write(parent_fd[1], msg, sizeof(msg));  //  если записываемые данные превышают PIPE_BUF, вызов блокируется пока все не запишется
      if (res < 0)  Err_Handler(__LINE__);
      printf("Запись сообщения в канал от потомка %d\n", getpid());
    }
    exit(0);
  } else {    //  Родитель  ----------------------------------------------
    close(parent_fd[1]);  //  немедленно закрыли канал для записи
    if (mode) { //  двунаправленный режим (запись в child_fd, чтение из parent_fd)
      close(child_fd[0]); //  закрыли канал для чтения

      printf("Родитель пишет в канал...\n");
      res = write(child_fd[1], msg, sizeof(msg));
      if (res < 0) Err_Handler(__LINE__);
      printf("Родитель читает из канала...\n");
      res = read(parent_fd[0], read_buf, sizeof(read_buf));
      if (res < 0) Err_Handler(__LINE__);
      printf("Родитель прочитал сообщение: %s\n", read_buf);
    } else {    //  однонаправленный режим
      printf("Родитель считывает из канала...\n");
      res = read(parent_fd[0], read_buf, sizeof(read_buf));   // если канал пуст, то блокируемся в ожидании данных
      if (res < 0) Err_Handler(__LINE__);
      printf("Родитель считал сообщение [%d байт] из канала:\n\t%s\n", res, read_buf);
    }

  }
  /* Создание процесса для двунаправленной связи */

  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
