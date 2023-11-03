#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
  Написать программу, которая выполняет некоторый системный вызов,
  например open, и затем выводит возвращаемую ошибку.
  Использовать в программе разные способы вывода сообщения об ошибке:
  вывод значения errno, вывод соответствующего сообщения из системного
  массива сообщений об ошибках sys_errlist[], вызов библиотечной функции perror.
*/

int main(int argc, char const *argv[])
{
  int fd = 0;
  /* Откроем несуществующий файл */
  fd = open("xyz", O_RDONLY);
  if (fd < 0) {
    printf("Номер ошибки: %d\n", errno);
    perror("[Использование perror()]\tОшибка открытия файла");
/*
* Согласно man perror:
* В  настоящее  время  использование sys_errlist[] не рекомендуется;
* вместо этого используйте strerror(3)
*/
    printf("[Использование strerror()]\tОшибка открытия файла: %s\n", strerror(errno));
    //fprintf(stderr, "Текст ошибки в stderr: %s\n", strerror(errno));
    //fprintf(stdout, "Текст ошибки в stdout: %s\n", strerror(errno));
  }
  else {
    close(fd);
  }
  return 0;
}
