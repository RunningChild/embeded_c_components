#ifndef __PULL_BUFFER_H
#define __PULL_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PULL_BUFFER_SIZE        (512)

typedef struct
{
    uint8_t *           p_buf;
    uint16_t            buf_size_mask;
    volatile uint32_t   read_pos;
    volatile uint32_t   write_pos;
} poll_t;


//对外函数接口
int poll_init(poll_t * poll, int poll_size);
int poll_init_static(poll_t * poll, uint8_t * data_buf, int poll_size);
void poll_uninit(poll_t * poll);
void poll_uninit_static(poll_t * poll);
int poll_get_data_size(poll_t * poll);
int poll_flush(poll_t * p_poll);
int poll_fetch(poll_t * poll, uint8_t * buf, int except_size);
int poll_read(poll_t * poll, uint8_t * buf, int except_size);
int poll_write(poll_t * poll, uint8_t * buf, int except_size);

#ifdef __cplusplus
}
#endif

#endif
