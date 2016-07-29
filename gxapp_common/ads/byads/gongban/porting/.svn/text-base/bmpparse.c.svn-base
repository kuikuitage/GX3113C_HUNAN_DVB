#ifndef BMPPARSE_C
#define BMPPARSE_C

#include <stdio.h>
#include <stdlib.h>
//#include <debug.h>
#include <assert.h>
#include <string.h>
//#include "PriType.h"
#include "bmpparse.h"

//#define BMP_DISPLAY_TEST
	


/*------------------------------------------------------------------------------------------------*/
AdU8 BmpParse(AdU8 *pBuffer,AdU32 iLen,BmpInfo *pBmpInfo)
{
	AdU8 		*pData = pBuffer;
	AdU32  	iOffset = 0;
	AdU32  	iFileSize = 0,ibfOffBits = 0,biSize = 0;
	AdU32  	biWidth = 0,biHeight = 0,biPlanes = 0;
	AdU8 		biBitCount = 0;
	AdU32		biCompression = 0,biSizeImage = 0,biXpelsPerMete = 0,biYpelsPerMete = 0;
	AdU32 	biClrUsed = 0,biClrimportant = 0;
	AdU32  	iCount = 0,i = 0,iBmpSize = 0,iDatasizePerLine = 0;
	
	#ifdef BMP_DISPLAY_TEST
	printf(" iLen bitmap %d \n",iLen);
	#endif
	if((pBuffer == NULL) || (iLen <= 54))
	{
		printf(" error bitmap \n");
		return false;
	}
	if((pData[0] == 'B') && (pData[1] == 'M'))
	{
		pData 	+= 2;
		iOffset += 2;
		
	}else
	{
		printf(" unknow bitmap \n");
		return false;
	}
	
	
	iFileSize = pData[0]+((unsigned int)pData[1]<<8)+((unsigned int)pData[2]<<16)+((unsigned int)pData[3]<<24);
	#ifdef BMP_DISPLAY_TEST
	printf("iFileSize %d \n",iFileSize);
	#endif
	
	pData 	+= 8;	// 4 filesize + 2 resvers
	iOffset += 8;
	// 10
	ibfOffBits =  pData[0]+((unsigned int)pData[1]<<8)+((unsigned int)pData[2]<<16)+((unsigned int)pData[3]<<24);
	#ifdef BMP_DISPLAY_TEST
	printf("ibfOffBits %d \n",ibfOffBits);
	#endif
	pData 	+= 4;	
	iOffset += 4;
	
	//14
	biSize = pData[0]+((unsigned int)pData[1]<<8)+((unsigned int)pData[2]<<16)+((unsigned int)pData[3]<<24);
	#ifdef BMP_DISPLAY_TEST
	printf("biSize %d  \n",biSize);
	#endif
	if(biSize > BMP_MAX_SIZE)
	{
		printf(" Sorry! BITMAPS file so large %d \n",biSize);
		return false;
	}	
	pData 	+= 4;	
	iOffset += 4;
	//18
	biWidth 	= pData[0]+((unsigned int)pData[1]<<8)+((unsigned int)pData[2]<<16)+((unsigned int)pData[3]<<24);
	biHeight	= pData[4]+((unsigned int)pData[5]<<8)+((unsigned int)pData[6]<<16)+((unsigned int)pData[7]<<24);
	#ifdef BMP_DISPLAY_TEST
	printf("biWidth %d biHeight %d \n",biWidth,biHeight);
	#endif
	if((biWidth > 720) || (biHeight > 576))
	{
		printf(" Sorry! File biWidth or biHeight too large \n");
		return false;
	}
	pData 	+= 8;	
	iOffset += 8;
	//26
	biPlanes = pData[0]+((unsigned short)pData[1]<<8);
	pData 	+= 2;	
	iOffset += 2;
	//28
	biBitCount = pData[0]+((unsigned short)pData[1]<<8);
	#ifdef BMP_DISPLAY_TEST
	printf("biBitCount %d \n",biBitCount);
	#endif
	if(biBitCount != 8)
	{
		printf(" Sorry! File no 256 biBitCount  \n");
		return false;
	}
	pData 	+= 2;	
	iOffset += 2;
	//30
	biCompression = pData[0]+((unsigned int)pData[1]<<8)+((unsigned int)pData[2]<<16)+((unsigned int)pData[3]<<24);
	biSizeImage		= pData[4]+((unsigned int)pData[5]<<8)+((unsigned int)pData[6]<<16)+((unsigned int)pData[7]<<24);
	#ifdef BMP_DISPLAY_TEST
	printf("biCompression %d biSizeImage %d \n",biCompression,biSizeImage);
	#endif
	pData 	+= 8;	
	iOffset += 8;
	//38
	biXpelsPerMete = pData[0]+((unsigned int)pData[1]<<8)+((unsigned int)pData[2]<<16)+((unsigned int)pData[3]<<24);
	biYpelsPerMete = pData[4]+((unsigned int)pData[5]<<8)+((unsigned int)pData[6]<<16)+((unsigned int)pData[7]<<24);
	#ifdef BMP_DISPLAY_TEST
	printf("biXpelsPerMete %d biYpelsPerMete %d \n",biXpelsPerMete,biYpelsPerMete);
	#endif
	pData 	+= 8;	
	iOffset += 8;
	//46
	biClrUsed = pData[0]+((unsigned int)pData[1]<<8)+((unsigned int)pData[2]<<16)+((unsigned int)pData[3]<<24);
	biClrimportant = pData[4]+((unsigned int)pData[5]<<8)+((unsigned int)pData[6]<<16)+((unsigned int)pData[7]<<24);
	#ifdef BMP_DISPLAY_TEST
	printf("biClrUsed %d biClrimportant %d \n",biClrUsed,biClrimportant);
	#endif
	pData 	+= 8;	
	iOffset += 8;
	//54 //file header
	if(iLen < (1024+54))
	{
		printf(" File too small \n");
		return false;
	}
	if(biBitCount == 8)
	{
		pBmpInfo->biWidth = biWidth;
		pBmpInfo->biHeight = biHeight;
		
		iCount = 0;
		
		for(i = 0;i < 1024;)
		{
			pBmpInfo->pColorTable[iCount] 	= pData[i+2];
			pBmpInfo->pColorTable[iCount+1] = pData[i+1];
			pBmpInfo->pColorTable[iCount+2] = pData[i];
			i += 4;
			iCount += 3;
		}
	}
	pData 	+= 1024;	
	iOffset += 1024;
	//1078
	iBmpSize = iFileSize - ibfOffBits;
	#ifdef BMP_DISPLAY_TEST
	printf(" iBmpSize %d \n",iBmpSize);
	#endif
	
	{
		unsigned char *pSBuf = NULL,*pDBuf = NULL;
		unsigned int i = 0,j = 0; 
		
		iDatasizePerLine =  (biWidth*8+31)/8;
    iDatasizePerLine = iDatasizePerLine/4*4;
		pSBuf = (unsigned char *)malloc(iBmpSize);
		if(pSBuf == NULL)
		{
				printf(" no free Buffer 111\n");
			assert(0);
		}
		memcpy(pSBuf,pData,iBmpSize);
  	pDBuf = (unsigned char *)malloc(iBmpSize);
  	if(pDBuf == NULL)
  	{
  			printf(" no free Buffer 222\n");
			assert(0);
		}
  	iCount = 0;
		for(j = 0;j < biHeight;j++)
		{
			for(i = 0;i < biWidth;i++)
			{
			
			   pDBuf[iCount] = pSBuf[(biHeight-1-j)*iDatasizePerLine+i];
			   iCount++;
			}
		}
		#ifdef BMP_DISPLAY_TEST
		printf(" Filesize iCount %d \n",iCount);
		#endif
		pBmpInfo->biSize = iCount;
		if(pBmpInfo->pData != NULL)
		free(pBmpInfo->pData);
		if(pBmpInfo->pData == NULL)
		{
			pBmpInfo->pData = (unsigned char *)malloc(iCount);
			if(pBmpInfo->pData == NULL)
			{
					printf(" no free Buffer 3333\n");
				assert(0);
			}
		}
		memcpy(pBmpInfo->pData,pDBuf,iCount);
	
		if(pDBuf != NULL)
		free(pDBuf);
		if(pSBuf != NULL)
		free(pSBuf);
	}  
	#ifdef BMP_DISPLAY_TEST
	printf("  bitmap over  \n");
	#endif
	return true;
}
/*------------------------------------------------------------------------------------------------*/
AdU8 DeComBitmapData(AdU8 *DesBmp,AdU8 *SrcBmp,AdU32 *Len,AdU32 iSLen)
{
	AdU32 i = 0,iCount = 0;
	AdU8 bNum = 0,j = 0;
	#ifdef BMP_DISPLAY_TEST
	printf(" DeCompressBitmapData SrcBmp %d  iCount  %d \n",SrcBmp[0],iSLen);
	#endif
	for(i = 0;i < iSLen;)
	{
		bNum = SrcBmp[i];
		for(j = 0;j < bNum;j++)
		{
			DesBmp[iCount] =  SrcBmp[i+1];
			 iCount++;
		}
		i += 2;
	}
	*Len = iCount;
	#ifdef BMP_DISPLAY_TEST
	printf(" DeCompressBitmapData Over iCount  %d \n",iCount);
	#endif
	return true;
}
/*------------------------------------------------------------------------------------------------*/
#endif

