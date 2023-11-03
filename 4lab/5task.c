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
	С помощью системных вызовов pipe и dup реализовать конвейер: who | wc -l.
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
	int fd[2], res;

	/* Создание канала */
	if (pipe(fd) < 0) Err_Handler(__LINE__);

	res = fork();
	if (res == -1) Err_Handler(__LINE__);
	if (!res) {	//	Потомок
		close(fd[1]);	//	Закрытие канала для записи
		close(0);		//	Закрытие дескриптора stdin
		if (dup(fd[0]) < 0) Err_Handler(__LINE__); //	Установка дескриптора чтения на stdin
		close(fd[0]);	//	Закрытие старого дескриптора чтения
		execlp("wc", "wc", argv[1], NULL);	//	Вызов wc (флаг на кол-во символов, ибо строка всего 1)
		Err_Handler(__LINE__);
	} else {	//	Родитель
		close(fd[0]);	//	Закрытие канала для чтения
		close(1);		//	Закрытие stdout
		if (dup(fd[1]) < 0) Err_Handler(__LINE__); //	Установка записи в stdout
		close(fd[1]);	//	Закрытие старого дескриптора записи
		execlp("who", "who", NULL);
		Err_Handler(__LINE__);
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