#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Написать программу, которая добавляет новые строки в среду процесса.
  ARG_MAX - макс. число переменных окружения и параметров (<limits.h>)
*/

/* Func prototypes */
void Err_Handler(int line);
void Env_List(void);
void Get_String(char str[], int len);
/* Global variables */
/* Рекомендуется производить передачу окружения через эту глобальную переменную */
extern char **environ; // указатели на перем окружения

/* В *envp[] хранится только копия окружения, ориг есть в **environ */
int main(int argc, char *argv[], char *envp[])
{
  int res = 0;
  char my_env[] = "MY_ENV", value[16] = {0};
  printf("Список переменных окружения:\n");
  /* Вывод всех имеющихся переменных окружения */
  Env_List();

  printf("Очистка окружения...\n");
  res = clearenv();
  if (res < 0)
    Err_Handler(__LINE__);
  Env_List();

  printf("Добавление новой переменной окружения...\n");
  printf("Введите значение переменной:\n");
  Get_String(value, sizeof(value));
  res = setenv(my_env, value, 1);
  if (res < 0)
    Err_Handler(__LINE__);
  printf("Переменной %s присвоено значение %s\n", my_env, value);
  printf("Список переменных окружения:\n");
  Env_List();

  printf("Изменение переменной %s на значение VAL\n", my_env);
  res = setenv(my_env, "VAL", 1);
  if (res < 0)
    Err_Handler(__LINE__);
  printf("Получение значения переменной %s:\n", my_env);
  printf("%s = %s\n", my_env, getenv(my_env));

  //Env_List();
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
void Env_List()
{
  int cnt = 0;
  if (environ == NULL) {
    printf("Окружение пусто!\n");
    return;
  }
  while (environ[cnt]) {
    printf("Переменная [%d]: %.32s\n", cnt, environ[cnt]);
    cnt++;
  }
}
/*-----------------------------------------------------------------------*/
void Get_String(char str[], int len)
{
  fgets(str, len, stdin);
  str[strlen(str)-1] = '\0';
}
