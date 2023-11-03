#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
  Повторить п. 2, создавая параллельные процессы с помощью связки fork - exec.
  Как передать дескрипторы канала в порожденный процесс?
  (Через окружение)
*/

void Err_Handler(int line);
void arr(char buf[], int len);

int main(int argc, char const *argv[])
{
  int res, fd[2], fdd[2], status; //  [0] - дескриптор чтения, [1] - дескриптор записи
  char buf[32], env[4];

  if (pipe(fd) < 0) Err_Handler(__LINE__);
  if (pipe(fdd) < 0) Err_Handler(__LINE__);

  res = fork();
  if (res < 0) Err_Handler(__LINE__);
  if (!res) {   //  Потомок   -------------------------------------------
    close(fd[0]); //  Пишет в fd
    close(fdd[1]);//  Читает из fdd
    /* Создание своих переменных окружения */
    snprintf(env, sizeof(env), "%d", fd[1]);
    setenv("WRITE_FD", env, 1);
    snprintf(env, sizeof(env), "%d", fdd[0]);
    setenv("READ_FD", env, 1);
    /* Старт программы */
    execl("4etask", "4etask", NULL);
    Err_Handler(__LINE__);
    exit(0);
  } else {      //  Родитель  -------------------------------------------
    close(fd[1]); //  Читает из fd
    close(fdd[0]);//  Пишет в fdd
    while (1) {
      printf("[Р] Введите сообщение для потомка:\n");
      fgets(buf, sizeof(buf), stdin);
      res = write(fdd[1], buf, sizeof(buf));
      printf("[Р] Отправка %lu байт потомку\n", strlen(buf));
      //sleep(1);
      res = read(fd[0], buf, sizeof(buf));
      printf("[Р] Ответ от потомка (%lu байт): %s\n", strlen(buf), buf);
    }

    close(fdd[1]);
    close(fd[0]);
    res = wait(&status);
    printf("Потомок завершил работу статусом выхода %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))  //  Если 1 - значит потомок завершился по сигналу
      printf("Сигнал, завершивший потомка: %s(%d)\n", strsignal(WTERMSIG(status)), WTERMSIG(status));
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
