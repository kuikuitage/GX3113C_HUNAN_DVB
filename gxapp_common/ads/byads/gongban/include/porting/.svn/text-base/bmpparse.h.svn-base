#ifndef BMPPARSE_H
#define BMPPARSE_H

#include "adver_type.h"

#define BMP_MAX_SIZE				32*1024
#define BMP_MAX_COLORTable 	256*3		

typedef struct BmpInfo_Tag{
	AdU32 biWidth;
	AdU32 biHeight;
	AdU32 biSize;
	AdU8 pColorTable[BMP_MAX_COLORTable];
	AdU8 *pData;	
}BmpInfo;

/* C++ support */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

AdU8 BmpParse(AdU8 *pBuffer,AdU32 iLen,BmpInfo *pBmpInfo);//挂角广告BMP解压算法
AdU8 DeComBitmapData(AdU8 *DesBmp,AdU8 *SrcBmp,AdU32 *Len,AdU32 iSLen);//滚动字幕BMP解压算法

/* End C++ support */
#ifdef __cplusplus
}
#endif/* __cplusplus */

#endif

