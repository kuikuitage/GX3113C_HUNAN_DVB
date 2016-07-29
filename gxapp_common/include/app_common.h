/**
 *
 * @file        app_common.h
 * @brief
 * @version     1.1.0
 * @date        8/29/2014 16:20:49 PM
 * @author      cth,chenth@nationalchip.com
 *
 */
#ifndef __APP_COMMON__H__
#define __APP_COMMON__H__
 
/* 打印字体显示的颜色 */
#define NONE          "\033[m"
#define RED           "\033[0;31m"
#define GREEN         "\033[0;32m"
#define BLUE          "\033[0;34m"
#define DARY_GRAY     "\033[1;30m"
#define CYAN          "\033[0;36m"
#define PURPLE        "\033[0;35m"
#define BROWN         "\033[0;33m"
#define YELLOW        "\033[1;33m"
#define WHITE         "\033[1;37m"



#define INFO_MSG(fmt, args ... )                         \
    do {                                                \
        printf(GREEN "[MSG]"     \
                fmt NONE, ## args);              \
    } while(0)

#define INFO_ERR(fmt, args ... )                         \
    do {                                                \
        printf(RED "[ERR]"          \
                "(%s|%s|%d): " fmt NONE,            \
               __FILE__, __func__, __LINE__, ## args); \
    } while(0)

#define INFO_WARN(fmt, args ... )                        \
    do {                                                \
        printf(YELLOW "[WARN]"       \
                "(%s|%s|%d): " fmt NONE,           \
                __FILE__, __func__, __LINE__, ## args); \
    } while(0)

#ifdef _DEBUG
#define INFO_DBG(fmt, args ... )                         \
    do {                                                \
        printf(BLUE "[DBG]"         \
                "(%s|%s|%d): " fmt NONE,            \
                __FILE__, __func__, __LINE__, ## args); \
    } while(0)
#else
#define INFO_DBG(fmt, args ... )
#endif

#endif 

