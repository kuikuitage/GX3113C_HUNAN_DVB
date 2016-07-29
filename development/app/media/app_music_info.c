#include "app.h"
#include "play_music.h"

#define ID3_ARTIST     "music_info_text94"
#define ID3_ALBUM     "music_info_text95"
#define ID3_YEAR     "music_info_text96"
#define ID3_GENRE     "music_info_text97"
#define ID3_TRACK     "music_info_text98"
#define ID3_TITLE       "music_info_text99"
#if 0
char *ID3V2KeyWord[ID3V2_TOTAL] = 
{
	"TIT2",
	"TPE1",
	"TALB",
	"TYER",
	"TRCK",
	"TCON",
};

char *ID3V1Genre[ID3V1_GENRE_TYPE_NUM] =
{
	"blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop", "Jazz", "Metal", // 0~9
	"New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno", "Industrial",// 10~19
	"Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",//20~29
	"Fusion", "Trance", "Classical", "Instrumental", "Acid", "House", "Game", "Sound Clip", "Gospel", "Noise", //30~39
	"AlternRock", "Bass", "Soul", "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock", "Ethnic", "Gothic", //40~49
	"Darkware", "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",//50~59
	"Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret", "New Wave", "Psychadlic", "Rave", "Showtunes",//60~69
	"Trailer", "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock",// 70~79
	"Folk", "Folk/Rock", "National Folk", "Swing", "Fast-Fusion", "Bebob", "Latin", "Revival", "Celtic", "Bluegrass", //80~89
	"Advantgarde", "Gothic Rock", "Progressive Rock", "Psychadelic Rock", "Symphonic Rock", "Slow Rock", "Big Band", "Chorus", "Easy Listening", "Acoustic", // 90~99
	"Humour", "Speech", "Chanson", "Opera", "Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus", "Porn Groove", // 100~109
	"Satire", "Slow Jam", "Club", "Tango", "Samba", "Folklore", // 110~115
};
#endif

static PlayerID3Info *s_id3_info = NULL;
#if 0
//extern int gxgdi_iconv(const unsigned char *from_str,unsigned char **to_str,unsigned int from_size,unsigned int *to_size);

int  _parse_id3v2(ID3V2Type_e Type, PlayerID3V2* pRawData, char * pOutData, unsigned int MaxLenToOut)
{
	struct   id3_v2 *pTemp = NULL;
	char *pStr = NULL;
	char *pStrTemp = NULL;
	unsigned int Size = 0; 
	unsigned int SizeTemp = 0;
	
	if((pRawData == NULL) || (Type >= ID3V2_TOTAL) || (pOutData == NULL))
	{
		printf("\nMUSIC, parameter error... %s, %d\n",__FUNCTION__,__LINE__);
		return -1; 
	}

	pTemp = pRawData->pNext;
	while(pTemp)
	{
		if(0 == memcmp(ID3V2KeyWord[Type],pTemp->fName, strlen(ID3V2KeyWord[Type])))
		{
		/*
			for(i=0; i<pTemp->size; i++)
			{
				if((pTemp->pFBody[i]<0x1f) || (pTemp->pFBody[i]>0x20))
				{
					break;
				}
			}
		*/
			SizeTemp = pTemp->size;
			pStrTemp = GxCore_Malloc(pTemp->size + 1);
			memset(pStrTemp, 0, pTemp->size + 1);
			if(NULL == pStrTemp)
			{
				printf("\nMUSIC, not enough memory\n");
				break;
			}
			if(pTemp->pFBody[0] == 0x01)
			{// unicode
				if((pTemp->pFBody[1] == 0xff)
					&& (pTemp->pFBody[2] == 0xfe)
					&& (pTemp->size > 3))
				{
					pStrTemp[0] = 0x11;
					if(pTemp->pFBody[3] != 0x00) //数据大小端模式：大端，需要转成小�?
					{
						memcpy(pStrTemp+2, pTemp->pFBody+3, ((pTemp->size-3) -1));
						SizeTemp -= 3;
					}
					else
					{
						memcpy(pStrTemp+1, pTemp->pFBody+3, pTemp->size - 3);
						SizeTemp -= 2;
					}
				}
				else
				{
					memcpy(pStrTemp, pTemp->pFBody, pTemp->size);
				}
			}
			else
			{//8859-1
				memcpy(pStrTemp, pTemp->pFBody, pTemp->size);
			}
			
				
			if((gxgdi_iconv((const unsigned char *)&(pStrTemp[0]),(unsigned char**)&pStr,SizeTemp,&Size,NULL) == 0) &&(Size > 0))
			{
				if(Size >= MaxLenToOut)
				{
					Size = MaxLenToOut;
					printf("\nMUSIC, Length is overflow...%s,%d\n",__FUNCTION__,__LINE__);
				}
				memcpy(pOutData,pStr,Size);
				GxCore_Free(pStr);
				GxCore_Free(pStrTemp);
				return Size;
			}
			else
			{
				GxCore_Free(pStrTemp);
			}
		}
		pTemp = pTemp->pNext;
	}
	return  -1;	
}
#endif

void music_info_get(MediaInfo *info)
{
	GxPlayer_MediaFreeID3Info(s_id3_info);
	if(s_id3_info != NULL)
	{
		//GxCore_Free(s_id3_info);
		s_id3_info = NULL;
	}

	info->line[0].subt = "Title";
	info->line[1].subt = "Artist";
	info->line[2].subt = "Album";
	info->line[3].subt = "Year";
	info->line[4].subt = "Genre";
	info->line[5].subt = "Track";
	
	s_id3_info = play_music_get_id3_info();
	if(NULL == s_id3_info) return;
#if 0
	if((NULL == s_id3_info->v1) && (NULL == s_id3_info->v2)) 
	{
		GxPlayer_MediaFreeID3Info(s_id3_info);
		//GxCore_Free(s_id3_info);
		s_id3_info = NULL;
		return ;
	}
	
	info->line[0].subt = "Title:";
	info->line[1].subt = "Artist:";
	info->line[2].subt = "Album:";
	info->line[3].subt = "Year:";
	info->line[4].subt = "Genre:";
	info->line[5].subt = "Track:";

	static char buf_art[ID3V2_INFO_LENGTH] = {0};
	static char buf_alb[ID3V2_INFO_LENGTH] = {0};
	static char buf_yea[ID3V2_INFO_LENGTH] = {0};
	static char buf_gen[ID3V2_INFO_LENGTH] = {0};
	static char buf_tra[ID3V2_INFO_LENGTH] = {0};
	static char buf_tit[ID3V2_INFO_LENGTH] = {0};

	memset(buf_art, 0, sizeof(buf_art));
	memset(buf_alb, 0, sizeof(buf_alb));
	memset(buf_yea, 0, sizeof(buf_yea));
	memset(buf_gen, 0, sizeof(buf_gen));
	memset(buf_tra, 0, sizeof(buf_tra));
	memset(buf_tit, 0, sizeof(buf_tit));

	if(NULL != s_id3_info->v1)
	{
		if(strlen((s_id3_info->v1)->Title))
			info->line[0].info = (s_id3_info->v1)->Title;
		else if(NULL != s_id3_info->v2)
		{
			if((_parse_id3v2(ID3V2_TITLE,s_id3_info->v2,buf_tit,ID3V2_INFO_LENGTH)) > 0)
				info->line[0].info = buf_tit;
		}
		
		if(strlen((s_id3_info->v1)->Artist))	
			info->line[1].info = (s_id3_info->v1)->Artist;
		else if(NULL != s_id3_info->v2)
		{
			if((_parse_id3v2(ID3V2_ARTIST,s_id3_info->v2,buf_art,ID3V2_INFO_LENGTH)) > 0)
				info->line[1].info = buf_art;
		}
		
		if(strlen((s_id3_info->v1)->Album))
			info->line[2].info = (s_id3_info->v1)->Album;
		else if(NULL != s_id3_info->v2)
		{
			if((_parse_id3v2(ID3V2_ALBUM,s_id3_info->v2,buf_alb,ID3V2_INFO_LENGTH)) > 0)
				info->line[2].info = buf_alb;
		}
		
		if(strlen((s_id3_info->v1)->Year))
			info->line[3].info = (s_id3_info->v1)->Year;
		else if(NULL != s_id3_info->v2)
		{
			if((_parse_id3v2(ID3V2_YEAR,s_id3_info->v2,buf_yea,ID3V2_INFO_LENGTH)) > 0)
				info->line[3].info = buf_yea;
		}
		
		if(strlen((s_id3_info->v1)->Genre)) 
		{
			if((s_id3_info->v1)->Genre[0] < ID3V1_GENRE_TYPE_NUM )
				info->line[4].info = ID3V1Genre[(int)((s_id3_info->v1)->Genre[0])];
		}
		else if(NULL != s_id3_info->v2)
		{
			if((_parse_id3v2(ID3V2_GENRE,s_id3_info->v2,buf_gen,ID3V2_INFO_LENGTH)) > 0)
				info->line[4].info = buf_gen;
		}
		
		if(strlen((s_id3_info->v1)->Track))
			info->line[5].info = (s_id3_info->v1)->Track;
		else if(NULL != s_id3_info->v2)
		{
			if((_parse_id3v2(ID3V2_TRACK,s_id3_info->v2,buf_tra,ID3V2_INFO_LENGTH)) > 0)
				info->line[5].info = buf_tra;
		}
	}
	else if(NULL != s_id3_info->v2)
	{
		if((_parse_id3v2(ID3V2_TITLE,s_id3_info->v2,buf_tit,ID3V2_INFO_LENGTH)) > 0)
			info->line[0].info = buf_tit;

		if((_parse_id3v2(ID3V2_ARTIST,s_id3_info->v2,buf_art,ID3V2_INFO_LENGTH)) > 0)
			info->line[1].info = buf_art;

		if((_parse_id3v2(ID3V2_ALBUM,s_id3_info->v2,buf_alb,ID3V2_INFO_LENGTH)) > 0)
			info->line[2].info = buf_alb;

		if((_parse_id3v2(ID3V2_YEAR,s_id3_info->v2,buf_yea,ID3V2_INFO_LENGTH)) > 0)
			info->line[3].info = buf_yea;

		if((_parse_id3v2(ID3V2_GENRE,s_id3_info->v2,buf_gen,ID3V2_INFO_LENGTH)) > 0)
			info->line[4].info = buf_gen;

		if((_parse_id3v2(ID3V2_TRACK,s_id3_info->v2,buf_tra,ID3V2_INFO_LENGTH)) > 0)
			info->line[5].info = buf_tra;
	}
#else
	if(NULL == s_id3_info->v1) 
	{
		GxPlayer_MediaFreeID3Info(s_id3_info);
		
		s_id3_info = NULL;
		return ;
	}	
	
	if (strlen(s_id3_info->v1->Title))
	{
		info->line[0].info = s_id3_info->v1->Title;
	}

	if (strlen(s_id3_info->v1->Artist))
	{
		info->line[1].info = s_id3_info->v1->Artist;
	}

	if (strlen(s_id3_info->v1->Album))
	{
		info->line[2].info = s_id3_info->v1->Album;
	}

	if (strlen(s_id3_info->v1->Year))
	{
		info->line[3].info = s_id3_info->v1->Year;
	}

	if (strlen(s_id3_info->v1->Genre))
	{
		info->line[4].info = s_id3_info->v1->Genre;
	}

	if (strlen(s_id3_info->v1->Track))
	{
		info->line[5].info = s_id3_info->v1->Track;
	}

#endif
}

void music_info_destroy(void)
{
	GxPlayer_MediaFreeID3Info(s_id3_info);
	if(s_id3_info != NULL)
	{
		//GxCore_Free(s_id3_info);
		s_id3_info = NULL;
	}
}
