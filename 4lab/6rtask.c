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
	Создать именованный канал (любым способом). Убедиться, что канал создан.
	Рассмотреть особенности работы с именованным каналом (открыть файл с
	установленным и не установленным флагом O_NDELAY). Использовать его для
	обмена информацией с процессом, созданным другим пользователем (своим соседом).
	(С флагом O_NDELAY вызов open() не будет блокироваться, и если данных в канале нету,
	он ыернет -1 и ошибку)
	При этом, если флага нету, то вызов будет блокироваться до тех пор, пока другой процесс не
	откроет файл очереди для противоположной операции.
*/

void Err_Handler(int line);

int main(int argc, char const *argv[])
{
	int fd, res;
	char buf[64];
	//sleep(1);	//	Если подождать, то родительский процесс успеет записать данные в очередь и ошибок не будет
	fd = open("my_fifo", O_RDONLY);
	if (fd < 0) Err_Handler(__LINE__);
	printf("[П] Именованный канал открыт на чтение\n");
	/* Чтение из очереди */
	res = read(fd, buf, sizeof(buf));
	if (res < 0) Err_Handler(__LINE__);
	printf("[П] Из канала прочитано %d байт\n", res);
	printf("[П] Получена строка: %s\n", buf);
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