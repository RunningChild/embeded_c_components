#include <stdlib.h>
#include <string.h>

#include "poll_buffer.h"
#include "common.h"

#include "port.h"

static __INLINE uint32_t poll_length(poll_t * p_poll)
{
    uint32_t tmp = p_poll->read_pos;
    return p_poll->write_pos - tmp;
}

/**< Macro for calculating the FIFO length. */
#define POLL_LENGTH poll_length(p_fifo)


/**@brief Put one byte to the FIFO. */
static __INLINE void poll_put(poll_t * p_poll, uint8_t byte)
{
    p_poll->p_buf[p_poll->write_pos & p_poll->buf_size_mask] = byte;
    p_poll->write_pos++;
}


/**@brief Look at one byte in the FIFO. */
static __INLINE void poll_peek(poll_t * p_poll, uint16_t index, uint8_t * p_byte)
{
    *p_byte = p_poll->p_buf[(p_poll->read_pos + index) & p_poll->buf_size_mask];
}


/******************************************************************************
 * @brief poll_init
 *****************************************************************************/
int poll_init(poll_t * poll, int poll_size)
{
    if (poll == NULL)
        return -1;

    if (poll_size == 0)
        return -2;

    if (!IS_POWER_OF_TWO(poll_size))
        return -3;

    uint8_t * poll_buf = MEM_MALLOC(poll_size);
    if (poll_buf == NULL)
        return -3;

    poll->p_buf = poll_buf;
    poll->buf_size_mask = poll_size - 1;
    poll->read_pos = 0;
    poll->write_pos = 0;
    return 0;
}

/******************************************************************************
 * @brief poll_init_static
 * @cannot release data_buf before poll_uninit_static
 *****************************************************************************/
int poll_init_static(poll_t * p_poll, uint8_t * data_buf, int poll_size)
{
    if (p_poll == NULL)
        return -1;

    if (poll_size == 0)
        return -2;

    if (!IS_POWER_OF_TWO(poll_size))
        return -3;

    uint8_t * poll_buf = data_buf;

    p_poll->p_buf = poll_buf;
    p_poll->buf_size_mask = poll_size - 1;
    p_poll->read_pos = 0;
    p_poll->write_pos = 0;
    return 0;
}

/******************************************************************************
 * @brief poll_uninit
 *****************************************************************************/
void poll_uninit(poll_t * p_poll)
{
    if (p_poll == NULL)
        return;

    if (p_poll->p_buf)
        MEM_FREE(p_poll->p_buf);

    p_poll->p_buf = NULL;
    p_poll->buf_size_mask = 0;
    p_poll->read_pos = 0;
    p_poll->write_pos = 0;
}

/******************************************************************************
 * @brief poll_uninit_static
 *****************************************************************************/
void poll_uninit_static(poll_t * poll)
{
    if (poll == NULL)
        return;

    poll->p_buf = NULL;
    poll->buf_size_mask = 0;
    poll->read_pos = 0;
    poll->write_pos = 0;
}

/******************************************************************************
 * @brief poll_get_data_size
 *****************************************************************************/
int poll_get_data_size(poll_t * p_poll)
{
    return  poll_length(p_poll);
}

/******************************************************************************
 * @brief poll_fetch
 *****************************************************************************/
int poll_fetch(poll_t * p_poll, uint8_t * p_buf, int except_size)
{
    if (p_poll == NULL || p_buf == NULL)
        return -1;

    const uint32_t byte_count    = poll_length(p_poll);
    const uint32_t requested_len = except_size;
    uint32_t       index         = 0;
    uint32_t       read_size     = MIN(requested_len, byte_count);

    // Fetch bytes from the FIFO.
    while (index < read_size)
    {
        poll_peek(p_poll, index, &p_buf[index]);
        index++;
    }

    return read_size;
}

/******************************************************************************
 * @brief poll_flush
 * @thread safe
 *****************************************************************************/
int poll_flush(poll_t * p_poll)
{
    ENTER_REGION_SECTION();
    p_poll->write_pos = 0;
    p_poll->read_pos = 0;
    EXIT_REGION_SECTION();

    return 0;
}

/******************************************************************************
 * @brief poll_read
 * @thread safe
 *****************************************************************************/
int poll_read(poll_t * p_poll, uint8_t * p_buf, int except_size)
{
    if (p_poll == NULL || p_buf == NULL)
        return -1;

    uint32_t read_size = poll_fetch(p_poll, p_buf, except_size);

    ENTER_REGION_SECTION();
    p_poll->read_pos += read_size;
    EXIT_REGION_SECTION();

    return read_size;
}

/******************************************************************************
 * @brief poll_write
 * @thread safe
 *****************************************************************************/
int poll_write(poll_t * p_poll, uint8_t * p_buf, int except_size)
{
    if (p_poll == NULL || p_buf == NULL)
        return -1;

    const uint32_t available_count = p_poll->buf_size_mask - poll_length(p_poll) + 1;
    const uint32_t requested_len   = except_size;
    uint32_t       index           = 0;
    uint32_t       write_size      = MIN(requested_len, available_count);

    if( (requested_len != available_count)
     && (available_count == write_size) )
    {
        LOG(LEVEL_DEBUG, "\033[1;34;40mpoll write full!\033[0m");
    }

    ENTER_REGION_SECTION();
    while (index < write_size)
    {
        poll_put(p_poll, p_buf[index++]);
    }
    EXIT_REGION_SECTION();

    return write_size;
}

