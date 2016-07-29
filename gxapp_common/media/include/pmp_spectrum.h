#ifndef __PMP_SPECTRUM_H__
#define __PMP_SPECTRUM_H__

#include <stdio.h>
#include <stdlib.h>



int spectrum_create(char* widget_canvas, int x0,int y0,int width,int height);
int spectrum_destroy(void);
int spectrum_start(void);
int spectrum_stop(void);
int spectrum_pause(void);
int spectrum_resume(void);

int spectrum_redraw(void);

 void showSpectrumView(void);

#endif

