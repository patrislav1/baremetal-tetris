#include "uart.h"

#include <stddef.h>

#include "board.h"
#include "ringbuf.h"

#ifdef CONSOLE_UART
#include "usart.h"
#endif

#ifdef CONSOLE_USB
#include "coop_sched.h"
#include "usbd_cdc_if.h"
#endif

static char rx_buf[512];
static char tx_buf[512];

static ringbuf_t rx_rb = RINGBUF_INIT(rx_buf);
static ringbuf_t tx_rb = RINGBUF_INIT(tx_buf);

#ifdef CONSOLE_UART
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
#endif

static inline void do_send_char(const char c)
{
    while (ringbuf_full(&tx_rb)) {
#ifdef CONSOLE_USB
        sched_yield();
#endif
    };
    ringbuf_push(&tx_rb, c);
#ifdef CONSOLE_UART
    LL_USART_EnableIT_TXE(CONSOLE_UART);
#endif
}

#ifdef CONSOLE_USB
#define USB_MAX_PKT_SIZE 64
static void usb_tx_task(void* arg)
{
    while (1) {
        while (ringbuf_empty(&tx_rb)) {
            sched_yield();
        };

        char tmp[USB_MAX_PKT_SIZE];
        size_t i;
        for (i = 0; i < sizeof(tmp) && !ringbuf_empty(&tx_rb); i++) {
            tmp[i] = ringbuf_pop(&tx_rb);
        }
        while (CDC_Transmit_FS((uint8_t*)tmp, i) == USBD_BUSY) {
            sched_yield();
        }
    }
}

void usb_rx(const char* data, size_t len)
{
    while (len--) {
        if (!ringbuf_full(&rx_rb)) {
            ringbuf_push(&rx_rb, *data++);
        }
    }
}

static void usb_start(void)
{
    static uint8_t stack[2000];
    static coop_task_t task;
    sched_create_task(&task, &usb_tx_task, NULL, stack, sizeof(stack));
}

#endif

void uart_reset(void)
{
    ringbuf_init(&rx_rb);
    ringbuf_init(&tx_rb);
}

void uart_init(void)
{
    uart_reset();
#ifdef CONSOLE_UART
    LL_USART_EnableIT_RXNE(CONSOLE_UART);
    LL_USART_EnableIT_ERROR(CONSOLE_UART);
#endif

#ifdef CONSOLE_USB
    usb_start();
#endif
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
