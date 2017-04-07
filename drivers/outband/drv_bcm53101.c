/*********************************
 *Author:     zhangjj@bjhuahuan.com
 *date:	      2015-10-20
 *Modified:
 ********************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>	//for copy_to_user
#include <linux/gpio.h>

#include "phy_api.h"

static struct mutex	bcm53101_lock;
struct cdev bcm53101_cdev;

struct fsl_pq_mdio {
	u8 res1[16];
	u32 ieventm;	/* MDIO Interrupt event register (for etsec2)*/
	u32 imaskm;	/* MDIO Interrupt mask register (for etsec2)*/
	u8 res2[4];
	u32 emapm;	/* MDIO Event mapping register (for etsec2)*/
	u8 res3[1280];
	u32 miimcfg;		/* MII management configuration reg */
	u32 miimcom;		/* MII management command reg */
	u32 miimadd;		/* MII management address reg */
	u32 miimcon;		/* MII management control reg */
	u32 miimstat;		/* MII management status reg */
	u32 miimind;		/* MII management indication reg */
	u8 reserved[28];	/* Space holder */
	u32 utbipar;		/* TBI phy address reg (only on UCC) */
	u8 res4[2728];
} __attribute__ ((packed));

struct fsl_pq_mdio __iomem *preg = NULL;

extern int bcm53101_get_preg(struct fsl_pq_mdio **upreg);
extern int fsl_pq_local_mdio_read(struct fsl_pq_mdio __iomem *regs, int mii_id, int regnum);
extern int fsl_pq_local_mdio_write(struct fsl_pq_mdio __iomem *regs, int mii_id, int regnum, u16 value);

#define PSEPHY_ACCESS_CTRL	16
#define PSEPHY_RDWR_CTRL	17
#define PSEPHY_ACCESS_REG1	24
#define PSEPHY_ACCESS_REG2	25
#define PSEPHY_ACCESS_REG3	26
#define PSEPHY_ACCESS_REG4	27

#define PSEDOPHY		30
#define PHY0			0
#define PHY1			1
#define PHY2			2
#define PHY3			3
#define PHY4			4

#define ACCESS_EN		1

#define OPER_RD 0x2
#define OPER_WR 0x1

struct bcm53101_t{
	unsigned char  which_sw;
	unsigned char  page;
	unsigned char  addr;
	unsigned short val[4];
}__attribute__((packed));

int fsl_mdio_write(int mii_id, unsigned short addr, unsigned short val)
{
	int ret = 0;

	if(NULL == preg)
	{
		printk("preg is NULL\n");
		return -1;
	}

	ret = fsl_pq_local_mdio_write(preg, mii_id, addr, val);

	return ret;
}
int fsl_mdio_read(int mii_id, unsigned short addr, unsigned short *val)
{
        int ret = 0;

        if(NULL == preg)
        {
                printk("preg is NULL\n");
                return -1;
        }

        ret = fsl_pq_local_mdio_read(preg, mii_id, addr);
	*val = ret & 0xffff;
	
        return 0;

}

#define BCM53101_GPIO_SEL	8
#define BCM53101_GPIO_SEL1	13

long switch_access_set(unsigned char cmd)
{
        int  retval = 0;

	switch (cmd)
        {
                case BCM53101_A:
        		gpio_direction_output(BCM53101_GPIO_SEL, 0);
        		gpio_direction_output(BCM53101_GPIO_SEL1, 0);
                        break;

                case BCM53101_B:
		        gpio_direction_output(BCM53101_GPIO_SEL, 1);
		        gpio_direction_output(BCM53101_GPIO_SEL1, 0);
                        break;
                case BCM53101_C:
		        gpio_direction_output(BCM53101_GPIO_SEL, 1);
		        gpio_direction_output(BCM53101_GPIO_SEL1, 1);
                        break;
                default:
                 retval = -1;
                break;
        }
        return retval;
}

int bcm53101_write(unsigned char which_sw, unsigned char page, unsigned char addr, unsigned short *value)
{
        unsigned short mdio_val = 0;
        unsigned short s_count = 0xffff;
        int mii_id = 0, ret_val = 0;

	mutex_lock(&bcm53101_lock);
	if(switch_access_set(which_sw))
	{
		ret_val = -1;
		goto END;
	}
        if(((page >= 0x10) && (page <= 0x14))||(page == 0x17))//get real port phy addr
        {
                mii_id = page - 0x10;
//		printk("mii_id=%d\n",mii_id);
                fsl_mdio_write(mii_id, addr/2, value[0]);
        }
        else                            //get psedophy addr
        {
                mii_id = PSEDOPHY;
		page &= 0xff;
		mdio_val |= (page << 8);
	        mdio_val |= ACCESS_EN;
	
	        fsl_mdio_write(mii_id, PSEPHY_ACCESS_CTRL,mdio_val);
	
	        fsl_mdio_write(mii_id, PSEPHY_ACCESS_REG1,value[0]);
	        fsl_mdio_write(mii_id, PSEPHY_ACCESS_REG2,value[1]);
	        fsl_mdio_write(mii_id, PSEPHY_ACCESS_REG3,value[2]);
	        fsl_mdio_write(mii_id, PSEPHY_ACCESS_REG4,value[3]);

		mdio_val = 0;
	        mdio_val |= addr << 8;
	        mdio_val |= OPER_WR;
	        fsl_mdio_write(mii_id, PSEPHY_RDWR_CTRL,mdio_val);

//	msleep(1);	
		do
		{
			fsl_mdio_read(mii_id, PSEPHY_RDWR_CTRL, &mdio_val);
			if(!(mdio_val&0x11))
				break;
			s_count --;
	//		msleep(1);
		}while(s_count > 0);
	}
END:
        mutex_unlock(&bcm53101_lock);
	if(ret_val)
		return -1;
	return 0;
}
int bcm53101_fs_write(struct file *filp, const char __user *buf,
                size_t count, loff_t *f_pos)
{
	struct bcm53101_t bcmstru;
	unsigned char which_sw, page, addr;	
	unsigned short value[4];

	copy_from_user(&bcmstru, buf, count);
	which_sw = bcmstru.which_sw;
	page = bcmstru.page;
	addr = bcmstru.addr;
	
	memcpy(value, bcmstru.val, sizeof(value));

	bcm53101_write(which_sw, page, addr, value);

	return 0;
}

int bcm53101_read(unsigned char which_sw, unsigned char page, unsigned char addr, unsigned short *value)
{
        unsigned short mdio_val = 0;
        unsigned short setval, s_count = 0xffff; 
	int ret_val = 0;
	int mii_id = 0;	

	mutex_lock(&bcm53101_lock);
	if(switch_access_set(which_sw))
	{
		ret_val = -1;
		goto END;
	}
	if(((page >= 0x10) && (page <= 0x14))||(page == 0x17))//get real port phy addr
	{
		mii_id = page - 0x10;
		fsl_mdio_read(mii_id, addr/2, &mdio_val);
		value[3] = 0; 
		value[2] = 0; 
		value[1] = 0; 
		value[0] = mdio_val; 
	}
	else				//get psedophy addr
	{
		mii_id = PSEDOPHY;
	        page &= 0xff;
		setval = 0;
	        setval |= (page << 8);
	        setval |= ACCESS_EN;
	        fsl_mdio_write(mii_id, PSEPHY_ACCESS_CTRL,setval);
		
	        setval = 0;
	        setval |= addr << 8;
	        setval |= OPER_RD;
	        fsl_mdio_write(mii_id, PSEPHY_RDWR_CTRL,setval);
	
	        do
	        {
	                ret_val = fsl_mdio_read(mii_id, PSEPHY_RDWR_CTRL, &setval);
	                if((!(setval&0x11))||(ret_val))
	                        break;
			s_count --;
	//		msleep(1);
	        }while(s_count > 0);
	        ret_val = fsl_mdio_read(mii_id, PSEPHY_ACCESS_REG1, &setval);
		value[0] = setval;
	        ret_val = fsl_mdio_read(mii_id, PSEPHY_ACCESS_REG2, &setval);
		value[1] = setval;
	        ret_val = fsl_mdio_read(mii_id, PSEPHY_ACCESS_REG3, &setval);
		value[2] = setval;
	        ret_val = fsl_mdio_read(mii_id, PSEPHY_ACCESS_REG4, &setval);
		value[3] = setval;
	}
END:
        mutex_unlock(&bcm53101_lock);
	if(ret_val)
		return -1;

	return 0;
}
int bcm53101_fs_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct bcm53101_t bcmstru;
	unsigned char which_sw, page, addr;	
	unsigned short value[4];

	copy_from_user(&bcmstru, buf, count);
	which_sw = bcmstru.which_sw;
	page = bcmstru.page;
	addr = bcmstru.addr;

//	printk("which_sw=%d, addr0x%x, bcmstru.addr=%x\n",which_sw, addr,bcmstru.addr);

	bcm53101_read(which_sw, page, addr, value);
	memcpy(bcmstru.val, value, sizeof(value));
//printk("bcm53101_fs_read bcmstru.val=%x\n",bcmstru.val);
	copy_to_user(buf, &bcmstru, sizeof(bcmstru));

	return 0;
}
int bcm53101_cfg_init()
{
	unsigned short mdio_val[4] = {0};

	if(0 != bcm53101_get_preg(&preg))
	{
		printk("Get preg error\n");
		return -1;
	}
	return 0;	
}
static struct file_operations bcm53101_fops = {
	.owner = THIS_MODULE,
	.read  = bcm53101_fs_read,
	.write = bcm53101_fs_write
};

static struct class *bcm53101_cls;
int major;
static int __init bcm53101_init(void)
{
	dev_t dev_id;
	printk(KERN_ALERT "BCM53101 driver init...");

	if(bcm53101_cfg_init())
	{
		return 0;
	}
	mutex_init(&bcm53101_lock); 

	gpio_direction_output(BCM53101_GPIO_SEL, 0);
	gpio_direction_output(BCM53101_GPIO_SEL1, 0);

	alloc_chrdev_region(&dev_id, 0, 1, "bcm53101");
	major = MAJOR(dev_id);

	cdev_init(&bcm53101_cdev, &bcm53101_fops);
	cdev_add(&bcm53101_cdev, dev_id, 1);

	bcm53101_cls = class_create(THIS_MODULE, "bcm53101");

	device_create(bcm53101_cls, NULL, dev_id, NULL, "bcm53101");	

	outband_init();
	//bcm53101_cfg_init();

	printk(KERN_ALERT "Done\n");

	return 0;
}

static void __exit bcm53101_exit(void)
{
     	printk(KERN_ALERT "bcm53101 driver exit\n");

	device_destroy(bcm53101_cls, MKDEV(major, 0));
    	class_destroy(bcm53101_cls);
 
	/* 3.3 删除cdev*/
	cdev_del(&bcm53101_cdev);
 
	/* 3.4 释放设备号*/
	unregister_chrdev_region(MKDEV(major, 0), 1);
	outband_exit();
}

module_init(bcm53101_init);
module_exit(bcm53101_exit);
MODULE_LICENSE("GPL");


