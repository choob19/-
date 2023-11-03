#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Добавить в один из процессов использование слабой блокировки. Что произойдет?
  (Зависит от момента, в который будет установлена блокировка.
  Если залочить внутри цикла считывания - анные будут считываться корректно. В остальных случаях 
  данные будут считываться некорректно)
  fcntl - блочит отдельные участки файла (можно и весь)
  Чтобы заблокировать файл на чтение, он должен быть открыт на чтение и т.д. для записи
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res, status, fd;
  char buf[64];
  struct flock lock;  //  Залочим весь файл для записи на время чтения
  if (argc != 2) {
    printf("Укажите имя файла! Пример: %s [filename]\n", argv[0]);
    exit(0);
  }

  lock.l_start = 0;          //  Смещение блокируемой записи отн. точки отсчета
  lock.l_whence = SEEK_SET;  //  Точка отсчета смещения записи в файле
  lock.l_len = 0;            //  Длина блокируемой записи (0 - до конца файла)

  res = fork();
  if (res < 0) Err_Handler(__LINE__);
  if (!res) {   //  Потомок   -------------------------------------------
    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) Err_Handler(__LINE__);
    printf("Потомок открыл файл (%s)[%d] на чтение\n", argv[1], fd);

    for (int i = 0; i < 100000; i++) {
      lock.l_type = F_RDLCK; //  Тип блокировки: F_RDLCK, F_WRLCK, F_UNLCK
      if (fcntl(fd, F_SETLKW, &lock)  < 0) Err_Handler(__LINE__);//  Залочили файл на запись
      if (read(fd, buf, sizeof(buf)) == -1) Err_Handler(__LINE__);
      lock.l_type = F_UNLCK;
      fcntl(fd, F_SETLK, &lock);//  Разлочили файл
      if (lseek(fd, SEEK_SET, 0) < 0) Err_Handler(__LINE__);
    }

    printf("buf = %d\n", atoi(buf));
    exit(0);
  } else {      //  Родитель  -------------------------------------------
    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) Err_Handler(__LINE__);
    printf("Родитель открыл файл (%s)[%d] на запись\n", argv[1], fd);
    for (int i = 0; i <= 100000; i++) {

      snprintf(buf, sizeof(buf), "%d      ", i);
      if (write(fd, buf, sizeof(buf)) == -1) Err_Handler(__LINE__);

      if (lseek(fd, SEEK_SET, 0) < 0) Err_Handler(__LINE__);
      res = i;
    }
    printf("Записано число %d\n", res);
    res = wait(&status);
    printf("\nПотомок завершил работу статусом выхода %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))  //  Если 1 - значит потомок завершился по сигналу
      printf("Сигнал, завершивший потомка: %s(%d)\n", strsignal(WTERMSIG(status)), WTERMSIG(status));
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
