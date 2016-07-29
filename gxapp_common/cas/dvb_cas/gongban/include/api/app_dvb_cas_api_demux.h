/**
 *
 * @file        app_common_porting_stb_api.h
 * @brief
 * @version     1.1.0
 * @date        11/30/2012 09:47:49 AM
 * @author      zhouhuaming (zhouhm), zhuohm@nationalchip.com
 *
 */
#ifndef __APP_DVB_CAS_API_DEMUX_H__
#define __APP_DVB_CAS_API_DEMUX_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "app_common_play.h"
	handle_t app_dvb_cas_api_bat_get_filter_handle(void);
	handle_t app_dvb_cas_api_bat_get_channel_handle(void);
	uint8_t  app_dvb_cas_api_release_bat_filter(void);
	uint8_t  app_dvb_cas_api_release_ecm_filter(void);
	uint8_t  app_dvb_cas_api_start_ecm_filter(play_scramble_para_t *pplayPara);
	uint8_t  app_dvb_cas_api_release_emm_filter(void);
	uint8_t  app_dvb_cas_api_start_emm_filter(uint16_t emm_pid);
	int      app_dvb_cas_api_bat_filter_open(void);
	bool     app_dvb_cas_api_check_cat_ca_descriptor_valid(uint8_t *sectionData,uint32_t CA_system_id);
	bool     app_dvb_cas_api_check_pmt_ca_descriptor_valid(uint8_t *sectionData,uint32_t CA_system_id);
	void     app_dvb_cas_api_create_filter_time_out_task(void);
	void     app_dvb_cas_api_nit_zone_des(uint8_t* databuf);

	void     app_dvb_cas_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
	void     app_dvb_cas_api_bat_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
	void     app_dvb_cas_filter_timeout_notify(handle_t Filter, const uint8_t* Section, size_t Size);
	void app_dvb_cas_ecm_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
	void app_dvb_cas_emm_filter_notify(handle_t Filter, const uint8_t* Section, size_t Size);
	uint8_t app_dvb_cas_api_reset_ecm_filter(void);
	uint8_t app_dvb_cas_api_reset_emm_filter(void);

#ifdef __cplusplus
}
#endif
#endif /*__app_dvb_CAS_API_DEMUX_H__*/

