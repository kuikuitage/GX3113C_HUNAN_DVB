/*
 * =====================================================================================
 *
 *       Filename:  common_commands.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/28/2014 05:04:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Z.Z.R 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#include <stddef.h>
#include <stdio.h>

#include "../include/module_command.h"
#include "../include/module_device.h"
#include "../include/common_autotest.h"

enum
{
	IS_SUCCESSFUL = 0,
	IS_CODE_ERROR,
	PARAMETER_IS_INVALID,	
};

static dev_tbl_t *dev_ops = NULL;

static signed int do_set_mode(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
    char dev_name[10] = {0};
	char *pResult = NULL;
	unsigned int result_code = IS_SUCCESSFUL;
	
	if((argc > cmdtp->maxargs) || (argc < 2))
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}

	sscanf(argv[1],"%*[^=]=%s",dev_name);
	if(strlen(dev_name) == 0)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: demod type is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}

	dev_ops = find_dev(dev_name);
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: the demod type is wrong or not register!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	pResult = "successful";
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	// TODO: reply lock status
	return 0;
err:
	// TODO: reply failed
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;

}


static signed int do_set_tp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	void *tp_info = NULL;
	void *frontend_info = NULL;
	signed int ret = 0; 
	char *pResult = NULL;
	unsigned int result_code = IS_SUCCESSFUL;
	
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(dev_ops->parse == NULL)
	{
		// no function for parse
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: software error, no parse function!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	ret = dev_ops->parse(orgcmd, &frontend_info, &tp_info);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parse command failed!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	// init frontend parameter: device, demux, and so on.
	ret = init_frontend(frontend_info);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: initialize frontend failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	
	if(dev_ops->lock_tp == NULL)
	{
		// no function for set tp
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: software error, no set tp function!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	ret = dev_ops->lock_tp(tp_info);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: set tp failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	pResult = "successful";
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	// TODO: reply lock status
	return 0;
err:
	// TODO: reply failed
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;

}

static signed int do_set_destroy_friontend(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	char *pResult = NULL;
	//char buffer[100] = {0};
	int ret = 0;
	unsigned int result_code = IS_SUCCESSFUL;
	
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	ret = destroy_frontend();
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: destroy frontend failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	pResult = "successful";
	// TODO: reply lock status
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return 0;
err:
	// TODO: reply failed
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;	
}


static signed int do_get_lock_status(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	int lock_status = 0;
	char *pResult = NULL;
	char buffer[100] = {0};
	char result[10] = {0};
	unsigned int result_code = IS_SUCCESSFUL;
	
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	// get data
	lock_status = get_lock_status();
	if(lock_status < 0)
	{
		pResult = "failed: get lock status failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	if(lock_status > 0)
	{
		sprintf(buffer, "%s: %s", "successful", "locked");
		strcat(result, "(1)");
	}
	else
	{
		sprintf(buffer, "%s: %s", "successful", "unlocked");
		strcat(result, "(0)");
	}
	pResult = buffer;
	// TODO: reply lock status
	auto_test_reply(cmdtp->name, pResult, result_code, result);
	return 0;
err:
	// TODO: reply failed
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;
}


static signed int do_set_play(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	void *prog_info = NULL;
	signed int ret = 0; 
	char *pResult = NULL;
	unsigned int result_code = IS_SUCCESSFUL;
	
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(dev_ops->parse == NULL)
	{
		// no function for parse
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: software error, no parse function!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	ret = dev_ops->parse(orgcmd, &prog_info, NULL);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parse command failed!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	// init play parameter
	ret = init_play(prog_info);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: initialize play failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	
	if(dev_ops->play == NULL)
	{
		// no function for play
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: software error, no play function!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	ret = dev_ops->play(prog_info);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: play program failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	pResult = "successful";
	// TODO: replay the ts compare result
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return 0;
err:
	// TODO: failed 
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;

}


static signed int do_set_ts_compare(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	void *ts_info = NULL;
	signed int ret = 0; 
	char *pResult = NULL;
	unsigned int result_code = IS_SUCCESSFUL;
	
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(dev_ops->parse == NULL)
	{
		// no function for parse
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: software error, no parse function!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	ret = dev_ops->parse(orgcmd, &ts_info, NULL);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parse command failed!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	ret = compare_ts_data(ts_info);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: set ts data compare failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	pResult = "successful";
	// TODO: replay the ts compare result
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return 0;
err:
	// TODO: failed 
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;

}


static signed int do_get_ts_compare_result(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	int ReceivePackageCount = 0, ErrorPackageCount = 0, ret = 0;
	char *pResult = NULL;
	char buffer[256] = {0};
	int lock_status = 0;
	unsigned int result_code = IS_SUCCESSFUL;
	char result[50] = {0};
	
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	ret = get_ts_compare_result(&ReceivePackageCount,&ErrorPackageCount);
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: get ts compare result failed! maybe the function is not work";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	// get lock status
	lock_status = get_lock_status();
	if(lock_status < 0)
	{
		pResult = "failed: get lock status failed! maybe the frontend device has been destroyed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	if(lock_status > 0)
		sprintf(buffer, "%s: %s(lock status) %d(receive packages) %d(error packages)", "successful", "locked", ReceivePackageCount, ErrorPackageCount);
	else
		sprintf(buffer, "%s: %s(lock status) %d(receive packages) %d(error packages)", "successful", "unlocked", ReceivePackageCount, ErrorPackageCount);

	sprintf(result, "(%d,%d,%d)", lock_status, ReceivePackageCount, ErrorPackageCount);
	pResult = buffer;
	// TODO: replay the ts compare result
	auto_test_reply(cmdtp->name, pResult, result_code, result);
	return 0;
err:
	// TODO: failed 
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;
}

static signed int do_set_play_stop(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	int ret = 0;
	char *pResult = NULL;
	unsigned int result_code = IS_SUCCESSFUL;
	
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	ret = stop_play();
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: stop play failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	pResult = "successful";
	// TODO: replay the ts compare result
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return 0;
err:
	// TODO: failed 
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;

}

static signed int do_set_ts_compare_stop(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	int ret = 0;
	char *pResult = NULL;
	unsigned int result_code = IS_SUCCESSFUL;
	
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	ret = stop_ts_compare();
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: stop compare ts failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	pResult = "successful";
	// TODO: replay the ts compare result
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return 0;
err:
	// TODO: failed 
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;

}

static signed int do_set_all_stop(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[], char *orgcmd)
{
	int ret = 0;
	char *pResult = NULL;
	unsigned int result_code = IS_SUCCESSFUL;
	if(dev_ops == NULL)
	{
		// set mode failed
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: do not select a vaild demod mode! set mode first!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	if(argc > cmdtp->maxargs)
	{
		// parameter error
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: parameter is wrong!";
		result_code = PARAMETER_IS_INVALID;
		goto err;
	}
	ret = stop_all();
	if(ret < 0)
	{
		log_printf("\nAUTO TEST, error, [%s, %d]!\n", __FUNCTION__, __LINE__);
		pResult = "failed: stop all functions failed!";
		result_code = IS_CODE_ERROR;
		goto err;
	}
	// clear all
	dev_ops = NULL;
	pResult = "successful";
	// TODO: replay the ts compare result
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return 0;
err:
	// TODO: failed 
	auto_test_reply(cmdtp->name, pResult, result_code, NULL);
	return -1;

}

cmd_tbl_t cmd_set_mode={
	.name = "set_demod_type",
	.maxargs = 2,
	.repeatable = 1,
	.cmd = do_set_mode,
	.usage = "set the demod mode,such as dvbs or dvbc."
};

cmd_tbl_t cmd_set_tp={
	.name = "set_tp",
	.maxargs = 10,
	.repeatable = 1,
	.cmd = do_set_tp,
	.usage = "set tp, need demux parameters"
};

cmd_tbl_t cmd_set_destroy_frontend={
	.name = "clear_tp_lock",
	.maxargs = 1,
	.repeatable = 1,
	.cmd = do_set_destroy_friontend,
	.usage = "destroy frontend resource"
};


cmd_tbl_t cmd_get_lock_status={
	.name = "get_tp_status",
	.maxargs = 1,
	.repeatable = 1,
	.cmd = do_get_lock_status,
	.usage = "get lock status, include demod status and demux status"
};

cmd_tbl_t cmd_set_play ={
	.name = "play",
	.maxargs = 6,
	.repeatable = 1,
	.cmd = do_set_play,
	.usage = "play program, need set tp first"
};

cmd_tbl_t cmd_set_play_stop ={
	.name = "stop",
	.maxargs = 1,
	.repeatable = 1,
	.cmd = do_set_play_stop,
	.usage = "stop play, need set tp first"
};


cmd_tbl_t cmd_set_ts_compare ={
	.name = "set_ts_compare",
	.maxargs = 2,
	.repeatable = 1,
	.cmd = do_set_ts_compare,
	.usage = "start ts compare, need set tp first"
};

cmd_tbl_t cmd_get_ts_compare_result ={
	.name = "get_ts_compare_result",
	.maxargs = 1,
	.repeatable = 1,
	.cmd = do_get_ts_compare_result,
	.usage = "get the result of the TS compare, need set tp first"
};

cmd_tbl_t cmd_set_ts_compare_stop ={
	.name = "clear_ts_compare",
	.maxargs = 1,
	.repeatable = 1,
	.cmd = do_set_ts_compare_stop,
	.usage = "stop comparing TS"
};

cmd_tbl_t cmd_set_all_stop ={
	.name = "exit",
	.maxargs = 1,
	.repeatable = 1,
	.cmd = do_set_all_stop,
	.usage = "stop all functions"
};




