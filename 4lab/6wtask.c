#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

/*
	Создать именованный канал (любым способом). Убедиться, что канал создан.
	Рассмотреть особенности работы с именованным каналом (открыть файл с
	установленным и не установленным флагом O_NDELAY). Использовать его для
	обмена информацией с процессом, созданным другим пользователем (своим соседом).
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
	int fd, res;
	struct stat f_info;
	char buf[] = "Test string";
	if (argc != 2) {
		printf("Укажите правильное число аргументов: %s [FIFO_name]\n", argv[0]);
		exit(0);
	}

	/* Проверка что файл является FIFO */
	if (stat(argv[1], &f_info) < 0) Err_Handler(__LINE__);
	if (!S_ISFIFO(f_info.st_mode)) {
		printf("Файл не является именованным каналом!\n");
		exit(0);
	}

	res = fork();
	if (res == -1) Err_Handler(__LINE__);
	if (!res) {	//	Потомок
		execlp("./6rtask", "6rtask", NULL);
		Err_Handler(__LINE__);
	} else {		//	Родитель
		/* Открытие файла на запись */
		fd = open(argv[1], O_WRONLY);
		if (fd < 0) Err_Handler(__LINE__);
		printf("[Р] Именованный канал открыт на запись\n");
		/* Запись в очередь */
		//sleep(1);
		res = write(fd, buf, sizeof(buf));
		if (res < 0) Err_Handler(__LINE__);
		printf("[P] В канал записано %d байт\n", res);
		res = wait(&res);
	}
	close(fd);
	return 0;
}
/*-----------------------------------------------------------------------*/
void Err_Handler(int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n %s:%d\n", strerror(errno), __FILE__, line);
  exit(0);
}