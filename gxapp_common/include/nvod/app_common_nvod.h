/**
 *
 * @file        app_common_nvod.h
 * @brief
 * @version     1.1.0
 * @date        10/19/2012 09:40:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_COMMON_NVOD__H__
#define __APP_COMMON_NVOD__H__
#ifdef __cplusplus
extern "C" {
#endif

int app_nvod_get_shift_prog_pos_from_nvod_id(int nvod_id,int nvod_type);
int app_nvod_get_shift_prog_id_from_nvod_id(int nvod_id);

int app_nvod_set_full_screen_play_flag(int flag);
int app_nvod_set_full_screen_play_flag(int flag);

int app_nvod_set_full_screen_play_flag(int flag);


int app_nvod_get_full_screen_play_flag(void);

int app_nvod_set_playing_pos(int nvod_pos);

int app_nvod_get_playing_pos(void);


#ifdef __cplusplus
}
#endif
#endif /*__APP_COMMON_NVOD__H__*/

