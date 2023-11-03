#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/*
	Написать программу, которая выводит на экран содержимое файла в обратном порядке:
	сначала выводится последний символ файла, затем предпоследний и так далее до первого.
	Использовать lseek и read.
*/

void Err_Handler(int fd, int line);

int main(int argc, char const *argv[])
{
	int fd = 0, file_offset = 1, res = 0;
	char byte_read;
	if (argc < 2) {
		printf("Укажите необходимое число аргументов!\n");
		printf("%s [file_name]\n", argv[0]);
		return 0;
	}

	/* Открываем файл и сразу двигаем указатель в конец */
	fd = open(argv[1], O_RDONLY | O_APPEND);
	if (fd < 0)
		Err_Handler(0, __LINE__);

	for (int i = 1; file_offset > 0; i++) {
		file_offset = lseek(fd, -i, SEEK_END);
		if (file_offset < 0)
			Err_Handler(fd, __LINE__);

		res = read(fd, &byte_read, sizeof(char));
		if (res < 0)
			Err_Handler(fd, __LINE__);
		putchar(byte_read);
	}
	printf("\n\n");
	if (close(fd) < 0)
		Err_Handler(0, __LINE__);
	return 0;
}
//--------------------------------------------------------------------------
void Err_Handler(int fd, int line)
{
  printf("Errno value:\t%d\n", errno);
  fprintf(stderr, "Error occured! %s\n Line: %d\n", strerror(errno), line);
  if (fd)
    close(fd);
  exit(0);
}
