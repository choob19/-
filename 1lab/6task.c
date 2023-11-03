#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Модифицировать программу п. 4 для создания разреженного файла.
  Реальный размер файла смотреть утилитой du
  Использование: du -h filename
*/

void Err_Handler(int fd, int line);

int main(int argc, char const *argv[])
{
  int fd = 0, mode = 0, res = 0, bytes_write = 0;

  /* creat == open(path, O_WRTONLY | O_CREAT | O_TRUNC, mode) */
  /* Создаем файл только на чтение */
  /* S_IRUSR - права на только на чтение для юзера (0400) */
  if (argc < 3) {
    printf("Укажите необходимое число аргументов!\n");
    printf("./[prog_name] [file_name] [mode]\n");
    return 0;
  }
  mode = strtol(argv[2], NULL, 8);
  if (mode < 0)
    Err_Handler(0, __LINE__);

  /* Создание файла (если файл уже сущ. то вернет ошибку) */
  printf("Создание файла...\n");
  fd = open(argv[1], O_RDWR | O_CREAT | O_EXCL, mode);
  if (fd < 0)
    Err_Handler(0, __LINE__);

  /* Запись нескольких строк в файл */
  printf("Запись в файл нескольких строк...\n");
  for (int i = 0; i < 2; i++) {
    res = write(fd, argv[i], strlen(argv[i]));
    bytes_write += res;
    if (res < 0)
      Err_Handler(fd, __LINE__);
  }
  printf("Всего записано в файл: %d байт\n", bytes_write);

  /* Записываем строку через 128 Kбайт */
  res = lseek(fd, 1024 * 128, SEEK_END);
  if (res < 0)
    Err_Handler(fd, __LINE__);
  res = write(fd, argv[2], strlen(argv[1]));
  if (res < 0)
    Err_Handler(fd, __LINE__);

  /* Закрываем файл */
  printf("Закрытие файла...\n");
  if (close(fd) < 0)
      Err_Handler(0, __LINE__);
  return 0;
}
//--------------------------------------------------------------------------
void Err_Handler(int fd, int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  if (fd)
    close(fd);
  exit(0);
}
