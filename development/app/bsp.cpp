#ifdef ECOS_OS
#include "gxcore_bsp.h"
#include "app_bsp.hxx"
#include "frontend/gx_frontend_init.hxx"
#include "gxapp_sys_config.h"

#if (DVB_CHIP_TYPE == GX_CHIP_3113C)||(DVB_CHIP_TYPE == GX_CHIP_3115)
CHIP_GX3113C
#endif
#if(DVB_CHIP_TYPE == GX_CHIP_3201)
CHIP_GX3201
#endif
#if (DVB_CHIP_TYPE == GX_CHIP_3113H)||(DVB_CHIP_TYPE == GX_CHIP_3201H)
CHIP_GX3211
#endif


//MOD_WDT
MOD_I2C
MOD_IRR
MOD_SCI
MOD_FAT
MOD_NTFS
//MOD_JFFS2
MOD_CAMFS
MOD_ROMFS
MOD_RAMFS
MOD_MINIFS


MOD_AV(NULL)
MOD_UART
//MOD_NORFLASH
MOD_SPIFLASH
MOD_FLASHIO
//MOD_NANDFLASH
//MOD_NET
MOD_USB

MOD_PANEL

/* -------------------------- Frontend ----------------------------------- */

/* -------------------------- Language Package --------------------------- */
//LANG_US
LANG_SIMP_CHINESE
//LANG_TRAD_CHINESE
//LANG_KOREAN
//LANG_JAPANESE
//LANG_ARABIC_O
//LANG_ARABIC_W
//LANG_GREEK_O
//LANG_GREEK_W
//LANG_CENT_EUR
//LANG_BALTIC_O
//LANG_BALTIC_W
//LANG_MULTI_LANTIN1
//LANG_LATIN2_O
//LANG_LATIN1_W
//LANG_CYRILLIC_O
//LANG_CYRILLIC_W
//LANG_RUSSIAN_O
//LANG_TURKISH_O
//LANG_TURKISH_W
//LANG_MULTI_LATIN1_EUR
//LANG_HEBREW_O
//LANG_HEBREW_W
//LANG_THAI
//LANG_VIETNAM

//CODEC_AC3
//CODEC_AVSA
//CODEC_AVSV
//CODEC_DRA
//CODEC_H263
//CODEC_H264

#if(DVB_CHIP_TYPE == GX_CHIP_3201)
CODEC_VIDEO(gx3201)
CODEC_DRA(gx3201)
CODEC_MPEG212A(gx3201)
CODEC_MPEG4_AAC(gx3201)
#if AUDIO_DOLBY
CODEC_DOLBY(gx3201)
#endif
#elif (DVB_CHIP_TYPE == GX_CHIP_3113H)||(DVB_CHIP_TYPE == GX_CHIP_3201H)
CODEC_VIDEO(gx3211)
CODEC_DRA(gx3211)
CODEC_MPEG212A(gx3211)
CODEC_MPEG4_AAC(gx3211)
#if AUDIO_DOLBY
CODEC_DOLBY(gx3211)
#endif
#elif (DVB_CHIP_TYPE == GX_CHIP_3113C)||(DVB_CHIP_TYPE == GX_CHIP_3115)
CODEC_VIDEO(gx3113c)
//CODEC_AUDIO(gx3113c)
CODEC_DRA(gx3113c)
CODEC_MPEG212A(gx3113c)
CODEC_MPEG4_AAC(gx3113c)
//test add decode support
CODEC_DTS(gx3113c)
CODEC_RA_AAC(gx3113c)
CODEC_RA_RA8LBR(gx3113c)
//test
#endif

#endif