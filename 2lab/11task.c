#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Выполнить из программы на Си какую-либо команду Shell (cp или ls):
  с помощью вызовов fork-exec, с помощью библиотечной функции system.
  Необходимые для команды данные передать через аргументы командной строки
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
  int res = 0, status = 0, str_size = 0;
  char *arg_string;
  if (argc < 2) {
    printf("Укажите необходимое число аргументов!\n");
    printf("%s [prog_name] [args]\n", argv[0]);
    return 0;
  }

  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок ---------------------------------------
    /* Считаем длину строки аргументов */
    for (int i = 1; i < argc; i++) {
      str_size += strlen(argv[i]) + 1;  //  Длина строки + пробел
    }
    printf("Длина строки: %d\n", str_size);
    /* Выделяем память под строку для system() */
    arg_string = (char *) calloc(str_size, sizeof(char));
    if (!arg_string) Err_Handler(__LINE__);
    /* Формирование строки для system */
    for (int i = 1; i < argc; i++) {
      strncat(arg_string, argv[i], strlen(argv[i]));
      strncat(arg_string, " ", 1);
    }
    /* Вызов system() */
    if (system(arg_string) < 0) Err_Handler(__LINE__);
    free(arg_string);
    exit(5);
  } else {    //  Родитель --------------------------------------
    res = wait(&status);
    if (res == -1) Err_Handler(__LINE__);
    printf("Процесс завершён. Статус: %d\n", WEXITSTATUS(status));
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
