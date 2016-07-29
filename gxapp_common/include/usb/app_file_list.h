#ifndef __APP_FILE_LIST_H__
#define __APP_FILE_LIST_H__

#include "module/update/gxupdate_partition_file.h"
#include "module/update/gxupdate_partition_flash.h"
#include "module/update/gxupdate_protocol_usb.h"
#include "module/update/gxupdate_protocol_ts.h"
#include "module/update/gxupdate_protocol_serial.h"
#include "module/config/gxconfig.h"
#include "service/gxextra.h"
#include "gxoem.h"
#include "gui_core.h"


#include "gxhotplug.h"
#include "gxupdate.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	DEST_MODE_FILE = 0,
	DEST_MODE_DIR
}DestPathMode;

typedef struct
{
    DestPathMode dest_mode;
    char **dest_path;
    char *cur_path;
    char *suffix;
    int pos_x;
    int pos_y;
}FileListParam;

typedef enum
{
	WND_EXEC = 0,
	WND_CANCLE,
	WND_OK 
}WndStatus;

WndStatus app_get_file_path_dlg(FileListParam *file_list);
void app_free_dir_ent(GxDirent **ents, int *nents);



#ifdef __cplusplus
}
#endif

#endif /* __APP_OPEN_FILE_H__ */


