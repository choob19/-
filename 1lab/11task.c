#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/*
  Написать программу, которая определяет, какой файл из перечисленных
  в командной строке имеет наибольшую длину. Вывести длину файла в байтах.
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res = 0;
  unsigned int max_size = 0, pointer_to_name = 1;
  struct stat f_info = {0};
  if (argc < 2) {
    printf("Укажите достаточное количество файлов!\n");
    return 0;
  }
  for (int i = 1; i < argc; i++) {
    res = stat(argv[i], &f_info);
    if (res < 0) {
      if (errno == 2) { //  не завершать с ошибкой, если файла нет
        printf("Файл [%s] не существует\n", argv[i]);
        continue;
      }
      else              //  для остальных ошибок вызывать обработчик
        Err_Handler(__LINE__);
    }
    if (max_size < f_info.st_size) {
      max_size = f_info.st_size;
      pointer_to_name = i;
    }
  }
  printf("Наибольший файл из списка: %s\nРазмер файла: %u байт\n", argv[pointer_to_name], max_size);

  return 0;
}
//--------------------------------------------------------------------------
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}
