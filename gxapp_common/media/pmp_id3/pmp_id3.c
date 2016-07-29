//#include "app.h"
#include "app_common_media.h"

#include "pmp_id3.h"
#include "gui_core.h"

#define TEXT_ID3_ARTIST0		"music_view_text_artist0"
#define TEXT_ID3_ALBUM0			"music_view_text_album0"
#define TEXT_ID3_YEAR0			"music_view_text_year0"
#define TEXT_ID3_GENRE0			"music_view_text_genre0"
#define TEXT_ID3_TRACK0			"music_view_text_track0"
#define TEXT_ID3_TITLE0			"music_view_text_title0"

#define TEXT_ID3_ARTIST1		"music_view_text_artist1"
#define TEXT_ID3_ALBUM1			"music_view_text_album1"
#define TEXT_ID3_YEAR1			"music_view_text_year1"
#define TEXT_ID3_GENRE1			"music_view_text_genre1"
#define TEXT_ID3_TRACK1			"music_view_text_track1"
#define TEXT_ID3_TITLE1			"music_view_text_title1"

#define IMG_ID3_BACKGROUND		"music_view_imag_id3back"

static PlayerID3Info *s_id3_info = NULL;

int id3_start(void);

int id3_create(void)
{
	printf("[ID3] id3_create\n");

	GUI_SetProperty(TEXT_ID3_ARTIST0, "state", "show");
	GUI_SetProperty(TEXT_ID3_ALBUM0, "state", "show");
	GUI_SetProperty(TEXT_ID3_YEAR0, "state", "show");
	GUI_SetProperty(TEXT_ID3_GENRE0, "state", "show");
	GUI_SetProperty(TEXT_ID3_TRACK0, "state", "show");
	GUI_SetProperty(TEXT_ID3_TITLE0, "state", "show");

	GUI_SetProperty(TEXT_ID3_ARTIST1, "state", "show");
	GUI_SetProperty(TEXT_ID3_ALBUM1, "state", "show");
	GUI_SetProperty(TEXT_ID3_YEAR1, "state", "show");
	GUI_SetProperty(TEXT_ID3_GENRE1, "state", "show");
	GUI_SetProperty(TEXT_ID3_TRACK1, "state", "show");
	GUI_SetProperty(TEXT_ID3_TITLE1, "state", "show");

	GUI_SetProperty(IMG_ID3_BACKGROUND, "state", "show");

	id3_start();
	
	return 0;
}

int id3_destroy(void)
{
	printf("[ID3] id3_destroy\n");
	
	GxPlayer_MediaFreeID3Info(s_id3_info);
	if(s_id3_info != NULL)
	{
		//GxCore_Free(s_id3_info);
		s_id3_info = NULL;
	}

	GUI_SetProperty(TEXT_ID3_ARTIST0, "state", "hide");
	GUI_SetProperty(TEXT_ID3_ALBUM0, "state", "hide");
	GUI_SetProperty(TEXT_ID3_YEAR0, "state", "hide");
	GUI_SetProperty(TEXT_ID3_GENRE0, "state", "hide");
	GUI_SetProperty(TEXT_ID3_TRACK0, "state", "hide");
	GUI_SetProperty(TEXT_ID3_TITLE0, "state", "hide");

	GUI_SetProperty(TEXT_ID3_ARTIST1, "state", "hide");
	GUI_SetProperty(TEXT_ID3_ALBUM1, "state", "hide");
	GUI_SetProperty(TEXT_ID3_YEAR1, "state", "hide");
	GUI_SetProperty(TEXT_ID3_GENRE1, "state", "hide");
	GUI_SetProperty(TEXT_ID3_TRACK1, "state", "hide");
	GUI_SetProperty(TEXT_ID3_TITLE1, "state", "hide");

	GUI_SetProperty(IMG_ID3_BACKGROUND, "state", "hide");
	return 0;
}

int id3_start(void)
{
	printf("[ID3] id3_start\n");

	GUI_SetProperty(TEXT_ID3_ARTIST1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_ALBUM1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_YEAR1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_GENRE1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_TRACK1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_TITLE1, "string", NULL);

	GxPlayer_MediaFreeID3Info(s_id3_info);
	if(s_id3_info != NULL)
	{
		//GxCore_Free(s_id3_info);
		s_id3_info = NULL;
	}
	s_id3_info = play_music_get_id3_info();
	if(NULL == s_id3_info) return 1;
#if 0
	if((NULL == s_id3_info->v1) && (NULL == s_id3_info->v2)) 
	{
		GxPlayer_MediaFreeID3Info(s_id3_info);
		//GxCore_Free(s_id3_info);
		s_id3_info = NULL;
		return 1;
	}
	if(NULL != s_id3_info->v1)
	{
		if(strlen((s_id3_info->v1)->Title))
			GUI_SetProperty(TEXT_ID3_TITLE1, "string", (s_id3_info->v1)->Title);
		else if(NULL != s_id3_info->v2)
		{
			char Buffer[ID3V2_INFO_LENGTH] ;
			memset(Buffer, 0, ID3V2_INFO_LENGTH);
			if((_parse_id3v2(ID3V2_TITLE,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
				GUI_SetProperty(TEXT_ID3_TITLE1, "string", Buffer);
			else
				GUI_SetProperty(TEXT_ID3_TITLE1, "string", "Unknow");
		}
		else
			GUI_SetProperty(TEXT_ID3_TITLE1, "string", "Unknow");
		
		if(strlen((s_id3_info->v1)->Artist))	
			GUI_SetProperty(TEXT_ID3_ARTIST1, "string", (s_id3_info->v1)->Artist);
		else if(NULL != s_id3_info->v2)
		{
			char Buffer[ID3V2_INFO_LENGTH] ;
			memset(Buffer, 0, ID3V2_INFO_LENGTH);
			if((_parse_id3v2(ID3V2_ARTIST,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
				GUI_SetProperty(TEXT_ID3_ARTIST1, "string", Buffer);
			else
				GUI_SetProperty(TEXT_ID3_ARTIST1, "string", "Unknow");
		}
		else
			GUI_SetProperty(TEXT_ID3_ARTIST1, "string", "Unknow");
		
		if(strlen((s_id3_info->v1)->Album))
			GUI_SetProperty(TEXT_ID3_ALBUM1, "string", (s_id3_info->v1)->Album);
		else if(NULL != s_id3_info->v2)
		{
			char Buffer[ID3V2_INFO_LENGTH] ;
			memset(Buffer, 0, ID3V2_INFO_LENGTH);
			if((_parse_id3v2(ID3V2_ALBUM,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
				GUI_SetProperty(TEXT_ID3_ALBUM1, "string", Buffer);
			else
				GUI_SetProperty(TEXT_ID3_ALBUM1, "string", "Unknow");
		}
		else
			GUI_SetProperty(TEXT_ID3_ALBUM1, "string", "Unknow");
		
		if(strlen((s_id3_info->v1)->Year))
			GUI_SetProperty(TEXT_ID3_YEAR1, "string", (s_id3_info->v1)->Year);
		else if(NULL != s_id3_info->v2)
		{
			char Buffer[ID3V2_INFO_LENGTH] ;
			memset(Buffer, 0, ID3V2_INFO_LENGTH);
			if((_parse_id3v2(ID3V2_YEAR,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
				GUI_SetProperty(TEXT_ID3_YEAR1, "string", Buffer);
			else
				GUI_SetProperty(TEXT_ID3_YEAR1, "string", "Unknow");
		}
		else
			GUI_SetProperty(TEXT_ID3_YEAR1, "string", "Unknow");
		
		if(strlen((s_id3_info->v1)->Genre)) 
		{
			extern char *ID3V1Genre[];
			if((s_id3_info->v1)->Genre[0] < ID3V1_GENRE_TYPE_NUM )
				GUI_SetProperty(TEXT_ID3_GENRE1, "string", ID3V1Genre[(int)((s_id3_info->v1)->Genre[0])]);
			else
				GUI_SetProperty(TEXT_ID3_GENRE1, "string", "Unknow");
		}
		else if(NULL != s_id3_info->v2)
		{
			char Buffer[ID3V2_INFO_LENGTH] ;
			memset(Buffer, 0, ID3V2_INFO_LENGTH);
			if((_parse_id3v2(ID3V2_GENRE,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
				GUI_SetProperty(TEXT_ID3_GENRE1, "string", Buffer);
			else
				GUI_SetProperty(TEXT_ID3_GENRE1, "string", "Unknow");
		}
		else
			GUI_SetProperty(TEXT_ID3_GENRE1, "string", "Unknow");
		
		if(strlen((s_id3_info->v1)->Track))
			GUI_SetProperty(TEXT_ID3_TRACK1, "string", (s_id3_info->v1)->Track);
		else if(NULL != s_id3_info->v2)
		{
			char Buffer[ID3V2_INFO_LENGTH] ;
			memset(Buffer, 0, ID3V2_INFO_LENGTH);
			if((_parse_id3v2(ID3V2_TRACK,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
				GUI_SetProperty(TEXT_ID3_TRACK1, "string", Buffer);
			else
				GUI_SetProperty(TEXT_ID3_TRACK1, "string", "Unknow");
		}
		else
			GUI_SetProperty(TEXT_ID3_TRACK1, "string", "Unknow");
	}
	else if(NULL != s_id3_info->v2)
	{
		char Buffer[ID3V2_INFO_LENGTH] ;
		memset(Buffer, 0, ID3V2_INFO_LENGTH);
		if((_parse_id3v2(ID3V2_TITLE,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
			GUI_SetProperty(TEXT_ID3_TITLE1, "string", Buffer);
		else
			GUI_SetProperty(TEXT_ID3_TITLE1, "string", "Unknow");
		memset(Buffer, 0, ID3V2_INFO_LENGTH);
		if((_parse_id3v2(ID3V2_ARTIST,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
			GUI_SetProperty(TEXT_ID3_ARTIST1, "string", Buffer);
		else
			GUI_SetProperty(TEXT_ID3_ARTIST1, "string", "Unknow");
		memset(Buffer, 0, ID3V2_INFO_LENGTH);
		if((_parse_id3v2(ID3V2_ALBUM,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
			GUI_SetProperty(TEXT_ID3_ALBUM1, "string", Buffer);
		else
			GUI_SetProperty(TEXT_ID3_ALBUM1, "string", "Unknow");
		memset(Buffer, 0, ID3V2_INFO_LENGTH);
		if((_parse_id3v2(ID3V2_YEAR,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
			GUI_SetProperty(TEXT_ID3_YEAR1, "string", Buffer);
		else
			GUI_SetProperty(TEXT_ID3_YEAR1, "string", "Unknow");
		memset(Buffer, 0, ID3V2_INFO_LENGTH);
		if((_parse_id3v2(ID3V2_TRACK,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
			GUI_SetProperty(TEXT_ID3_TRACK1, "string", Buffer);
		else
			GUI_SetProperty(TEXT_ID3_TRACK1, "string", "Unknow");
		memset(Buffer, 0, ID3V2_INFO_LENGTH);
		if((_parse_id3v2(ID3V2_GENRE,s_id3_info->v2,Buffer,ID3V2_INFO_LENGTH)) > 0)
			GUI_SetProperty(TEXT_ID3_GENRE1, "string", Buffer);
		else
			GUI_SetProperty(TEXT_ID3_GENRE1, "string", "Unknow");
	}

#else
	if(NULL == s_id3_info->v1)
	{
		GxPlayer_MediaFreeID3Info(s_id3_info);
		//free(s_id3_info);
		s_id3_info = NULL;
		return 1;
	}
	
	GUI_SetProperty(TEXT_ID3_ARTIST1, "string", (s_id3_info->v1)->Artist);
	GUI_SetProperty(TEXT_ID3_ALBUM1, "string", (s_id3_info->v1)->Album);
	GUI_SetProperty(TEXT_ID3_YEAR1, "string", (s_id3_info->v1)->Year);
	GUI_SetProperty(TEXT_ID3_GENRE1, "string", (s_id3_info->v1)->Genre);
	GUI_SetProperty(TEXT_ID3_TRACK1, "string", (s_id3_info->v1)->Track);
	GUI_SetProperty(TEXT_ID3_TITLE1, "string", (s_id3_info->v1)->Title);
#endif
	return 0;
}

int id3_stop(void)
{
	printf("[ID3] id3_stop\n");
	GxPlayer_MediaFreeID3Info(s_id3_info);
	if(s_id3_info != NULL)
	{
		//GxCore_Free(s_id3_info);
		s_id3_info = NULL;
	}
	GUI_SetProperty(TEXT_ID3_ARTIST1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_ALBUM1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_YEAR1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_GENRE1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_TRACK1, "string", NULL);
	GUI_SetProperty(TEXT_ID3_TITLE1, "string", NULL);

	return 0;
}

int id3_pause(void)
{
	printf("[ID3] id3_pause\n");

	return 0;
}

int id3_resume(void)
{
	printf("[ID3] id3_resume\n");

	return 0;
}



