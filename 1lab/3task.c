#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Повторно выполнить программу п. 2 с теми же исходными данными;
  проверить результаты выполнения каждого системного вызова.
  Объяснить полученные результаты
*/

void Err_Handler(int fd);

int main(int argc, char const *argv[])
{
  int fd = 0, mode, res, bytes_read;
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
  printf("Создание файла...\n");
  fd = creat(argv[1], mode);
  printf("Результат вызова creat(): %d\n", fd);
  if (fd < 0)
    Err_Handler(0);
  /* Записываем несколько строк в файл */
  printf("Запись в файл нескольких строк...\n");
  for (int i = 0; i < 3; i++) {
    res = write(fd, argv[i], strlen(argv[i]));
    printf("Результат вызова write() при записи %d строки: %d\n", i+1, res);
    if (res < 0)
      Err_Handler(fd);
  }
  /* Закрываем файл */
  printf("Закрытие файла...\n");
  res = close(fd);
  printf("Результат вызова close(): %d\n", res);
  if (res < 0)
      Err_Handler(0);
  /* Повторно открываем файл */
  printf("Повторное открытие файла...\n");
  fd = open(argv[1], O_RDONLY, mode);
  printf("Результат вызова open(): %d\n", fd);
  if (fd < 0)
    Err_Handler(fd);
  /* Читаем инфу из файла */
  printf("Чтение инфы из файла...\n");
  bytes_read = read(fd, read_buf, sizeof(read_buf));
  printf("Результат вызова read(): %d\n", bytes_read);
  if (bytes_read < 0)
    Err_Handler(fd);
  printf("Из файла прочитано %d байт. Данные: %s\n", bytes_read, read_buf);
  /* Очередное закрытие файла */
  printf("Очередное закрытие файла\n");
  res = close(fd);
  printf("Результат вызова close(): %d\n", res);
  if (res < 0)
    Err_Handler(0);
  /* Последнее открытие файла */
  printf("Попытка открытия файла с правами на чтение и запись...\n");
  fd = open(argv[1], O_RDWR, mode);
  printf("Результат вызова open(): %d\n", fd);
  if (fd < 0)
    Err_Handler(0);

  close(fd);
  return 0;
}
/*
  При попытке открыть файл с правами на чтение и запись выдаст ошибку прав доступа
  потому что файл создавался только с правами на запись. Если создать файл с
  правами на чтение и запись то и ошибки не будет. При этом на права накладывается
  маска umask = 0x0022, которая запрещает права на запись у грyппы и остальных.
*/

void Err_Handler(int fd)
{
  printf("Код ошибки:\t%d\n", errno);
  fprintf(stderr, "Возникла ошибка!\nТип: %s\n", strerror(errno));
  if (fd)
    close(fd);
  exit(0);
}
