/*
 Storage Driver for s5p6442
 
 This driver will read the dgs partition from mtd onenand driver

*/

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>

#include <asm/uaccess.h>

#define SIZ_DGS_INFO		0x00000100
#define DPRAM_DGS_INFO_BLOCK_SIZE                       0x100


static int count;

static int
storage_open(struct inode *inode, struct file *file)
{
        count++;
        printk("================================================================\n");
	printk("Storage device (count=%d)\n", count);
        printk("================================================================\n");
	return 0;
}

static int
storage_ioctl(struct inode *inode, struct file *file,
             unsigned int cmd, unsigned long arg)
{

	struct mtd_info *mtd;
	size_t retlen = 0;
	int ret;
        unsigned char *temp = kmalloc(4096, GFP_KERNEL);

        printk("================================================================\n");
        printk("(%s) cmd = %x arg = %x\n", __func__, cmd, arg);
        printk("================================================================\n");

	mtd = get_mtd_device_nm("dgs");
	if (IS_ERR(mtd)) {
		printk("(%s)Cannot find dgs partition.\n",__func__);
		return PTR_ERR(mtd);
	}

	ret = mtd->read(mtd, 0,
					SIZ_DGS_INFO, &retlen, temp);
	if (ret || retlen != SIZ_DGS_INFO) {
		printk("(%s)Failed to read dgs block.\n", __func__);
		put_mtd_device(mtd);
		return (ret) ? ret : -EIO;
	}

	put_mtd_device(mtd);

        ret = copy_to_user((void __user *)arg, temp, DPRAM_DGS_INFO_BLOCK_SIZE);
	return ret;
}

static const struct file_operations storage_fops = {
        .owner                = THIS_MODULE,
	.open		      = storage_open,
        .ioctl                = storage_ioctl,
};

static struct miscdevice storage_dev = {
        MISC_DYNAMIC_MINOR,
        "storage",
        &storage_fops
};


static int __init
storage_init(void)
{
        int ret;
        printk("Storage driver (mtd) for apollo\n");

        ret = misc_register(&storage_dev);
        if (ret)
                printk(KERN_ERR
                       "Unable to register storage device\n");

	count = 0;

        return ret;
}

module_init(storage_init);

static void __exit
storage_exit(void)
{
        misc_deregister(&storage_dev);
}

module_exit(storage_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sebastien Dadier <s.dadier at gmail.com>");
MODULE_DESCRIPTION("Storage module for s5p6442");

