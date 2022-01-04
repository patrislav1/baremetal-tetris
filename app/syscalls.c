#include <errno.h>
#include <unistd.h>

#include "coop_sched.h"
#include "uart.h"

int _read(int fd, char* ptr, int len)
{
    if (fd != STDIN_FILENO) {
        errno = EBADF;
        return -1;
    }

    int l = len;
    while (l--) {
        while (!uart_has_input()) {
            sched_yield();
        }
        *ptr++ = uart_getch();
    }

    return len;
}

int _write(int fd, char* ptr, int len)
{
    if (fd != STDOUT_FILENO && fd != STDERR_FILENO) {
        errno = EBADF;
        return -1;
    }

    uart_write_bytes(ptr, len);

    return len;
}
