#pragma once
#include "pico/util/queue.h"

#define queue_size 6
#define chunk_size 256 * 38

queue_t free_queue;
queue_t full_queue;

typedef struct
{
    uint8_t data[1024];
    uint8_t id;
} recording_t;
recording_t recordings[queue_size];

typedef struct
{
    uint8_t id;
    int sm;
    int dma_chans[2];
    uint8_t data[2][chunk_size];
    dma_channel_config dma_config[2];
    uint32_t current;
} reader_t;
