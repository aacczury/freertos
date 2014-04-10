#include "host.h"

typedef union param_t{
	int pdInt;
	void *pdPtr;
	char *pdChrPtr;
} param;


/*action will be in r0, and argv in r1*/
int host_call(enum HOST_SYSCALL action, void *argv)
{
    /* For Thumb-2 code use the BKPT instruction instead of SWI.
* Refer to:
* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471c/Bgbjhiea.html
* http://en.wikipedia.org/wiki/ARM_Cortex-M#Cortex-M4 */
    int result;
    __asm__( \
      "bkpt 0xAB\n"\
      "nop\n" \
      "bx lr\n"\
        :"=r" (result) ::\
    );
    return result;
}

/* Refer to:
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0471c/Bgbjhiea.html */
int host_open(const char *pathname, int flags)
{
    param semi_param[3] = {
        { .pdChrPtr = (char *) pathname },
        { .pdInt    = flags },
        { .pdInt    = strlen(pathname) }
    };

    return host_call(SYS_OPEN, semi_param);
}

int host_close(int fd)
{
    return host_call(SYS_CLOSE, (void *)&fd);
}

size_t host_write(int fd, const void *buf, size_t count)
{
    param semi_param[3] = {
        { .pdInt = fd },
        { .pdPtr = (void *) buf },
        { .pdInt = count }
    };

    return host_call(SYS_WRITE, semi_param);
}

size_t host_read(int fd, void *buf, size_t count)
{
    param semi_param[3] = {
        { .pdInt = fd },
        { .pdPtr = buf },
        { .pdInt = count }
    };

    return host_call(SYS_READ, semi_param);
}

int host_system(char *cmd){
	return host_call(SYS_SYSTEM, (param []){{.pdChrPtr=cmd}, {.pdInt=strlen(cmd)}});
}
