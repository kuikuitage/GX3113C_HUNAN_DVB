#ifndef ADVERPARSE_C
#define ADVERPARSE_C

//#define STTBX_Print

//#include "sttbx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <debug.h> 
#include <assert.h>
#include "adver.h"
#include "adverparse.h"
//#include "adverdis.hpp"
//#include "adverGIF.h"
//#include "adver_init.h"
#include "app_by_ads_porting_stb_api.h"

/*---------------------------------------------------------------------------*/
static AdU32 bAdverCRC32 = 0;
static AdU16 bgAdverScrollID = 0;
static AdverScroll_ProgInfo gScroll_ProgInfo;
AdverOpenPicture_Info gSaveAdOpenPic_Info;
#define CRC32_MAX_COEFFICIENTS			256

AdU32  CRC32_table[CRC32_MAX_COEFFICIENTS]=
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
	0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
	0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
	0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
	0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
	0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
	0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
	0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
	0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
	0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
	0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
	0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
	0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
	0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
	0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
	0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
	0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
	0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};
/*---------------------------------------------------------------------------*/
AdU8 AdverDescriptor(AdU8 *pData,AdU16 bLength,AdverCorner_Info *pAdCorner_Info,
	AdverScroll_Info *pAdScr_Info,AdverOpenPicture_Info *pAdOpenPic,AdU8 *pbPlayer);
AdU8 AdverScroll_prog_Des(AdU8 *pData,AdU8 bLength,AdU8 *pbPlayer);	
AdU8 AdverScroll_Descriptor(AdU8 *pData,AdU8 bLength,AdverScroll_Info *pAdverScrInfo);
AdU8 AdverCornerDescriptor(AdU8 *pData,AdU16 bLength,AdverCorner_Info *pAdverC_Info);
AdU8 CheckAdverFull(AdU8 bSecNum,AdU8 bLastSecNum,AdU8 bType);
void CRC32Calculate(AdU8 *buffer,AdU32 size,AdU32 *CRC32);
/*---------------------------------------------------------------------------*/
void AdverPsiParse(AdU8 *pAdverData,AdU32 AdverLen)
{
  AdU16 iSectionLen = 0;
  AdU8 *pSectData = pAdverData;
  AdU8 bVerNum = 0,bSaveFlag = 0,bAD_Type = 0,bNeedDisplayAdCorner = false,bNeedDisplayAdScroll = false;
  AdU8 bSectionNum = 0,bSectionLastNum = 0;
  AdU8 badScrollTableVer = 0,bValid = false;
  AdU16  iOffset = 0,iCount = 0;
  AdU16 uiadScroolId = 0,uiadDes_Loop_Len = 0;
  AdU32	 ulDataLength = 0,TableCRC32 = 0,DataCRC32 = 0,FlashCRC32 = 0;
  AdverCorner_Info  mAdverCornerInfo;
  AdverScroll_Info  mAdScr_Info;
  AdU8 bNeedDisplayAdOpenPic = false;

/*	
	printf("\n ------------- AdverPsiParse -----0x%x------ \n",pAdverData[0]);
	*/
	
	if(pAdverData[0] != ADVER_TABKE_ID)
 	{
		return;
	}

	iSectionLen 	= ((AdU16)(pSectData[1]&0x0F)*256)|(AdU16)(pSectData[2]);
	bVerNum 			= pSectData[5]>>1;
	bSaveFlag 		= pSectData[5]&0x80;
  	bSectionNum 	= pSectData[6]; 
  	bSectionLastNum = pSectData[7]; 
  	TableCRC32 =  (((AdU32)pSectData[AdverLen-4])<< 24) + (((AdU32)pSectData[AdverLen-3])<< 16)+
  							(((AdU32)pSectData[AdverLen-2])<< 8)+pSectData[AdverLen-1];
 	
 	
  //if(bAdverCRC32  == TableCRC32)
  //return;

//  STTBX_Print(("\n ------------- AdverPsiParse ---0x%x------ 0x%x 0x%x\n",pAdverData[0],bAdverCRC32,TableCRC32));
  bAdverCRC32 = TableCRC32;
  badScrollTableVer = pSectData[8];
    
  iOffset += 9+4;
  pSectData += 9+4;
  uiadScroolId = ((AdU16)pSectData[0]<< 8) + (AdU16)pSectData[1];
  if(bgAdverScrollID == uiadScroolId)
  {
//  	printf("bgAdverScrollID=%d,uiadScroolId=%d\n",bgAdverScrollID,uiadScroolId);
  	return;
  }
 // printf("----- TableCRC32 0x%x 0x%x 0x%x  %x  %x \n",TableCRC32,bSectionNum,bSectionLastNum,uiadScroolId,bgAdverScrollID);
   
//  semaphore_wait(gpAdverDataSem);
  
  iOffset +=2;
  pSectData += 2; 
  bAD_Type = pSectData[0];
  uiadDes_Loop_Len = ((AdU16)pSectData[1]<< 8) + (AdU16)pSectData[2];
  
  iOffset +=3;
  pSectData += 3; 
  
 //STTBX_Print((" ---------- uiadDes_Loop_Len 0x%x 0x%x 0x%x 0x%x \n",uiadDes_Loop_Len,bAD_Type,uiadScroolId,bgAdverScrollID));
  if(uiadDes_Loop_Len > 0)
  AdverDescriptor(pSectData,uiadDes_Loop_Len,&mAdverCornerInfo,&mAdScr_Info,&gSaveAdOpenPic_Info,&bValid);
  
  iOffset += uiadDes_Loop_Len+2;
  pSectData += uiadDes_Loop_Len+2;//reser
  
  ulDataLength = (((AdU32)pSectData[0])<< 24) | (((AdU32)pSectData[1])<< 16)|(((AdU32)pSectData[2])<< 8)|pSectData[3];
  
  if(ulDataLength > ADVER_MAXFILE)
  {
  	printf(" file too large to malloc \n");
//		semaphore_signal(gpAdverDataSem);
		return;
  }
  iOffset += 4;
  pSectData += 4;
  
  DataCRC32 = (((AdU32)pSectData[0])<< 24) | (((AdU32)pSectData[1])<< 16)|(((AdU32)pSectData[2])<< 8)|pSectData[3];
 
  if((bAD_Type == OPEN_VIDEO_PICTRUE) && (bValid == 1))
  {	

			 #if 1
			 LoaderCheckPictureCRC32(&FlashCRC32);
			 //STTBX_Print(("\n bValid %d %d  DataCRC32 0x%x FlashCRC32 0x%x \n",bAD_Type,bValid,DataCRC32,FlashCRC32));	 
			  if(FlashCRC32 != DataCRC32)	
			  {
//		         printf(" FlashCRC32 0x%x gSaveAdOpenPic_Info.mad_CRC32 0x%x DataCRC32 0x%x %d \n",FlashCRC32,gSaveAdOpenPic_Info.mad_CRC32,DataCRC32,bValid);
					 if(DataCRC32 != gSaveAdOpenPic_Info.mad_CRC32)
					{
					  	gSaveAdOpenPic_Info.mad_TotleLen = ulDataLength;
						gSaveAdOpenPic_Info.mad_CRC32 = DataCRC32;
						
							for(iCount = 0;iCount < GIF_BUFFER_FULLFLAG;iCount++)
							{
								gSaveAdOpenPic_Info.mad_DataFullFlag[iCount] = false;
							}
							if(gSaveAdOpenPic_Info.mad_Data != AdNULL)
							free(gSaveAdOpenPic_Info.mad_Data);
							
							gSaveAdOpenPic_Info.mad_Data = (AdU8 *)malloc((AdInt)ulDataLength);
							
							if(gSaveAdOpenPic_Info.mad_Data == AdNULL)
							assert(0);	

							bNeedDisplayAdOpenPic = false;
					  }
				  	if(gSaveAdOpenPic_Info.mad_DataFullFlag[bSectionNum] == false)
					{
						gSaveAdOpenPic_Info.mad_DataFullFlag[bSectionNum] = true;
						bNeedDisplayAdOpenPic = true;
					}
			  	}
			#endif	
  }
 
  if(bValid == true)
  {
  	if(bAD_Type == CORNER_PICTURE)
  	{
  		#if 1	
  	       if(gSaveAdverCorner_Info.mad_CRC32 != DataCRC32)
  	      {
						gSaveAdverCorner_Info.mad_CRC32 = DataCRC32;
						gSaveAdverCorner_Info.mad_TotleLen = ulDataLength;
						for(iCount = 0;iCount < GIF_BUFFER_FULLFLAG;iCount++)
						{
						gSaveAdverCorner_Info.mad_DataFullFlag[iCount] = false;
						}

						if(gSaveAdverCorner_Info.mad_Data != AdNULL)
						free(gSaveAdverCorner_Info.mad_Data);
						gSaveAdverCorner_Info.mad_Data = (AdU8 *)malloc((AdInt)ulDataLength);
						if(gSaveAdverCorner_Info.mad_Data == AdNULL)
						assert(0);	
  	       }
  	       
	       if(gSaveAdverCorner_Info.mad_DataFullFlag[bSectionNum] == false)
				{
					gSaveAdverCorner_Info.mad_DataFullFlag[bSectionNum] = true;
					bNeedDisplayAdCorner = true;
				}
		  #endif 
  	     
  	}else if(bAD_Type == FONT_MOVE_PICTURE)
  	{
  		//printf("\n ========= FONT_MOVE_PICTURE  %d  0x%x DataCRC32 0x%x bSectionNum %d \n",mAdScr_Info.mad_TextOrBmp,
		//	gSaveAdverScroll_Info.mad_CRC32,DataCRC32,bSectionNum);
		
  		//if(mAdScr_Info.mad_TextOrBmp != 0)
  		{
	  		if(gSaveAdverScroll_Info.mad_CRC32 != DataCRC32)
	  		{
					gSaveAdverScroll_Info.mad_CRC32 = DataCRC32;
					gSaveAdverScroll_Info.mad_TotleLen = ulDataLength;
					for(iCount = 0;iCount < SCROLL_BUFFER_FULLFLAG;iCount++)
					{
						gSaveAdverScroll_Info.mad_DataFullFlag[iCount] = false;
					}
					if(gSaveAdverScroll_Info.mad_Data != AdNULL)
					free(gSaveAdverScroll_Info.mad_Data);
					gSaveAdverScroll_Info.mad_Data = (AdU8 *)malloc((AdInt)ulDataLength);
					if(gSaveAdverScroll_Info.mad_Data == AdNULL)
					assert(0);				
	  		}else
	  		{
	  			/*STTBX_Print(("\n FONT_MOVE_PICTURE %d   DataCRC32  0x%x \n",bAD_Type,DataCRC32));	*/
	  			if(CheckAdverFull(bSectionNum,bSectionLastNum,bAD_Type) == true)
	  			{
	  			   if(GetAdverCurrentDisplayState()==false)
	  			   {
	  			   	 AdverDisplayText_Start(&gSaveAdverScroll_Info);
	  			   }
	  				
	  			}
	  		}
			
			//printf("\n gSaveAdverScroll_Info.mad_DataFullFlag[bSectionNum]  %d  %d \n",gSaveAdverScroll_Info.mad_DataFullFlag[bSectionNum] ,bSectionNum);
				if(gSaveAdverScroll_Info.mad_DataFullFlag[bSectionNum] == false)
				{
					gSaveAdverScroll_Info.mad_DataFullFlag[bSectionNum] = true;	
					bNeedDisplayAdScroll = true;
				}
			}
		
  	}
  }
   
  iOffset += 4;
  pSectData += 4;
  
  iSectionLen -= (iOffset - 3);// - header 3 byte
  iSectionLen -=  4; //CRC32
  iCount = 0;
  //printf(" 0x%x  %d  %d iSectionLen  %d bAD_Type 0x%x \n",pSectData[0],bSectionNum,bSectionLastNum,iSectionLen,bAD_Type);
 
  if(iSectionLen > 0)
  {
		if(iSectionLen > DataBuff_Max_Number)
		iSectionLen = DataBuff_Max_Number ;
		if(bNeedDisplayAdCorner == true)
		{	
			memcpy((void *)(gSaveAdverCorner_Info.mad_Data+(bSectionNum*DataBuff_Max_Number)),(void*)pSectData,iSectionLen);
		}
		if(bNeedDisplayAdScroll == true)
		{
			memcpy((void *)(gSaveAdverScroll_Info.mad_Data+(bSectionNum*DataBuff_Max_Number)),(void*)pSectData,iSectionLen);
		}
		if(bNeedDisplayAdOpenPic == true)
		{
			memcpy((void *)(gSaveAdOpenPic_Info.mad_Data+(bSectionNum*DataBuff_Max_Number)),(void*)pSectData,iSectionLen);
		}
		
  }
  if((bAD_Type == OPEN_VIDEO_PICTRUE) && (bValid == 1))
  {
  
  	if(CheckAdverFull(bSectionNum,bSectionLastNum,bAD_Type) == true)
  	{
  		AdU32 CRC32 = 0;
  		
//		 WDF_DogSet(1);
  		LoaderCheckPictureCRC32(&FlashCRC32);
		 //STTBX_Print(("\n OPEN_VIDEO_PICTRUE CheckAdverFull  %d %d FlashCRC32 %x  DataCRC32 %x  \n",bSectionNum,bSectionLastNum,FlashCRC32,DataCRC32));
		if(DataCRC32!= FlashCRC32)
		{
			CRC32Calculate(gSaveAdOpenPic_Info.mad_Data,gSaveAdOpenPic_Info.mad_TotleLen,&CRC32);
//			STTBX_Print(("\n ========= CRC32 0x%x FlashCRC32 0x%x 0x%x \n",CRC32,FlashCRC32,DataCRC32));
			if(FlashCRC32 != CRC32)	
			{
//	  			STTBX_Print(("\n ========= receive Open picture Over \n"));	
				LoaderVideoCheckWrite(&gSaveAdOpenPic_Info);
			}
		}
  	}
  }
   if(bValid == true)
  {
  		
  	  if(bAD_Type == CORNER_PICTURE)
  	  {
			 #if 1
			  if(CheckAdverFull(bSectionNum,bSectionLastNum,bAD_Type) == true)
			  {
//			  	AdU32 CRC32 = 0; 
   				
				   memset((void *)&gSaveAdverCorner_Info.mad_CornerInfo,0,sizeof(AdverCorner_Info));
		  		 memcpy((void *)&gSaveAdverCorner_Info.mad_CornerInfo,(void *)&mAdverCornerInfo,sizeof(AdverCorner_Info));
					gSaveAdverCorner_Info.mad_Type = bAD_Type;
					
					//CRC32Calculate(gSaveAdverCorner_Info.mad_Data,gSaveAdverCorner_Info.mad_TotleLen,&CRC32);

					if(/*CRC32 == gSaveAdverCorner_Info.mad_CRC32*/1)
					{						
						bgAdverScrollID = uiadScroolId;
						if( gSaveAdverCorner_Info.mad_CornerInfo.mad_PicType == 1)
						{
							AdverDisplayCornerBMP_Start(&gSaveAdverCorner_Info);
						}
						else if( gSaveAdverCorner_Info.mad_CornerInfo.mad_PicType == 2)
						{
							
							AdverGifStart(&gSaveAdverCorner_Info);
						}
						
					}else
					{
						printf("\n ---------CRC32Calculate error ------gSaveAdverCorner_Info------------- \n");
					}
			  }
			  #endif
  	  }else if(bAD_Type == FONT_MOVE_PICTURE)
  	  {
  	  	// STTBX_Print(("\n CheckAdverFull    %d  %d %d tt %d\n",bSectionNum,bSectionLastNum,bAD_Type,gSaveAdverScroll_Info.mad_DataFullFlag[0])); 
  	  	if(CheckAdverFull(bSectionNum,bSectionLastNum,bAD_Type) == true)
  	  	{
  	  		AdU32 CRC32 = 0;	 
//  	  		STTBX_Print(("\n       CheckAdverFull    full \n")); 	
		  		memset((void *)&gSaveAdverScroll_Info.mad_ScrollInfo,0,sizeof(AdverScroll_Info));
		  		memcpy((void *)&gSaveAdverScroll_Info.mad_ScrollInfo,(void *)&mAdScr_Info,sizeof(AdverScroll_Info));
		  		gSaveAdverScroll_Info.mad_Type = bAD_Type;		      
		  		
		      if(gSaveAdverScroll_Info.mad_ScrollInfo.mad_TextOrBmp == 0)
		      {
//		      	STTBX_Print((" ##### CheckAdScrollFull ok %d 0x%x \n",gSaveAdverScroll_Info.mad_TotleLen,gSaveAdverScroll_Info.mad_Data[0]));
		      	
		      }else
		      {
		     		
						CRC32Calculate(gSaveAdverScroll_Info.mad_Data,gSaveAdverScroll_Info.mad_TotleLen,&CRC32);					
						if(/*CRC32 == gSaveAdverScroll_Info.mad_CRC32*/1)
						{				
//							STTBX_Print(("\n----0x%x------ CRC32Calculate ok ----------0x%x \n",CRC32,gSaveAdverScroll_Info.mad_CRC32));		
							if(GetAdverCurrentDisplayState()==false)
							{
								bgAdverScrollID = uiadScroolId;
								//printf("\n FONT_MOVE_PICTURE222222222 \n");
								AdverDisplayText_Start(&gSaveAdverScroll_Info);
							}
						}else
						{
							//printf("\n ######## CRC32Calculate error 0x%x 0x%x ********\n",CRC32,gSaveAdverScroll_Info.mad_CRC32);
							memset(&gSaveAdverScroll_Info,0x00,sizeof(SaveAdverScroll_Info));
						}
					}
  	  	}else
  	  	{
  	  		//printf("\n       CheckAdverFull    error \n");
  	  	}
  	  }
  }

//  semaphore_signal(gpAdverDataSem);
 
}
/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
AdU8 AdverScroll_prog_Des(AdU8 *pData,AdU8 bLength,AdU8 *pbPlayer)
{
	AdU8 	i = 0,j = 0,k = 0,iLen = 0;
	AdU8 	*pProgData = pData;
	AdU8  *pBuf = AdNULL;
	iLen = pProgData[1];

	if(iLen <= 2)
	return false;
	
	pBuf = (unsigned char *)malloc((AdInt)iLen*8);
	
	if(pProgData[0] == ADVER_SCROLL_PROGRAM_DESCRIPTOR)
	{
		pProgData += 2;
		for(i = 0;i < iLen;i++)
		{
			gScroll_ProgInfo.prog_data[i]  = pProgData[i];
			for(j = 0;j < 8;j++)
		  	{
				if((pProgData[0]&0x80) != 0)
				{
					pBuf[k] = 1;
				}else
				{
					pBuf[k] = 0;
				}
				pProgData[0] <<= 1;
				k++;
			}
			pProgData += 1;
		}
		if(gAdverProgramId > 0xff)//PMT  ProgId
		{
			*pbPlayer = false;
		}
		if(pBuf[gAdverProgramId] == true)
		{
			*pbPlayer = true;
		}else
		{
			*pbPlayer = false;
		}
		if(pBuf != AdNULL)
		free(pBuf);
		return true;
	}else
	{
		if(pBuf != AdNULL)
		free(pBuf);
		return false;
	}
}
/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

AdU8 AdverScroll_Descriptor(AdU8 *pData,AdU8 bLength,AdverScroll_Info *pAdverScrInfo)
{
	AdU8 *pDesData = pData,bOffset = 0;
	//	AdverScroll_Data bAdver_data;
		/*
		printf(" bLength %d \n",bLength);
		for(i = 0;i < bLength+2;i++)
		{
			printf(" 0x%x ",pData[i]);
		}
		printf("\n");
		*/
	/*	AdverLib_Scroll_Des(pData,bLength,&bAdver_data);*/
	/*	pAdverScrInfo->mad_BackgroundColor	= bAdver_data.ad_BackgroundColor;
		pAdverScrInfo->mad_EndxPos			= bAdver_data.ad_EndxPos;
		pAdverScrInfo->mad_EndyPos			= bAdver_data.ad_EndyPos;
		pAdverScrInfo->mad_FontColor		= bAdver_data.ad_FontColor;
		pAdverScrInfo->mad_MoveDir			= bAdver_data.ad_MoveDir;
		pAdverScrInfo->mad_Speed			= bAdver_data.ad_Speed;
		
		pAdverScrInfo->mad_StartxPos		= bAdver_data.ad_StartxPos;
		pAdverScrInfo->mad_StartyPos		= bAdver_data.ad_StartyPos;
		pAdverScrInfo->mad_TextOrBmp		= bAdver_data.ad_TextOrBmp;
		pAdverScrInfo->mad_Times			= bAdver_data.ad_Times;
		memset(pAdverScrInfo->mad_StartTime,0,5);
		memcpy(pAdverScrInfo->mad_StartTime,bAdver_data.ad_StartTime,5);*/
		pAdverScrInfo->mad_BackgroundColor	= 0;
		pAdverScrInfo->mad_EndxPos			= 0;
		pAdverScrInfo->mad_EndyPos			= 0;
		pAdverScrInfo->mad_FontColor		= 0;
		pAdverScrInfo->mad_MoveDir			= 0;
		pAdverScrInfo->mad_Speed			= 0;
		
		pAdverScrInfo->mad_StartxPos		= 0;
		pAdverScrInfo->mad_StartyPos		= 0;
		pAdverScrInfo->mad_TextOrBmp		= 0;
		pAdverScrInfo->mad_Times			= 0;
		memset(pAdverScrInfo->mad_StartTime,0,5);

	#if 1
	if(pDesData[0] == ADVER_SCROLL_DESCRIPTOR)
	{
		if(bLength > 0)
		{
			pDesData += 2;
			bOffset += 2;
			pAdverScrInfo->mad_Times = pDesData[0];
			pAdverScrInfo->mad_StartxPos = ((unsigned short)pDesData[1]<<8)+pDesData[2];
			pAdverScrInfo->mad_StartyPos = ((unsigned short)pDesData[3]<<8)+pDesData[4];
			pDesData += 5;
			bOffset += 5;
			pAdverScrInfo->mad_EndxPos		= ((unsigned short)pDesData[0]<<8)+pDesData[1];
			pAdverScrInfo->mad_EndyPos		= ((unsigned short)pDesData[2]<<8)+pDesData[3];
			pDesData += 4;
			bOffset += 4;
			pAdverScrInfo->mad_MoveDir = pDesData[0]; 
			pAdverScrInfo->mad_Speed	= pDesData[1];
			pDesData += 2;
			bOffset += 2;
			memset(pAdverScrInfo->mad_StartTime,0,5);
			memcpy(pAdverScrInfo->mad_StartTime,pDesData,5);
			pDesData += 5;
			bOffset += 5;
			pAdverScrInfo->mad_TextOrBmp = pDesData[0];
			//printf(" pAdverScrollInfo->mad_TextOrBmp 0x%x \n",pAdverScrollInfo->mad_TextOrBmp);
			pDesData += 1;
			bOffset += 1;
			if(bOffset < bLength)
			{
				if(pAdverScrInfo->mad_TextOrBmp == 0)
				{
					//printf(" AdverScroll_Descriptor mad_TextOrBmp \n");
					pAdverScrInfo->mad_FontColor = pDesData[0];
					pAdverScrInfo->mad_BackgroundColor = pDesData[1];
				}
			}
			pDesData += 4;
			bOffset += 4;
		}
		return true;
	}else
	{
		
		return false;
	}
	#endif
}
/*--------------------------------------------------------------------------*/
AdU8 AdverCornerDescriptor(AdU8 *pData,AdU16 bLength,AdverCorner_Info *pAdverC_Info)
{
	AdU8 *pDesData = pData;
	AdverCorner_Info *pAdverCorner_Info = pAdverC_Info;
	if(pDesData[0] == ADVER_CORNER_DESCRIPTOR)
	{
		if(bLength > 0)
		{
			pDesData += 2;
			
			pAdverCorner_Info->mad_StartxPos = ((unsigned short)pDesData[0]<<8)+pDesData[1];
			pAdverCorner_Info->mad_StartyPos = ((unsigned short)pDesData[2]<<8)+pDesData[3];
			/*printf(" pAdverCorner_Info->mad_StartxPos %d pAdverCorner_Info->mad_StartyPos %d \n",
							pAdverCorner_Info->mad_StartxPos,pAdverCorner_Info->mad_StartyPos);*/
			pDesData += 4;
			
			pAdverCorner_Info->mad_Width = ((unsigned short)pDesData[0]<<8)+pDesData[1];
			pAdverCorner_Info->mad_Height = ((unsigned short)pDesData[2]<<8)+pDesData[3];
			pDesData += 4;
			
			pAdverCorner_Info->mad_PicType = pDesData[0];
			pDesData += 1;
			
			memset(pAdverCorner_Info->mad_StartTime,0,5);
			memcpy(pAdverCorner_Info->mad_StartTime,pDesData,5);	
			pDesData += 5;
			
			memset(pAdverCorner_Info->mad_Duration,0,3);
			memcpy(pAdverCorner_Info->mad_Duration,pDesData,3);
			
		}
	}
	return true;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
AdU8 AdverOpenPictureDescriptor(AdU8 *pData,AdU16 bLength,AdverOpenPicture_Info *pAdOpenP_Info)
{
	AdU8 *pDesData = pData;
	
	if(pDesData[0] == ADVER_OPEN_PICTURE_DESCRIPTOR)
	{
		if(bLength > 0)
		{
			pDesData += 2;
			
			pAdOpenP_Info->mad_Width = ((unsigned short)pDesData[0]<<8)+pDesData[1];
			pAdOpenP_Info->mad_Height = ((unsigned short)pDesData[2]<<8)+pDesData[3];
			pDesData += 4;
			
			pAdOpenP_Info->mad_PicFormat = pDesData[0];
			pAdOpenP_Info->mad_Duration = pDesData[1];
			pDesData += 2;
			
			memset(pAdOpenP_Info->mad_Reserve,0,5);
			memcpy(pAdOpenP_Info->mad_Reserve,pDesData,5);	
			pDesData += 5;
			
			
		}
	}
	return true;
}

/*--------------------------------------------------------------------------*/
AdU8 AdverDescriptor(AdU8 *pData,AdU16 bLength,AdverCorner_Info *pAdCorner_Info,
	AdverScroll_Info *pAdScr_Info,AdverOpenPicture_Info *pAdOpenPic,AdU8 *pbPlayer)
{
	AdU8 *pDesData;
	AdU8 bLen = 0;

	pDesData = pData;
	while(bLength >0)
	{
		bLen = pDesData[1];
		switch(pDesData[0])
		{
			case ADVER_SCROLL_PROGRAM_DESCRIPTOR:
				//printf(" -----------ADVER_SCROLL_PROGRAM_DESCRIPTOR \n");
				AdverScroll_prog_Des(pDesData,bLen,pbPlayer);
				break;
			case ADVER_SCROLL_DESCRIPTOR:
				AdverScroll_Descriptor(pDesData,bLen,pAdScr_Info);
				//printf(" -----------ADVER_SCROLL_DESCRIPTOR %d \n",pAdScr_Info->mad_TextOrBmp);
				break;
			case ADVER_CORNER_DESCRIPTOR:
				AdverCornerDescriptor(pDesData,bLen,pAdCorner_Info);
				//printf(" -----------ADVER_CORNER_DESCRIPTOR \n");
				break;
			case ADVER_OPEN_PICTURE_DESCRIPTOR:
				//printf(" -----------ADVER_OPEN_PICTURE_DESCRIPTOR 0x%x \n",pDesData[0]);
				AdverOpenPictureDescriptor(pDesData,bLen,pAdOpenPic);
				*pbPlayer = 1;
				break;
			default:
				break;
		}
		pDesData += (bLen+2);
		bLength -= (bLen+2);
	}
	return true;
}
/*--------------------------------------------------------------------------*/
AdU8 CheckAdverFull(AdU8 bSecNum,AdU8 bLastSecNum,AdU8 bType)
{
	AdU8 i = 0;

	for(i = 0;i <= bLastSecNum;i++)
	{
		if(bType == CORNER_PICTURE)
		{
			if(gSaveAdverCorner_Info.mad_DataFullFlag[i] == false)
			return false;
		}else if(bType == FONT_MOVE_PICTURE)
		{
			if(gSaveAdverScroll_Info.mad_DataFullFlag[i] == false)
			return false;
		}else if(bType == OPEN_VIDEO_PICTRUE)
		{
			if(gSaveAdOpenPic_Info.mad_DataFullFlag[i] == false)
			return false;
		}
	}
	return true;
}
/*--------------------------------------------------------------------------*/
void CRC32Calculate(AdU8 *buffer,AdU32 size,AdU32 *CRC32)
{
  AdU32 crc32 = 0xFFFFFFFF;
	AdU32 cntByte,temp1,temp2,temp3;

	for (cntByte = 0; cntByte < size; cntByte++)
	{
		temp1=(crc32 << 8 );
		temp2=CRC32_table[((crc32 >> 24) ^ *buffer) & 0xFF];
		temp3=temp1^temp2;
		crc32 = (crc32 << 8 ) ^ CRC32_table[((crc32 >> 24) ^ *buffer++) & 0xFF];
	}
	*CRC32 = crc32;
}
/*--------------------------------------------------------------------------*/
#endif
