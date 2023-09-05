#ifndef HEADER_H
#define HEADER_H

// Define the macro, if it needs to be used by all source files
// including this header file
#define DG_LEN 59
#define DG_PER_MESSAGE 50
#define MESSAGE_LENGTH (DG_LEN * DG_PER_MESSAGE + 16)
#define MESSAGE_QUEUE_LENGTH 25

#define CS_PIN 14
#define MISO_PIN 15

#define STARTBYTE 0xa7

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define TRIGGER_PIN 7

#define CS_OUT_START 16
#define CS_IN_START 20
#define noop

#define TRIGGER_PIN 7
#define PPS_PIN 11
// #define PPS_PIN 6
#define FREQ 1000
#define TIME_CORRECTION 3

extern queue_t message_queue;
extern queue_t free_queue;

typedef struct message
{
    uint8_t data[MESSAGE_LENGTH];
} message_t;

#endif
