#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Создать (с помощью связки fork - exec) параллельный процесс.
  Имя исполняемого файла для exec передается с помощью аргумента командной строки.
  Передать в порожденный процесс некоторую информацию через список параметров
  (список аргументов в функции main). Каждый процесс (и родительский, и порожденный)
  должен вывести на экран список переданных ему аргументов и свое окружение.
*/

/* Func prototypes */
void Err_Handler(int line);
/* Global variables */
extern char **environ;

/* Можно юзать *envp[], но в ней лежит копия **environ */
int main(int argc, char *const argv[])
{
  int res, exit_status;
  char *my_env[] = {"HOME=/", "PATH=", "USER=test", NULL};
  if (argc < 2) {
    printf("Укажите необходимое число аргументов!\n");
    printf("%s [prog_name] [args]\n", argv[0]);
    return 0;
  }
  /* Создание процесса */
  res = fork();
  if (res == -1) Err_Handler(__LINE__);
  if (!res) { //  Потомок ---------------------------------------
    printf("Дочерний процесс\nАргументы:\n");
    for (int i = 0; i < argc; i++)
      printf("arg[%d]: %s\n", i, argv[i]);
    printf("Переменные окружения\n");
    for (int i = 0; i < 16; i++)  //environ[i]
      printf("Child_Env[%d]: %.20s\n", i, environ[i]);
    //execvp(argv[1], argv+1);
    execve(argv[1], argv+1, my_env);
    perror("execve()");
  } else {    //  Родитель --------------------------------------
    printf("Родительский процесс\nАргументы:\n");
    for (int i = 0; i < argc; i++)
      printf("arg[%d]: %s\n", i, argv[i]);
    printf("Переменные окружения\n");
    for (int i = 0; i < 16; i++)  //environ[i]
      printf("Parent_Env[%d]: %.20s\n", i, environ[i]);
    res = wait(&exit_status);
    if (res == -1) Err_Handler(__LINE__);
    printf("Процесс завершён. Статус: %d\n", WEXITSTATUS(exit_status));
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
