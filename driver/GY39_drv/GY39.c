#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/random.h>
unsigned canread = 0;
						//  高字节 低字节
						
unsigned char buflight[9]={0x5A,0x5A,0x15,0x4,0,0,0,0,0x0b};
unsigned char buftemp[15]={0x5A,0x5A,0x45,0xa,0,0,0,0,0,0,0,0,0,0,0xfa};
unsigned char buftotal[24] = {0x5A,0x5A,0x15,0x4,0,0,0,0,0x0b,0x5a,0x5a,0x45,0xa,0,0,0,0,0,0,0,0,0,0,0xff};
unsigned long light = 0;
unsigned short temp = 0;
unsigned short humidity = 0;
unsigned short height = 0;
unsigned long pressure = 0;

static int GY39_open(struct inode *inode, struct file *file)
{
	uint i;
	for(i=4;i<8;i++)
		buflight[i]=0;
	for(i=4;i<14;i++)
		buftemp[i]=0;
	
	printk(KERN_EMERG "GY39_open \n");
	return 0;
}

ssize_t GY39_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{	
	uint n;
	if(canread == 0)
	{
		printk(KERN_EMERG, " plesae input correct command!\n");
		return 0;
	}	
	get_random_bytes(&light, sizeof(unsigned long));
	get_random_bytes(&temp, sizeof(unsigned short));
	get_random_bytes(&humidity, sizeof(unsigned short));
	get_random_bytes(&height, sizeof(unsigned short));
	get_random_bytes(&pressure, sizeof(unsigned long));
	light = light%2000;
	temp = temp%60;
	pressure = pressure %10000;
	height = height %8000;
	humidity = humidity % 100;
	
	if(canread == 1)//光照
	{
		buflight[4] =  (light>>16)/256;
		buflight[5] =  (light>>16)%256;
		buflight[6] =  light/256;
		buflight[7] =  light%256;
		n = copy_to_user(buf, buflight, 9);
	} 
	else if(canread == 2)//温度
	{
		buftemp[4] =  temp/256;
		buftemp[5] =  temp%256;
		buftemp[6] =  (pressure>>16)/256;
		buftemp[7] =  (pressure>>16)%256;
		buftemp[8] =  pressure/256;
		buftemp[9] =  pressure%256;
		buftemp[10] =  humidity/256;
		buftemp[11] =  humidity%256;
		buftemp[12] =  height/256;
		buftemp[13] =  height%256;
		n = copy_to_user(buf, buftemp, 15);

	}
	else if(canread == 3)//光照,温度
	{
		buftotal[4] =  (light>>16)/256;
		buftotal[5] =  (light>>16)%256;
		buftotal[6] =  light/256;
		buftotal[7] =  light%256;
		buftotal[13] =  temp/256;
		buftotal[14] =  temp%256;
		buftotal[15] =  (pressure>>16)/256;
		buftotal[16] =  (pressure>>16)%256;
		buftotal[17] =  pressure/256;
		buftotal[18] =  pressure%256;
		buftotal[19] =  humidity/256;
		buftotal[20] =  humidity%256;
		buftotal[21] =  height/256;
		buftotal[22] =  height%256;
		n = copy_to_user(buf, buftotal, 24);
	}
		
	return size - n; //成功读到了多少字节
}
static ssize_t GY39_write(struct file *file, const char __user *buf, size_t size, loff_t * off)
{
	unsigned char kbuf[10]={0};
	uint n;
	n = copy_from_user(kbuf, buf, size);
	if(kbuf[0]==0xA5 && kbuf[1]==0x51 && kbuf[2]==0xF6)
	{
		printk(KERN_EMERG, "light cmd!\n");
		canread =  1;
	}
	else if(kbuf[0]==0xA5 && kbuf[1]==0x52 && kbuf[2]==0xF7)
	{
		printk(KERN_EMERG, "temp humidity... cmd!\n");
		canread =  2;
	}
	else if(kbuf[0]==0xA5 && kbuf[1]==0x83 && kbuf[2]==0x28)
	{
		printk(KERN_EMERG, "light temp humidity... cmd!\n");
		canread =  3;
	}
	else 
	{
		printk(KERN_EMERG, "write cmd is wrong!\n");
		canread = 0;
	}
	return size-n;
}

static int GY39_close(struct inode *inode, struct file *file)
{

	printk(KERN_EMERG "GY39_close \n");
	return 0;
}

static struct file_operations GY39_ops=
{
	.open = GY39_open,
	.read = GY39_read,
	.write = GY39_write,
	.release = GY39_close
};


static struct miscdevice miscGY39={
	.minor = MISC_DYNAMIC_MINOR,   //动态分配次设备号
	.name = "ttySAC2",  //   /dev/myGY39
	.fops = &GY39_ops,
};


static int __init GY39_init(void)
{
	//2. 注册
	int ret;
	ret = misc_register(&miscGY39);
	if(ret < 0)
	{
		printk(KERN_EMERG "misc_register failed\n");
		return -1;
	}
	printk(KERN_EMERG "GY39 module register success\n");
	return 0;
}

static void __exit GY39_exit(void)
{
	misc_deregister(&miscGY39);
	printk(KERN_INFO "GY39 module deleted success\n");
}

module_init(GY39_init);
module_exit(GY39_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("huang fu");
