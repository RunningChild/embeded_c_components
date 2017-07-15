#ifndef __CRC16_H
#define __CRC16_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif


//对外函数接口
uint16_t bd_crc16(uint16_t crc, const uint8_t *buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
