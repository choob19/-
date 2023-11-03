#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Создать новый файл с правом только на чтение
  (имя файла и права доступа ввести из командной строки),
  записать в него несколько строк, закрыть.
  Повторно открыть данный файл на чтение, прочитать из него информацию
  и вывести ее на экран, закрыть. Еще раз открыть этот же файл на чтение и запись,
  проверить результат системного вызова open. Объяснить полученные результаты.
*/
void Err_Handler(int fd);

int main(int argc, char const *argv[])
{
  int fd = 0, mode, res, bytes_read;
  char read_buf[64] = {0};
  /* creat == open(path, O_WRTONLY | O_CREAT | O_TRUNC, mode) */
  /* S_IRUSR - права на только на чтение для юзера (0400) */
  if (argc < 3) {
    printf("Укажите необходимое число аргументов!\n");
    printf("./[prog_name] [file_name] [mode]\n");
    return 0;
  }
  mode = strtol(argv[2], NULL, 8);
  if (mode < 0)
    Err_Handler(0);
  /* Создаем файл только на чтение */
  printf("Создание файла...\n");
  fd = creat(argv[1], mode);
  if (fd < 0)
    Err_Handler(0);
  /* Записываем несколько строк в файл */
  printf("Запись в файл нескольких строк...\n");
  for (int i = 0; i < 3; i++) {
    res = write(fd, argv[i], strlen(argv[i]));
    if (res < 0)
      Err_Handler(fd);
  }
  /* Закрываем файл */
  printf("Закрытие файла...\n");
  if (close(fd) < 0)
      Err_Handler(0);
  /* Повторно открываем файл */
  printf("Повторное открытие файла...\n");
  fd = open(argv[1], O_RDONLY, mode);
  if (fd < 0)
    Err_Handler(fd);
  /* Читаем инфу из файла */
  printf("Чтение инфы из файла...\n");
  bytes_read = read(fd, read_buf, sizeof(read_buf));
  if (bytes_read < 0)
    Err_Handler(fd);
  printf("Из файла прочитано %d байт. Данные: %s\n", bytes_read, read_buf);
  /* Очередное закрытие файла */
  printf("Очередное закрытие файла\n");
  if (close(fd) < 0)
    Err_Handler(0);
  /* Последнее открытие файла */
  printf("Попытка открытия файла с правами на чтение и запись...\n");
  fd = open(argv[1], O_RDWR, mode);
  printf("Результат open() = %d\n", fd);
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
  perror("Возникла ошибка:\t");
  if (fd)
    close(fd);
  exit(0);
}
