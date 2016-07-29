//#include "app.h"
//#include "app_msg.h"
//#include "app_module.h"
#include "./include/common_autotest.h"
#include "./include/module_command.h"
#include "./include/module_device.h"
// command 
extern cmd_tbl_t cmd_set_mode;
extern cmd_tbl_t cmd_set_tp;
extern cmd_tbl_t cmd_set_destroy_frontend;
extern cmd_tbl_t cmd_get_lock_status;
extern cmd_tbl_t cmd_set_play;
extern cmd_tbl_t cmd_set_ts_compare;
extern cmd_tbl_t cmd_get_ts_compare_result;
extern cmd_tbl_t cmd_set_play_stop;
extern cmd_tbl_t cmd_set_ts_compare_stop;
extern cmd_tbl_t cmd_set_all_stop;
// demod device
extern dev_tbl_t dev_dvbs_reg;
extern dev_tbl_t dev_dvbt2_reg;
extern dev_tbl_t dev_dvbc_reg;
extern dev_tbl_t dev_dvbdtmb_reg;




// memory
static unsigned char receive_memory[MAX_CMDBUF_SIZE*2] = {0};
static int sg_memory_read_pos	= 0;
static int sg_memory_write_pos	= 0;

static status_t _autotest_service_init(handle_t self, int priority)
{
	handle_t sch;
	int ret = 0;
	// register command 
	
	register_cmd(&cmd_set_mode);
	register_cmd(&cmd_set_tp);
	register_cmd(&cmd_set_destroy_frontend);
	register_cmd(&cmd_get_lock_status);
	register_cmd(&cmd_set_play);
	register_cmd(&cmd_set_ts_compare);
	register_cmd(&cmd_get_ts_compare_result);
	register_cmd(&cmd_set_play_stop);
	register_cmd(&cmd_set_ts_compare_stop);
	register_cmd(&cmd_set_all_stop);

	// register support demod	
	register_dev(&dev_dvbs_reg);
	register_dev(&dev_dvbt2_reg);
	register_dev(&dev_dvbc_reg);
	register_dev(&dev_dvbdtmb_reg);


	// init console device such as uart
	ret = uart_init(115200);
	if(ret < 0)
		return GXCORE_ERROR;
	
	// create
	sch = GxBus_SchedulerCreate("AutoTestConsoleScheduler", GXBUS_SCHED_CONSOLE,
		1024 * 100, GXOS_DEFAULT_PRIORITY + priority);

	GxBus_ServiceLink(self, sch);

	return GXCORE_SUCCESS;	
}

static void _autotest_service_destroy(handle_t self)
{
	// unregister command

	// register support demod

	// deinit console device
    uart_close();
	// destroy

	GxBus_ServiceUnlink(self);

	return;
}

#if 0
static GxMsgStatus _autotest_service_recvmsg(handle_t self, GxMessage *msg)
{
	return GXMSG_OK;
}
#endif

static void _autotest_service_console(handle_t self)
{
	unsigned char Buffer[MAX_CMDBUF_SIZE] = {0};
	int ByteRealRead = 0;
	if((ByteRealRead = uart_receive(Buffer,MAX_CMDBUF_SIZE)) > 0)
	{// read data
		// deal with the cmd
		if(auto_test_protocol_data_receive(receive_memory, MAX_CMDBUF_SIZE*2, 
										&sg_memory_read_pos, &sg_memory_write_pos,
										Buffer, ByteRealRead) == 0)
		{
			auto_test_protocol_data_dealwith(receive_memory, MAX_CMDBUF_SIZE*2, 
										&sg_memory_read_pos, &sg_memory_write_pos,
										MAX_CMDBUF_SIZE*2);
		}
	}
#ifdef ECOS_OS
    GxCore_ThreadDelay(100);
#endif
}

GxServiceClass service_autotest = {
	.name		        = "hareware autotest service",
	.init		        = _autotest_service_init,
	.destroy 	        = _autotest_service_destroy,
	.msg_process 	    = NULL,
	.console_process    = _autotest_service_console,
};

void atuotest_create_serivce(void)
{
	GxBus_ServiceCreate(&service_autotest);
}

