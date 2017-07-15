#include "app_error.h"

#include "port.h"

#include "log.h"

/**@brief Function for error handling, which is called when an error has occurred.
 */
void app_error_handler(void)
{
    //异常处理，在这里死循环，等待硬狗复位系统
    APP_ERROR_HANDLER();
}

/**@brief Function for error handling, which is called when an error has occurred.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_handler_log(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    //打印异常的信息
    LOG(LEVEL_DEBUG, "[app_error]error_code = %d, line_num = %d, file = %s", error_code, line_num, p_file_name);

    //处理异常
    app_error_handler();
}

