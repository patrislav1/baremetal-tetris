#pragma once

#include <stdbool.h>
#include <stddef.h>

void uart_init(void);

void uart_write_string(const char* str);
void uart_write_bytes(const char* buf, size_t n);

char uart_getch(void);
bool uart_has_input(void);
void uart_flush(void);
