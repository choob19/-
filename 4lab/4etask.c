#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*

*/

void Err_Handler(int line);
void arr(char buf[], int len);

int main(int argc, char const *argv[])
{
  char buf[32];
  int rfd, wfd, res;
  /* Считывание переменных окружения */
  rfd = strtol(getenv("READ_FD"), NULL, 10);
  wfd = strtol(getenv("WRITE_FD"), NULL, 10);
  //printf("rfd = %d, wfd = %d\n", rfd, wfd);
  while (1) {
    res = read(rfd, buf, sizeof(buf));
    printf("[П] Потомок принял сообщение из %lu байт: %s\n", strlen(buf), buf);
    arr(buf, strlen(buf));
    res = write(wfd, buf, sizeof(buf));
    printf("[П] Потомок отправил в ответ %lu байт\n", strlen(buf));
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
/*-----------------------------------------------------------------------*/
void arr(char buf[], int len)
{
  for (int i = 0; i < len; i++) {
    buf[i] = toupper(buf[i]);
  }
}
