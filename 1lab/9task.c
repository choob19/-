#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Изменить программу п. 8 таким образом, чтобы функция копирования
  использовала стандартные ввод и вывод, а вызывающая программа переназначала
  стандартные ввод и вывод на указанные в аргументах командной строки файлы
*/

/*
  Стандартные дескрипторы: 0 - ст. ввод; 1 - ст. вывод; 2 - ст. протокол.
  STDIN_FILENO - макрос дескриптора ст. ввода (0).
  STDOUT_FILENO - макрос дескриптора ст. вывода (1).
  STDERR_FILENO - макрос дескриптора вывода ошибки (2).
*/

void copy_file(int fd_src, int fd_dst);
void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  char src_fname[255] = {0}, dst_fname[255] = {0};
  int fd_src = 0, fd_dst = 0;
  int stdin_new, stdout_new, stdout_old;
  /* Проверка наличия аргументов и получение имен файлов */
  if (argc == 3) {
    strncpy(src_fname, argv[1], sizeof(src_fname));
    strncpy(dst_fname, argv[2], sizeof(src_fname));
  } else {
    printf("Введите имя файла источника: \n");
    fgets(src_fname, sizeof(src_fname), stdin);
    src_fname[strlen(src_fname)-1] = '\0';  //  удаление символа \n
    /* fgets() считывает также и символ \n, поэтому его надо убрать */
    printf("Введите имя файла приемника: \n");
    fgets(dst_fname, sizeof(src_fname), stdin);
    dst_fname[strlen(dst_fname)-1] = '\0';
  }
  printf("Копирование из [%s] в [%s]\n", src_fname, dst_fname);

  /* Открываем файлы. Файл источник с прaвами на чтение. Приемниk на W */
  fd_src = open(src_fname, O_RDONLY);
  if (fd_src < 0)
    Err_Handler(__LINE__);

  /* Eсли файл уже существует то не копируем  */
  fd_dst = open(dst_fname, O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (fd_dst < 0)
    Err_Handler(__LINE__);

  /* Сохранение старого значения дескриптора вывода */
  stdout_old = dup(STDOUT_FILENO);
  /* Переназначение потока вывода, закрытие ассоциации потока вывода 1 */
  if (close(STDOUT_FILENO) < 0)
    Err_Handler(__LINE__);
  if ((stdout_new = dup(fd_dst)) < 0)
    Err_Handler(__LINE__);
  /* Переназначение потока ввода, закрытие ассоциации потока ввода 0 */
  if (close(STDIN_FILENO) < 0)
    Err_Handler(__LINE__);
  if ((stdin_new = dup(fd_src)) < 0)
    Err_Handler(__LINE__);
  /* Передача функции новых дескрипторов */
  copy_file(STDIN_FILENO, STDOUT_FILENO);

  /* Возврат старого дескриптора потока вывода */
  if (close(STDOUT_FILENO) < 0)
    Err_Handler(__LINE__);
  stdout_new = dup2(stdout_old, STDOUT_FILENO);
  if (stdout_new < 0)
    Err_Handler(__LINE__);
  printf("Копирование завершено\n");
  close(fd_src);
  close(fd_dst);
  return 0;
}
//--------------------------------------------------------------------------
void copy_file(int fd_src, int fd_dst)
{
  char read_buf[1024];
  int bytes_read = 1024, res = 0;
  /* Считывание файла блоками по 1К и запись в новый файл */
  while (bytes_read == 1024) {
  /* Когда read() считает меньше байт, чем есть в блоке -- значит файл прочитан */
    bytes_read = read(fd_src, read_buf, sizeof(read_buf));
    if (bytes_read < 0)
      Err_Handler(__LINE__);
    res = write(fd_dst, read_buf, bytes_read);
    if (res < 0)
      Err_Handler(__LINE__);
  }
}
//--------------------------------------------------------------------------
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
