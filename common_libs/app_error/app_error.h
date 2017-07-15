#ifndef APP_ERROR_H__
#define APP_ERROR_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APP_ERROR_HANDLER_LOG_DETAIL            (1)
#define APP_ERROR_HANDLER_LOG_SIMPLE            (2)
#define APP_ERROR_HANDLER_LOG_CLOSE             (3)

#ifndef APP_ERROR_HANDLER_LEVEL
#define APP_ERROR_HANDLER_LEVEL                 (APP_ERROR_HANDLER_LOG_DETAIL)
#endif

#define APP_SUCCESS                             (0)
#define APP_FAIL                                (1000)


//对外函数接口
void app_error_handler(void);
void app_error_handler_log(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);


/**@brief Macro for calling error handler function.
 *
 * @param[in] ERR_CODE Error code supplied to the error handler.
 */
#if (APP_ERROR_HANDLER_LOG_DETAIL == APP_ERROR_HANDLER_LEVEL)
#define APP_ERROR_HANDLER(ERR_CODE)                                         \
    do                                                                      \
    {                                                                       \
        app_error_handler_log((ERR_CODE), __LINE__, (uint8_t*) __FILE__);   \
    } while (0)

#elif (APP_ERROR_HANDLER_LOG_SIMPLE == APP_ERROR_HANDLER_LEVEL)
#define APP_ERROR_HANDLER(ERR_CODE)                                         \
    do                                                                      \
    {                                                                       \
        app_error_handler_log((ERR_CODE), 0, NULL);                         \
    } while (0)

#elif (APP_ERROR_HANDLER_LOG_CLOSE == APP_ERROR_HANDLER_LEVEL)
#define APP_ERROR_HANDLER(ERR_CODE)                                         \
    do                                                                      \
    {                                                                       \
        app_error_handler();                                                \
    } while (0)

#endif


#define APP_ERROR_CHECK(ERR_CODE)                             \
    do                                                        \
    {                                                         \
        const uint32_t LOCAL_ERR_CODE = (ERR_CODE);           \
        if (LOCAL_ERR_CODE != APP_SUCCESS)                    \
        {                                                     \
            APP_ERROR_HANDLER(LOCAL_ERR_CODE);                \
        }                                                     \
    } while (0)

#define APP_ERROR_CHECK_BOOL(BOOLEAN_VALUE)                   \
    do                                                        \
    {                                                         \
        const uint32_t LOCAL_BOOLEAN_VALUE = (BOOLEAN_VALUE); \
        if (!LOCAL_BOOLEAN_VALUE)                             \
        {                                                     \
            APP_ERROR_HANDLER(APP_FAIL);                      \
        }                                                     \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
