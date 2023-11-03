#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Используя программу п. 2, создать новый файл с правами на чтение и запись;
  выполнить любые операции с файлом (чтение и/или запись),
  используя разные способы позиционирования. Объяснить полученные результаты.
*/

void Err_Handler(int fd);

int main(int argc, char const *argv[])
{
  int fd = 0, mode = 0, res = 0, bytes_read = 0, bytes_write = 0;
  char read_buf[64] = {0};
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
    Err_Handler(0);

  /* Создание файла */
  printf("Создание файла...\n");
  fd = creat(argv[1], mode);
  if (fd < 0)
    Err_Handler(0);

  /* Записываем несколько строк в файл */
  printf("Запись в файл нескольких строк...\n");
  for (int i = 0; i < 3; i++) {
    res = write(fd, argv[i], strlen(argv[i]));
    bytes_write += res;
    if (res < 0)
      Err_Handler(fd);
  }
  printf("Всего записано в файл: %d\n", bytes_write);
  /* Закрываем файл (потому что он открыт только на запись) */
  printf("Закрытие файла...\n");
  if (close(fd) < 0)
      Err_Handler(0);

  /* Теперь надо открыть файл с разрешением на чтение для возможности чтения */
  fd = open(argv[1], O_RDONLY, mode);
  if (fd < 0)
    Err_Handler(fd);

  /* Начало работы с файловым указателем */
  /* Установка указателя в начало */
  res = lseek(fd, 0, SEEK_SET);
  if (res < 0)
    Err_Handler(fd);
  printf("Файловый указатель установлен в начало: %d\n", res);
  printf("Чтение первых 3 байт по указателю...\n");
  bytes_read = read(fd, read_buf, 3);
  if (bytes_read < 0)
    Err_Handler(fd);
  printf("Результат чтения: %s\n", read_buf);

  /* Установка указателя в середину */
  res = lseek(fd, bytes_write / 2, SEEK_SET);
  if (res < 0)
    Err_Handler(fd);
  printf("Чтение 3 байт по указателю из середины файла...\n");
  bytes_read = read(fd, read_buf, 3);
  if (bytes_read < 0)
    Err_Handler(fd);
  printf("Результат чтения: %s\n", read_buf);

  /* Установка указателя в конец */
  res = lseek(fd, -3, SEEK_END);
  if (res == -1)
    Err_Handler(fd);
  printf("Чтение последних 3 байт по указателю...\n");
  bytes_read = read(fd, read_buf, 3);
  if (bytes_read < 0)
    Err_Handler(fd);
  printf("Результат чтения: %s\n", read_buf);

  /* Закрываем файл */
  printf("Закрытие файла...\n");
  if (close(fd) < 0)
      Err_Handler(0);
  return 0;
}
//--------------------------------------------------------------------------
void Err_Handler(int fd)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n", strerror(errno));
  if (fd)
    close(fd);
  exit(0);
}
