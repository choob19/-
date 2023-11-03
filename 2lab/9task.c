#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Выполнить п. 8 при условии, что общий файл для чтения открывается в каждом из порожденных процессов.
*/

void file_copy(int src_fd, int dst_fd);

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int fd, res, child_fd, parent_fd;
  if (argc != 2) {
    printf("Укажите необходимое число аргументов!\n");
    printf("%s [file_name]\n", argv[0]);
    return 0;
  }

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок ------------------------------------------
    /* Открытие основного файла на чтение */
    fd = open(argv[1], O_RDONLY, 0666);
    if (fd < 0) Err_Handler(__LINE__);
    /* Открытие дочернего файла для записи */
    child_fd = open("child_file", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (child_fd < 0) Err_Handler(__LINE__);

    /* Копирование из основного файла в дочерний */
    file_copy(fd, child_fd);
    close(child_fd);  // Закрытие дочернего файла
    exit(0);          // Завершение дочернего процесса
  } else {    //  Родитель  -----------------------------------------
    /* Открытие основного файла на чтение */
    fd = open(argv[1], O_RDONLY, 0666);
    if (fd < 0) Err_Handler(__LINE__);
    /* Открытие родительского файла на запись */
    parent_fd = open("parent_file", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (parent_fd < 0) Err_Handler(__LINE__);

    /* Копирование из основного файла в файл родителя */
    file_copy(fd, parent_fd);
    close(parent_fd); //  Закрытие файла родителя
  }
  printf("Завершение программы, закрытие файлов.\n");
  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
/*-----------------------------------------------------------------------*/
void file_copy(int src_fd, int dst_fd)
{
  char buf;
  int bytes_read = 1, res;
  while (bytes_read > 0) {
    bytes_read = read(src_fd, &buf, 1);
    if (bytes_read < 0) Err_Handler(__LINE__);
    res = write(dst_fd, &buf, 1);
    if (res < 0) Err_Handler(__LINE__);
  }
}
