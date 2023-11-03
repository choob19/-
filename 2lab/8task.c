#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Открыть файл (достаточно большого размера), доступный для чтения.
  Имя файла передается из командной строки.
  После открытия файла создать параллельный процесс с помощью только fork.
  В обоих процессах создать свои файлы для записи, читать информацию
  из общего файла и копировать ее в собственные выходные файлы (не на экран).

  Вывести на экран содержимое полученных файлов из порожденного процесса
  по окончании записи в файл и из родительского процесса,
  дождавшись завершения порожденного процесса. Посмотреть, что изменится,
  если читаемую процессами информацию сразу выводить на экран.
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int fd, res, parent_fd, child_fd, bytes_read = 1;
  char read_buf;
  if (argc != 2) {
    printf("Укажите необходимое число аргументов!\n");
    printf("%s [file_name]\n", argv[0]);
    return 0;
  }
  //res = strtol("666", NULL, 8);

  /* Открытие основного файла для чтения */
  fd = open(argv[1], O_RDONLY, 0666);
  if (fd < 0) Err_Handler(__LINE__);
  printf("Файл открыт. fd = %d Создание процесса...\n", fd);

  /* Создание процесса */
  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок ------------------------------------------

    /* Открытие дочернего файла для записи */
    child_fd = open("child_file", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (child_fd < 0) Err_Handler(__LINE__);

    /* Копирование из основного файла в дочерний */
    while (bytes_read > 0) {
      bytes_read = read(fd, &read_buf, sizeof(read_buf));
      if (bytes_read < 0) Err_Handler(__LINE__);
      res = write(child_fd, &read_buf, sizeof(read_buf));
      if (res < 0) Err_Handler(__LINE__);
      //putchar(read_buf);
    }
    close(child_fd);  // Закрытие дочернего файла
    exit(0);          // Завершение дочернего процесса
  } else {    //  Родитель  -----------------------------------------

    parent_fd = open("parent_file", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (parent_fd < 0) Err_Handler(__LINE__);

    /* Копирование из основного файла в файл родителя */
    while (bytes_read > 0) {
      bytes_read = read(fd, &read_buf, sizeof(read_buf));
      if (bytes_read < 0) Err_Handler(__LINE__);
      res = write(parent_fd, &read_buf, sizeof(read_buf));
      if (res < 0) Err_Handler(__LINE__);
      //putchar(read_buf);
    }
    close(parent_fd); //  Закрытие файла родителя
  }
  printf("Завершение работы. Закрытие файлов.\n");
  close(fd);          //  Закрытие основного файла
  return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
