/* ---------------------------------------------- */
#include "stb_panel.h"
#if (PANEL_TYPE == PANEL_TYPE_NONE)
GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock ={
	.m_PanelConfigFun = NULL,
	.m_PanelIsrFun = NULL,
	.m_PanelInitPinFun = NULL,
	.m_PanelLockFun = NULL,
	.m_PanelSetSignalValueFun = NULL,
	.m_PanelSetStringFun = NULL,
	.m_PanelSetValueFun = NULL,
	.m_PanelUnlockFun = NULL,
	.m_PanelShowModeFun = NULL,
	.m_PanelScanMangerFun = NULL,
	.m_PanelPowerOffFun = NULL,
};
#endif

