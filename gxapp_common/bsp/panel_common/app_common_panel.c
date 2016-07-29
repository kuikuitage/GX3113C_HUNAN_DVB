#include "app_common_panel.h"
#include <sys/ioctl.h>
extern int panel_fd;
void app_panel_show(unsigned int key, void *buf)
{
	if (panel_fd > 0)
		{
           	ioctl(panel_fd, key, buf);
		}
	return ;	
}

void app_panel_set_gpio_high(unsigned int eport)
{
	unsigned int level = eport;
	unsigned int key = PANEL_GPIO_HIGH;
	if (panel_fd > 0)
		{
           	ioctl(panel_fd, key, &level);
		}
	return ;
}

void app_panel_set_gpio_low(unsigned int eport)
{
	unsigned int level = eport;

	unsigned int key = PANEL_GPIO_LOW;
	if (panel_fd > 0)
		{
           	ioctl(panel_fd, key, &level);
		}
	return;
}



