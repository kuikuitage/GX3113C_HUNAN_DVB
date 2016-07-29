#if defined(LINUX_OS)
#include <linux/gx_gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/sysdev.h>
#include <linux/timer.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/types.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include <linux/blkpg.h>
#include <linux/bio.h>
#include <linux/blktrace_api.h>
#include <linux/poll.h>
#include <linux/wait.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h>

//#include "gx_panel.h"
#include "stb_panel.h"

uint32_t s_panel_key = -1;
//extern GXPANEL_PANEL_ControlBlock_t gs_PanelControlBlock;
MODULE_AUTHOR("huangbc");
MODULE_LICENSE("Dual BSD/GPL");

static int majorno;

struct class *gx_panel_class;


/*static uint32_t s_key_value[8]=
{
    PANEL_KEY_UP,PANEL_KEY_DOWN,PANEL_KEY_LEFT,PANEL_KEY_RIGHT,PANEL_KEY_OK,PANEL_KEY_MENU,PANEL_KEY_EXIT,PANEL_KEY_POWER
};*/





struct gx_panel_dev
{
        struct cdev cdev;
        uint32_t arg;
	wait_queue_head_t panel_read_queue;
};

struct gx_panel_dev *p_gx_panel_dev;


static struct timer_list bsp_timer;
static timer_func _panel_callback = NULL;
static unsigned long timer_delay = 0;



static void timer_cb(unsigned long data)
{
     if(_panel_callback != NULL)
     {
         _panel_callback();
     }
     mod_timer(&bsp_timer, jiffies + timer_delay);
}
 
void create_bsp_timer(timer_func func, unsigned long ms)
{
    _panel_callback = func;
    timer_delay = ms;
    bsp_timer.function = timer_cb;
    bsp_timer.expires = jiffies + timer_delay;
    if(!timer_pending(&bsp_timer))
    {
        init_timer(&bsp_timer);
        add_timer(&bsp_timer);
        //bsp_printf(KERN_INFO"create timer!\n");
    }
}

void distroy_bap_timer(void)
{
	_panel_callback = NULL;
	del_timer(&bsp_timer);
}


static int gx_panel_open(struct inode *inode,struct file *filp)
{
	bsp_printf("open panel\n");
    filp->private_data = p_gx_panel_dev;
    if(gs_PanelControlBlock.m_PanelConfigFun != NULL)		
	{
	    gs_PanelControlBlock.m_PanelConfigFun();
	}
	if(gs_PanelControlBlock.m_PanelInitPinFun != NULL)
	{
	   gs_PanelControlBlock.m_PanelInitPinFun(); 
	}
	if(gs_PanelControlBlock.m_PanelIsrFun != NULL)
	{
	    gs_PanelControlBlock.m_PanelIsrFun();
	}
        return 0;
}

void poll_wakeup(void)
{
    wake_up(&p_gx_panel_dev->panel_read_queue);
}


unsigned int (gx_panel_poll)(struct file * fp, struct poll_table_struct * table)
{
   int mask = 0;
   if(s_panel_key == -1)
   {
	poll_wait(fp,&p_gx_panel_dev->panel_read_queue, table);	
   } 
   else
   {
	mask |= POLLIN;
   }
   return mask;
}


static int gx_panel_read(struct file *filp, char *buf, size_t size, loff_t *ppos)
{
	if(s_panel_key != -1)
	{
		bsp_printf("gx_panel_read s_panel_key=0x%x\n",s_panel_key);
//	    copy_to_user(buf,&s_key_value[s_panel_key],sizeof(uint32_t));
	    copy_to_user(buf,&s_panel_key,sizeof(uint32_t));
	    s_panel_key = -1;
	}
        return 0;
}

static int gx_panel_write(struct file *filp, const char *buf,size_t size, loff_t *ppos )
{
	
        return 0;
}

static int gx_panel_release(struct inode *inode,struct file *filp)
{
        return 0;
}



#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0)
static int gx_panel_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,unsigned long arg)
#else
static long gx_panel_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
#endif
{
        int ret = 0;
//        int i;
//        void * __iomem reg_addr;
        uint32_t buf = 0;


	if(filp == NULL)return -1;
	if ((PANEL_GPIO_HIGH == cmd)||(PANEL_GPIO_LOW == cmd)
		||(PANEL_STRING == cmd)||(PANEL_DATA == cmd)
		||(PANEL_READ_KEY == cmd))
		{
			if(arg == 0)return -1;
		    buf = *(uint32_t*)arg;		
		}

	switch(cmd)
	{
	    case PANEL_READ_KEY:
		
		break;
		case PANEL_GPIO_HIGH:
			bsp_printf("PANEL_GPIO_HIGH %d\n",buf);
			gx_gpio_setio(buf, 1);
			gx_gpio_setlevel(buf,1);			
			break;
		case PANEL_GPIO_LOW:
			bsp_printf("PANEL_GPIO_LOW %d\n",buf);
			gx_gpio_setio(buf, 1);
			gx_gpio_setlevel(buf,0);			
			break;
	    case PANEL_STRING:
		bsp_printf("panel_string %d\n",buf);
		if(gs_PanelControlBlock.m_PanelSetStringFun != NULL)
		{
		    gs_PanelControlBlock.m_PanelSetStringFun((unsigned char *)buf);
		}
		break;
	    case PANEL_DATA:
		bsp_printf("panel_data %d\n",buf);
	        if(gs_PanelControlBlock.m_PanelSetValueFun != NULL)
		{
		    gs_PanelControlBlock.m_PanelSetValueFun(buf);
		}	
		break;
	    case PANEL_LOCK:
		if(gs_PanelControlBlock.m_PanelLockFun != NULL)
		{
		    gs_PanelControlBlock.m_PanelLockFun();
		}
		break;
	    case PANEL_UNLOCK:
	        if(gs_PanelControlBlock.m_PanelUnlockFun != NULL)
		{
		    gs_PanelControlBlock.m_PanelUnlockFun();
		}	
		break;
	    case PANEL_POWER_OFF:
        bsp_printf("PANEL_POWER_OFF\n");
		if(gs_PanelControlBlock.m_PanelStandByFun != NULL)
		{
#if (PANEL_TYPE == PANEL_TYPE_CT1642_JINYA)
			distroy_bap_timer();
#endif
		    gs_PanelControlBlock.m_PanelStandByFun();
		}
        
        if(gs_PanelControlBlock.m_PanelPowerOffFun != NULL)
		{
		    gs_PanelControlBlock.m_PanelPowerOffFun();
		}
        
		break;
	    case PANEL_SET_BLOCKMODE:
		break;
	    case PANEL_STANDBY:
		break;
	    case PANEL_WAKEUP:
		break;
	    case PANEL_KEY:
		break;
	}
	return ret;
}


struct file_operations gx_panel_fops = {

        .owner = THIS_MODULE,
        .open = gx_panel_open,
        .read = gx_panel_read,
        .write = gx_panel_write,
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0)
        .ioctl = gx_panel_ioctl,
#else
        .unlocked_ioctl = gx_panel_ioctl,
#endif
        .release = gx_panel_release,
        	.poll    = gx_panel_poll,

};

static void gx_panel_set_cdev(struct gx_panel_dev *dev,int index)
{
        int err,devno = MKDEV(majorno,0);
        cdev_init(&dev->cdev,&gx_panel_fops);
        dev->cdev.ops = &gx_panel_fops;
        err = cdev_add(&dev->cdev,devno,1);
        if(err)
                bsp_printf(KERN_INFO " Error adding monitor");
}


static int __init gx_panel_init(void)
{
        int result;
        dev_t devno;

        result = alloc_chrdev_region(&devno,0,1,"gx_panel");
        if(result < 0)
                return result;

        majorno = MAJOR(devno);
        if(result<0)return result;
        p_gx_panel_dev = kmalloc(sizeof(struct gx_panel_dev),GFP_KERNEL);
        if(!p_gx_panel_dev)
        {
                result = -ENOMEM;
                goto fail;
        }
        memset(p_gx_panel_dev,0,sizeof(struct gx_panel_dev));
        gx_panel_set_cdev(p_gx_panel_dev,0);
        gx_panel_class = class_create(THIS_MODULE,"gx_panel_class");
        device_create(gx_panel_class,NULL,MKDEV(majorno,0),NULL,"gxpanel%d",0);
	    init_waitqueue_head(&p_gx_panel_dev->panel_read_queue);
        bsp_printf("gx_panel_init ok\n");
	return 0;
fail:
        bsp_printf("init error and return\n");
        unregister_chrdev_region(devno,1);
        return result;
}


static void __exit gx_panel_exit(void)
{

        bsp_printf(KERN_INFO "gx_panel_exit.\n");
	    del_timer(&bsp_timer);
        cdev_del(&p_gx_panel_dev->cdev);
        device_destroy(gx_panel_class,MKDEV(majorno,0));
        class_destroy(gx_panel_class);
        kfree(p_gx_panel_dev);
        unregister_chrdev_region(MKDEV(majorno,0),1);
        bsp_printf(KERN_INFO "gx_panel_exit ok.\n");
}


module_init(gx_panel_init);
module_exit(gx_panel_exit);
#endif

