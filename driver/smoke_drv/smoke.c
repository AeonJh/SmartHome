#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/random.h>
unsigned canread = 0;
						//  高字节 低字节
unsigned char kbuf[10]={0xff,0x86,0,0,0,0,0,0,0xf5};
unsigned long smoke = 0;
static int smoke_open(struct inode *inode, struct file *file)
{
	kbuf[2] =  smoke/256;
	kbuf[3] =  smoke%256;
	printk(KERN_EMERG "smoke_open \n");
	return 0;
}

ssize_t smoke_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{	
	uint n;
	if(canread == 0)
	{
		printk(KERN_EMERG, " plesae input correct command!\n");
		return 0;
	}
	get_random_bytes(&smoke, sizeof(unsigned long));
	smoke = smoke%2000;
	
	kbuf[2] =  smoke/256;
	kbuf[3] =  smoke%256;
	
	n = copy_to_user(buf, kbuf, 9);
	 
	return size - n; //成功读到了多少字节
}
static ssize_t smoke_write(struct file *file, const char __user *buf, size_t size, loff_t * off)
{
	unsigned char kbuf[10]={0};
	uint n;
	n = copy_from_user(kbuf, buf, size);
	if(kbuf[0]==0xFF && kbuf[1]==0x01 && kbuf[2]==0x86)
	{
		printk(KERN_EMERG, "write cmd is correct!\n");
		canread =  1;
	}
	else
	{
		printk(KERN_EMERG, "write cmd is wrong!\n");
		canread = 0;
	}
	return size-n;
}

static int smoke_close(struct inode *inode, struct file *file)
{

	printk(KERN_EMERG "smoke_close \n");
	return 0;
}

static struct file_operations smoke_ops=
{
	.open = smoke_open,
	.read = smoke_read,
	.write = smoke_write,
	.release = smoke_close
};


static struct miscdevice miscsmoke={
	.minor = MISC_DYNAMIC_MINOR,   //动态分配次设备号
	.name = "ttySAC1",  //   /dev/mysmoke
	.fops = &smoke_ops,
};


static int __init smoke_init(void)
{
	//2. 注册
	int ret;
	ret = misc_register(&miscsmoke);
	if(ret < 0)
	{
		printk(KERN_EMERG "misc_register failed\n");
		return -1;
	}
	printk(KERN_EMERG "smoke module register success\n");
	return 0;
}

static void __exit smoke_exit(void)
{
	misc_deregister(&miscsmoke);
	printk(KERN_INFO "smoke module deleted success\n");
}

module_init(smoke_init);
module_exit(smoke_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("huang fu");
