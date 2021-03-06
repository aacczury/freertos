#include "shell.h"
#include <stddef.h>
#include "clib.h"
#include <string.h>
#include "fio.h"
#include "filesystem.h"

#include "FreeRTOS.h"
#include "task.h"
#include "host.h"

typedef struct {
	const char *name;
	cmdfunc *fptr;
	const char *desc;
} cmdlist;

void ls_command(int, char **);
void man_command(int, char **);
void cat_command(int, char **);
void ps_command(int, char **);
void host_command(int, char **);
void help_command(int, char **);
void host_command(int, char **);
void mmtest_command(int, char **);
void dumpsys_command(int, char **);
void host_write_command(int, char **);

#define MKCL(n, d) {.name=#n, .fptr=n ## _command, .desc=d}

cmdlist cl[]={
	MKCL(ls, "List directory"),
	MKCL(man, "Show the manual of the command"),
	MKCL(cat, "Concatenate files and print on the stdout"),
	MKCL(ps, "Report a snapshot of the current processes"),
	MKCL(host, "Run command on host"),
	MKCL(mmtest, "heap memory allocation test"),
	MKCL(help, "help"),
	MKCL(dumpsys, "show task TCB"),
	MKCL(host_write, "write something to host file")
};

int parse_command(char *str, char *argv[]){
	int b_quote=0, b_dbquote=0;
	int i;
	int count=0, p=0;
	for(i=0; str[i]; ++i){
		if(str[i]=='\'')
			++b_quote;
		if(str[i]=='"')
			++b_dbquote;
		if(str[i]==' '&&b_quote%2==0&&b_dbquote%2==0){
			str[i]='\0';
			argv[count++]=&str[p];
			p=i+1;
		}
	}
	/* last one */
	argv[count++]=&str[p];

	return count;
}

void ls_command(int n, char *argv[]){

}

int filedump(const char *filename){
	char buf[128];

	int fd=fs_open(filename, 0, O_RDONLY);

	if(fd==OPENFAIL)
		return 0;

	fio_printf(1, "\r\n");

	int count;
	while((count=fio_read(fd, buf, sizeof(buf)))>0){
		fio_write(1, buf, count);
	}

	fio_close(fd);
	return 1;
}

void ps_command(int n, char *argv[]){
	signed char buf[1024];
	vTaskList(buf);
	fio_printf(1, "\r\n%s\r\n", buf);	
}

void cat_command(int n, char *argv[]){
	if(n==1){
		fio_printf(2, "\r\nUsage: cat <filename>\r\n");
		return;
	}

	if(!filedump(argv[1]))
		fio_printf(2, "\r\n%s no such file or directory.\r\n", argv[1]);
}

void man_command(int n, char *argv[]){
	if(n==1){
		fio_printf(2, "\r\nUsage: man <command>\r\n");
		return;
	}

	char buf[128]="/romfs/manual/";
	strcat(buf, argv[1]);

	if(!filedump(buf))
		fio_printf(2, "\r\nManual not available.\r\n");
}

void host_command(int n, char *argv[]){
	int i, len = 0, rnt;
	char cmd[128] = {0};
	if(n>1){
		for(i=1; i<n; ++ i){
			memcpy(&cmd[len], argv[i], strlen(argv[i]));
			len += strlen(argv[i]) + 1;
			cmd[len - 1] = ' ';
		}
		cmd[len - 1] = '\0';
		rnt=host_system(cmd);
		fio_printf(1, "\r\nfinish with exit code %d.\r\n", rnt);
	}else
		fio_printf(2, "\r\nUsage: host 'command'\r\n");
}

void help_command(int n,char *argv[]){
	int i;
	fio_printf(1, "\r\n");
	for(i=0;i<sizeof(cl)/sizeof(cl[0]); ++i){
		fio_printf(1, "%s - %s\r\n", cl[i].name, cl[i].desc);
	}
}

void dumpsys_command(int n, char *argv[]){
	fio_printf(1, "\r\n");
	xTaskHandle xHandle = NULL;
	xHandle = xTaskGetCurrentTaskHandle();
	fio_printf(1, "%s\r\n", pcTaskGetTaskName(xHandle));
}

void host_write_command(int n, char *argv[]) {
    int fd;
    int error;
    int flag = 8;
    int i;

    if(!strcmp(argv[1], "-w")) flag = 4;
    else if(!strcmp(argv[1], "-a")) flag = 8;
    else{
        fio_printf(1, "Error mode.\r\n");
    	return;
    }

    fd = host_open(argv[2], flag);
    if(fd == -1) {
        fio_printf(1, "Open %s error!\r\n", argv[2]);
        return;
    }
    for(i=3; i<n; ++i){
	    error = host_write(fd, (void *)argv[i], strlen(argv[i]));
	    if(error != 0) {
	        fio_printf(1, "Write file error! Remain %d bytes didn't write in the file.\r\n", error);
	        host_close(fd);
	        return;
	    }
	    if(i+1<n)
	    	host_write(fd, (void *)" ", 1);
	}

    host_close(fd);
}

cmdfunc *do_command(const char *cmd){

	int i;

	for(i=0; i<sizeof(cl)/sizeof(cl[0]); ++i){
		if(strcmp(cl[i].name, cmd)==0)
			return cl[i].fptr;
	}
	return NULL;	
}
