#ifndef HOST_H
#define HOST_H
#include <string.h>

/* Refer to SYS_OPEN ARM document */
enum File_Type_t {
    OPEN_RD = 0,
    OPEN_RD_BIN, 
    OPEN_RD_ONLY,
    OPEN_RD_ONLY_BIN,
    OPEN_WR,
    OPEN_WR_BIN,
    OPEN_WR_ONLY,
    OPEN_WR_ONLY_BIN,
    OPEN_APPEND,
    OPEN_APPEND_BIN,
    OPEN_APPEND_ONLY,
    OPEN_APPEND_ONLY_BIN
};

/*
 *Reference: http://albert-oma.blogspot.tw/2012/04/semihosting.html
 */
enum HOST_SYSCALL{
	SYS_OPEN=0x01,
	SYS_CLOSE,
	SYS_WRITEC,
	SYS_WRITE0,
	SYS_WRITE,
	SYS_READ,
	SYS_READC,
	SYS_ISERROR,
	SYS_ISTTY,
	SYS_SEEK,
	SYS_FLEN=0xC,
	SYS_TMPNAM,
	SYS_REMOVE,
	SYS_RENAME,
	SYS_CLOCK,
	SYS_TIME,
	SYS_SYSTEM,
	SYS_ERRNO,
	SYS_GET_CMDLINE=0x15,
	SYS_HEAPINFO,
	SYS_ELAPSED=0x30,
	SYS_TICKFREQ
};

int host_call(enum HOST_SYSCALL, void *argv) __attribute__((naked));

int host_open(const char *pathname, int flags);
int host_close(int fd);
size_t host_write(int fd, const void *buf, size_t count);
size_t host_read(int fd, void *buf, size_t count);

int host_system(char *cmd);

#endif 
