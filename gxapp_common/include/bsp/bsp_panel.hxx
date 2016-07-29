/*
 * =====================================================================================
 *
 *       Filename:  stb_panel.hxx
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年10月17日 20时15分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  Hangzhou Nationalchip Science&Technology Co.Ltd.
 *
 * =====================================================================================
 */

#ifndef __STB_PANEL_HXX__
#define __STB_PANEL_HXX__

extern "C" void gx3113_panel_mod_init(void);

class cyg_panel_init_class {
    public:
        cyg_panel_init_class(void)
        { 
            gx3113_panel_mod_init();
        }
};

#endif

