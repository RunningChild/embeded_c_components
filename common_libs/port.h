#ifndef __PORT_H
#define __PORT_H

#include "common_string.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COMMON_LIBS_VERSION                 VERSION(1, 0, 3, 0)

#define APP_ERROR_HANDLER()                 while(1)
#define ENTER_REGION_SECTION()              INT_Disable()
#define EXIT_REGION_SECTION()               INT_Enable()
#define MEM_FREE(p)                         free_overwrite(p)
#define MEM_MALLOC(len)                     malloc_overwrite(len)

#ifdef __cplusplus
}
#endif

#endif

