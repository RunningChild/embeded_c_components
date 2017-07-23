#ifndef __PORT_H
#define __PORT_H

#include "common_string.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COMMON_LIBS_VERSION                 VERSION(1, 0, 0, 0)

#if defined(BSP_A)
#include "port_A.h"
#endif


#ifdef __cplusplus
}
#endif

#endif

