#ifndef ADVER_H
#define ADVER_H

//#include "stpti.h"

/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define  ADVER_OPEN

#define Adver_PROMPTOSD_MAXLEN			256
#define Adver_COLOR_TABLE						768

#define Adver_MAX_DISPLAYBMP				8

typedef struct tag_AdverScrollInfo_struct
{
	unsigned int		ADScroll_id;
	unsigned char		Scroll_time;	
	unsigned char		Font_color;
	unsigned char		Background_color;
	unsigned char		Postion;
	unsigned char  	xoffset;
	unsigned char 	AD_textlen;
	unsigned char		AD_text[Adver_PROMPTOSD_MAXLEN];
}Adver_Info;

#define DataBuff_Max_Number						4000

#define GIF_BUFFER_MAX				(64*1024)
#define GIF_BUFFER_FULLFLAG		(GIF_BUFFER_MAX/DataBuff_Max_Number+1)

#define SCROLL_BUFFER_MAX			(64*1024)
#define SCROLL_BUFFER_FULLFLAG	(SCROLL_BUFFER_MAX/DataBuff_Max_Number+1)

typedef struct tag_AdverscrollProgInfo_struct{
	unsigned char				prog_data[32];
}AdverScroll_ProgInfo;

typedef struct tag_Adver_struct{
	
	unsigned char mad_Times;
	unsigned short 	mad_StartxPos;
	unsigned short 	mad_StartyPos;
	unsigned short	mad_EndxPos;
	unsigned short	mad_EndyPos;
	unsigned short	mad_width;
	unsigned short	mad_height;
	unsigned char mad_MoveDir;
	unsigned char mad_Speed;
	unsigned char mad_StartTime[5];
	unsigned char mad_TextOrBmp;
	unsigned char mad_FontColor;
	unsigned char mad_BackgroundColor;	
}AdverScroll_Info;

typedef struct tag_AdverCorner_struct{
	unsigned short mad_StartxPos;
	unsigned short mad_StartyPos;
	unsigned short mad_Width;
	unsigned short mad_Height;
	unsigned char  mad_PicType;
	unsigned char  mad_StartTime[5];
	unsigned char  mad_Duration[3];

}AdverCorner_Info;

typedef struct tag_AdverOpenPic_struct{
	unsigned short mad_Width;
	unsigned short mad_Height;
	unsigned char  mad_PicFormat;
	unsigned char  mad_Duration;
	unsigned char  mad_Reserve[5];
	unsigned long  mad_CRC32;
	unsigned long  mad_TotleLen;
	unsigned char  mad_DataFullFlag[GIF_BUFFER_FULLFLAG];
	unsigned char  *mad_Data;
}AdverOpenPicture_Info;


typedef struct tag_SaveAdverCorner_struct{
	AdverScroll_ProgInfo	mad_ProgInfo;
	AdverCorner_Info		mad_CornerInfo;
	unsigned char mad_Type;
	unsigned long	mad_CRC32;
	unsigned long	mad_TotleLen;
	unsigned char mad_DataFullFlag[GIF_BUFFER_FULLFLAG];
	unsigned char	*mad_Data;
}SaveAdverCorner_Info;

typedef struct tag_SaveAdverScroll_struct{	
	AdverScroll_Info mad_ScrollInfo;
	unsigned char mad_Type;
	unsigned long mad_CRC32;
	unsigned long mad_TotleLen;
	unsigned char mad_DataFullFlag[SCROLL_BUFFER_FULLFLAG];
	unsigned char	*mad_Data;
}SaveAdverScroll_Info;

typedef struct tag_DisplayAdverScroll_struct{	
	AdverScroll_Info mad_ScrollInfo;
	AdverScroll_ProgInfo	mad_ProgInfo;
	AdverCorner_Info mad_CornerInfo;
	unsigned char mad_Type;
	unsigned long mad_CRC32;
	unsigned long mad_TotleLen;
	unsigned char	mad_Data[SCROLL_BUFFER_MAX];
}DisplayAdverScroll_Info;

typedef struct tag_AdverBitmap_struct{
	unsigned short mad_Left;
	unsigned short mad_Top;
	unsigned short mad_width;
	unsigned short mad_height;
	unsigned short mad_TransColorIndex;
	unsigned short mad_DelayTime;
	unsigned char  mad_TotleBmpNum;
	unsigned int	 mad_TotleLen;
	unsigned char	 mad_colorTable[Adver_COLOR_TABLE];
	unsigned char  *mad_data;
}AdverBitmap;

unsigned char Adver_Init(void);
//void AdverPsiStart(STPTI_Pid_t AdverPid,unsigned int ProgId);
void AdverPsiStop(void);
unsigned char GetAdverSpeed(void);

extern unsigned char Adver_GetDisplayInfo(Adver_Info *padInfo);
//extern semaphore_t  *gpAdverDataSem;
extern unsigned int gAdverProgramId;
extern SaveAdverCorner_Info gSaveAdverCorner_Info;
extern SaveAdverScroll_Info gSaveAdverScroll_Info;
/* End C++ support */
#ifdef __cplusplus
}
#endif/* __cplusplus */

#endif

