#include "app.h"

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

WndStatus app_get_file_path_dlg(FileListParam *file_list);

