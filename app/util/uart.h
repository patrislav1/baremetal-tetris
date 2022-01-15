#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "board.h"

void uart_init(void);

void uart_write_bytes(const char* buf, size_t n);

char uart_getch(void);
bool uart_has_input(void);
void uart_reset(void);

#ifdef CONSOLE_USB
void usb_rx(const char* data, size_t len);
#endif
