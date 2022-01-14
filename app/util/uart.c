#include "uart.h"

#include <stddef.h>

#include "board.h"
#include "ringbuf.h"
#include "usart.h"

static char rx_buf[512];
static char tx_buf[512];

static ringbuf_t rx_rb = RINGBUF_INIT(rx_buf);
static ringbuf_t tx_rb = RINGBUF_INIT(tx_buf);

void panic_print(const char* str)
{
    LL_USART_DisableIT_TC(CONSOLE_UART);
    LL_USART_DisableIT_TXE(CONSOLE_UART);
    LL_USART_DisableIT_RXNE(CONSOLE_UART);
    LL_USART_DisableIT_ERROR(CONSOLE_UART);

    while (*str) {
        while (!LL_USART_IsActiveFlag_TXE(CONSOLE_UART)) {
        };
        LL_USART_TransmitData8(CONSOLE_UART, *str++);
    }
}

void CONSOLE_IRQ_HANDLER(void)
{
    USART_TypeDef* const uart = CONSOLE_UART;
    if (LL_USART_IsEnabledIT_RXNE(uart) && LL_USART_IsActiveFlag_RXNE(uart)) {
        ringbuf_push(&rx_rb, LL_USART_ReceiveData8(uart));
    }

    if (LL_USART_IsEnabledIT_TXE(uart) && LL_USART_IsActiveFlag_TXE(uart)) {
        if (!ringbuf_empty(&tx_rb)) {
            LL_USART_TransmitData8(uart, ringbuf_pop(&tx_rb));
        } else {
            LL_USART_DisableIT_TXE(uart);
            LL_USART_EnableIT_TC(uart);
        }
    }

    if (LL_USART_IsEnabledIT_TC(uart) && LL_USART_IsActiveFlag_TC(uart)) {
        LL_USART_ClearFlag_TC(uart);
        LL_USART_DisableIT_TC(uart);
    }
}

static inline bool uart_has_pending()
{
    return LL_USART_IsEnabledIT_TXE(CONSOLE_UART) || LL_USART_IsEnabledIT_TC(CONSOLE_UART) ||
           !ringbuf_empty(&tx_rb);
}

static inline void do_send_char(const char c)
{
    while (ringbuf_full(&tx_rb)) {
    };
    ringbuf_push(&tx_rb, c);
    LL_USART_EnableIT_TXE(CONSOLE_UART);
}

void uart_init(void)
{
    ringbuf_init(&rx_rb);
    ringbuf_init(&tx_rb);

    LL_USART_EnableIT_RXNE(CONSOLE_UART);
    LL_USART_EnableIT_ERROR(CONSOLE_UART);
}

void uart_write_string(const char* str)
{
    while (*str) {
        do_send_char(*str++);
    }
}

void uart_write_bytes(const char* buf, size_t n)
{
    while (n--) {
        do_send_char(*buf++);
    }
}

char uart_getch(void)
{
    while (!uart_has_input()) {
    };
    return ringbuf_pop(&rx_rb);
}

bool uart_has_input(void)
{
    return !ringbuf_empty(&rx_rb);
}

void uart_flush(void)
{
    while (uart_has_pending()) {
    };
}
