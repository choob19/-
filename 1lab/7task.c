#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/*
	Написать программу для получения справочной информации о файлах
	с помощью системного вызова stat. Имя файла передается в программу
	из командной строки. Получить справочную информацию для различных
	типов файлов (обычный файл, каталог, специальные байт- и блок-ориентированные файлы).
*/

void Err_Handler(int fd, int line);

int main(int argc, char const *argv[])
{
	struct stat f_info = {0};
	time_t rawtime;	//	время в формате epoch
	struct tm time;	//	время в формате структуры
	char timebuf[64];
	int res = 0;

	if (argc < 2) {
		printf("Укажите необходимое число аргументов!\n");
		printf("%s [file_name]\n", argv[0]);
		return 0;
	}
	/* Вызов stat() */
	res = stat(argv[1], &f_info);
	if (res < 0)
		Err_Handler(0, __LINE__);

	printf("\tФайл: %s\n", argv[1]);
  printf("\tУстройство [ID]: %lu\n", f_info.st_dev);
  printf("\tInode: %lu\n", f_info.st_ino);
	printf("\tРежим доступа: %o\n", f_info.st_mode);	//	%o - спецификатор для 8-ричной сс
	printf("\tКол-во жестких ссылок: %lu\n", f_info.st_nlink);
	printf("\tИдентификатор пользователя-владельца [UID]: %d\n", f_info.st_uid);
	printf("\tИдентификатор группы-владельца [GID]: %d\n", f_info.st_gid);
	printf("\tТип устройства [Dev ID]: %lu\n", f_info.st_rdev);
	printf("\tОбщий размер в байтах: %lu байт\n", f_info.st_size);
	printf("\tКоличество выделенных блоков: %lu\n", f_info.st_blocks);

	rawtime = f_info.st_atime;		//	получение времени в формате epoch
	time = *localtime(&rawtime);	//	преобразование времени в структуру
	/* форматирование времени и преобразование его в строку */
	strftime(timebuf, sizeof(timebuf), "%d-%m-%Y %H:%M:%S %Z", &time);
	printf("\tВремя последнего доступа [atime]:\t%s\n", timebuf);

	rawtime = f_info.st_mtime;
	time = *localtime(&rawtime);
	strftime(timebuf, sizeof(timebuf), "%d-%m-%Y %H:%M:%S %Z", &time);
	printf("\tВремя последней модификации [mtime]:\t%s\n", timebuf);

	rawtime = f_info.st_ctime;
	time = *localtime(&rawtime);
	strftime(timebuf, sizeof(timebuf), "%d-%m-%Y %H:%M:%S %Z", &time);
	printf("\tВремя посл. мод. метаданных [ctime]:\t%s\n\n", timebuf);

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

/*
	struct stat {
    dev_t         st_dev;       устройство
    ino_t         st_ino;       inode
    mode_t        st_mode;      режим доступа
    nlink_t       st_nlink;     количество жестких ссылок
    uid_t         st_uid;       идентификатор пользователя-владельца
    gid_t         st_gid;       идентификатор группы-владельца
    dev_t         st_rdev;      тип устройства
                                (если это устройство)
    off_t         st_size;      общий размер в байтах
    blksize_t     st_blksize;   размер блока ввода-вывода
                                в файловой системе
    blkcnt_t      st_blocks;    количество выделенных блоков
    time_t        st_atime;     время последнего доступа
    time_t        st_mtime;     время последней модификации
    time_t        st_ctime;     время последнего изменения
	};
*/
