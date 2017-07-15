#include <string.h>
#include <stdlib.h>
#include "common_string.h"

/******************************************************************************
 * @brief uint16_to_char_array
 *****************************************************************************/
void uint16_to_char_array(uint16_t val, uint8_t* data)
{
    data[0] = ((val) >> 8) & 0x0FF;
    data[1] = (val) & 0x0FF;
}

/******************************************************************************
 * @brief uint32_to_char_array
 *****************************************************************************/
void uint32_to_char_array(uint32_t val, uint8_t* data)
{
    data[0] = ((val) >> 24) & 0x0FF;
    data[1] = ((val) >> 16) & 0x0FF;
    data[2] = ((val) >> 8) & 0x0FF;
    data[3] = (val) & 0x0FF;
}

/******************************************************************************
 * @brief convert_version_string_to_number
 *****************************************************************************/
int convert_version_string_to_number(char *version_string, uint32_t *version_number)
{
    char *v1,*v2,*v3,*v4;
    uint32_t n1,n2,n3,n4;

    char *search = ".";

    if(NULL == version_string) {
        return -1;
    }

    v1 = strtok(version_string, search);
    if(NULL == v1) {
        return -1;
    }

    n1 = strtoul(v1,NULL,10);
    if(n1 > 0xff) {
        return -1;
    }

    v2 = strtok(NULL, search);
    if(NULL == v2) {
        return -1;
    }

    n2 = strtoul(v2,NULL,10);
    if(n2 > 0xff) {
        return -1;
    }

    v3 = strtok(NULL, search);
    if(NULL == v3) {
        return -1;
    }

    n3 = strtoul(v3,NULL,10);
    if(n3 > 0xff) {
        return -1;
    }

    v4 = strtok(NULL, search);
    if(NULL == v4) {
        return -1;
    }

    n4 = strtoul(v4,NULL,10);
    if(n4 > 0xff) {
        return -1;
    }

    *version_number =  VERSION(n1,n2,n3,n4);

    return 0;

}

/******************************************************************************
 * @brief convert_number_to_version_string
 *****************************************************************************/
int convert_number_to_version_string(uint32_t version_number, char *version_string)
{
    uint8_t version_char[4];
    uint32_to_char_array(version_number, version_char);

    snprintf(version_string, 20, "%d.%d.%d.%d", version_char[0], version_char[1], version_char[2], version_char[3]);
    return 0;
}

/******************************************************************************
 * @brief hexstring_todec
 * @*str:十六进制字符串
 * @length:十六进制字符串长度
 *****************************************************************************/
bool hexstring_todec(uint8_t *str, uint16_t length)
{
    if(str == NULL)
    {
        return false;
    }

    for (int i = 0; i < length; i++)
    {
        if(str[i] >= '0' && str[i] <= '9')
        {
            str[i] = str[i] - '0';
        }
        else if(str[i] >= 'a' && str[i] <= 'f')
        {
            str[i] = str[i] - 'a' + 10;
        }
        else if(str[i] >= 'A' && str[i] <= 'F')
        {
            str[i] = str[i] - 'A' + 10;
        }
        else
        {
            return false;
        }
    }

    return true;
}

/******************************************************************************
 * @brief uint16_to_char_array
 * @*str:十进制
 * @length:十进制数组的长度
 *****************************************************************************/
bool dec_tohexstring(uint8_t *str, uint16_t length)
{
    if(str == NULL)
    {
        return false;
    }

    for (int i = 0; i < length; i++)
    {
        if(/*str[i] >= 0 && */str[i] <= 9)
        {
            str[i] = str[i] + '0';
        }
        else if(str[i] >= 10 && str[i] <= 15)
        {
            str[i] = str[i] + 'a' - 10;
        }
        else
        {
            return false;
        }
    }

    return true;
}

