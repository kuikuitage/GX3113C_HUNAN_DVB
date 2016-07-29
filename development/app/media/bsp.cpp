#include "gxcore_bsp.h"

/* ------------------------- Main Chip ----------------------------------- */
CHIP_GX3110
//CHIP_GX3200

/* ------------------------- Board support device ------------------------ */
MOD_WDT 
MOD_IRR
//MOD_SCI
//MOD_MMC
MOD_AV
MOD_UART
//MOD_NORFLASH 
MOD_SPIFLASH
MOD_FLASHIO
//MOD_NANDFLASH
MOD_NET
MOD_USB
MOD_PANEL
//MOD_HDMI

/* ------------------------------ Video & Audio Codec -------------------- */
CODEC_AC3
//CODEC_AVSA
//CODEC_AVSV
CODEC_DRA
CODEC_H263
CODEC_H264
CODEC_MPEG212A
CODEC_MPEG4_AAC
CODEC_MPEG4V
CODEC_OGG
CODEC_RA_AAC
CODEC_RA_RA8LBR
CODEC_RV
CODEC_JPEG

/* ------------------------- Support filesystem -------------------------- */
MOD_FAT
MOD_NTFS
//MOD_JFFS2
MOD_CAMFS
//MOD_ROMFS
MOD_RAMFS

/* -------------------------- Frontend ----------------------------------- */
//MOD_GX1001
//MOD_GX1101
//MOD_GX1131
//MOD_GX1201
//MOD_GX1501

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
//LANG_VIETNA
