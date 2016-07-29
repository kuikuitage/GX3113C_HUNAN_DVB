#ifndef _DVBCA_URGENT_H_
#define _DVBCA_URGENT_H_
typedef struct _ST_DVBCA_URGENT
{
	DVBCA_UINT8 ucDisplay;
	DVBCA_UINT8 ucNewUrgent;
	DVBCA_UINT8 ucVer;  			/*版本*/
	DVBCA_UINT8 ucConCycle;		/*持续时间*/
	DVBCA_UINT8 ucShowTimes;	/*显示次数*/
	DVBCA_UINT8 aucTitle[DVBCA_NOTIFY_TITLE_SIZE + 1];		/*标题*/
	DVBCA_UINT8 aucContent[DVBCA_NOTIFY_CONTENT_SIZE + 1];	/*内容*/
	DVBCA_UINT16 usIntervalCycle;/*间隔时间*/
	DVBCA_UINT32 	uiStartTime;
}ST_DVBCA_URGENT;

int DVBCA_Urgent_Init(DVBCA_UINT8 ucThreadPriority);
void DVBCA_UrgentParse(DVBCA_UINT8 *pucBuffer, DVBCA_UINT16 usLen);
void DVBCA_Urgent_UnInit(void);


#endif

