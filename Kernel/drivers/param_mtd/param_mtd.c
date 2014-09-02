/*
 * param.c
 *
 * Parameter read & save driver on param partition.
 *
 * COPYRIGHT(C) Samsung Electronics Co.Ltd. 2006-2010 All Right Reserved.
 *
 * 2008.02.26. Supprot for BML layer.
 * 2009.12.07. Modified to support for FSR_BML
 * 2010.04.22. Remove FSR_BML
 * 2013.04.14  Support for BML over MTD
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/ctype.h>
#include <linux/vmalloc.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mtd/mtd.h>

#include <mach/hardware.h>
#include <mach/param.h>

typedef param_status_t status_t;

#define PARAM_PROCFS_DEBUG
extern int factorytest;

#ifdef PARAM_PROCFS_DEBUG
struct proc_dir_entry *param_dir;
#endif

unsigned char *vaddr = NULL;
#define PARAM_SIZE 524288

/* added by geunyoung for LFS. */
static int load_lfs_param_value(void);
static int save_lfs_param_value(void);

static int param_check(unsigned char *addr)
{
	status_t *status;
	status = (status_t *)addr;

	if ((status->param_magic == PARAM_MAGIC) &&
			(status->param_version == PARAM_VERSION)) {
		klogi("Checking PARAM... OK");
		return 0;
	}

	klogi("Checking PARAM... Invalid");
	return -1;
}

void set_param_value(int idx, void *value)
{
	int i, str_i;
	param_status_t *addr;

	addr = (param_status_t *) vaddr;

	for (i = 0; i < MAX_PARAM; i++) {
		if (i < (MAX_PARAM - MAX_STRING_PARAM)) {
			if(addr->param_list[i].ident == idx) {
				addr->param_list[i].value = *(int *)value;
			}
		}
		else {
			str_i = (i - (MAX_PARAM - MAX_STRING_PARAM));
			if(addr->param_str_list[str_i].ident == idx) {
				strlcpy(addr->param_str_list[str_i].value,
					(char *)value, PARAM_STRING_SIZE);
			}
		}
	}

	save_lfs_param_value();
}
EXPORT_SYMBOL(set_param_value);

void get_param_value(int idx, void *value)
{
	int i, str_i;
	param_status_t *addr;
	addr = (param_status_t *) vaddr;

	for (i = 0 ; i < MAX_PARAM; i++) {
		if (i < (MAX_PARAM - MAX_STRING_PARAM)) {
			if(addr->param_list[i].ident == idx) {
				*(int *)value = addr->param_list[i].value;
			}
		}
		else {
			str_i = (i - (MAX_PARAM - MAX_STRING_PARAM));
			if(addr->param_str_list[str_i].ident == idx) {
				strlcpy((char *)value,
					addr->param_str_list[str_i].value, PARAM_STRING_SIZE);
			}
		}
	}
}
EXPORT_SYMBOL(get_param_value);

static void param_set_default(void)
{
	param_status_t *addr;
	addr = (param_status_t *) vaddr;

	memset(addr, 0, sizeof(status_t));

		addr->param_magic = PARAM_MAGIC;
		addr->param_version = PARAM_VERSION;
		addr->param_list[0].ident = __SERIAL_SPEED;
		addr->param_list[0].value = SERIAL_SPEED;
		addr->param_list[1].ident = __LOAD_RAMDISK;
		addr->param_list[1].value = LOAD_RAMDISK;
		addr->param_list[2].ident = __BOOT_DELAY;
		addr->param_list[2].value = BOOT_DELAY;
		addr->param_list[3].ident = __LCD_LEVEL;
		addr->param_list[3].value = LCD_LEVEL;
		addr->param_list[4].ident = __SWITCH_SEL;
		addr->param_list[4].value = SWITCH_SEL;
		addr->param_list[5].ident = __PHONE_DEBUG_ON;
		addr->param_list[5].value = PHONE_DEBUG_ON;
		addr->param_list[6].ident = __LCD_DIM_LEVEL;
		addr->param_list[6].value = LCD_DIM_LEVEL;
		addr->param_list[7].ident = __MELODY_MODE;
		addr->param_list[7].value = MELODY_MODE;
		addr->param_list[8].ident = __REBOOT_MODE;
		addr->param_list[8].value = REBOOT_MODE;
		addr->param_list[9].ident = __NATION_SEL;
		addr->param_list[9].value = NATION_SEL;
		addr->param_list[10].ident = __SET_DEFAULT_PARAM;
		addr->param_list[10].value = SET_DEFAULT_PARAM;

	addr->param_str_list[0].ident = __VERSION;
	strlcpy(addr->param_str_list[0].value,
			VERSION_LINE, PARAM_STRING_SIZE);
	addr->param_str_list[1].ident = __CMDLINE;
	strlcpy(addr->param_str_list[1].value,
			COMMAND_LINE, PARAM_STRING_SIZE);
}

#ifdef PARAM_PROCFS_DEBUG
static void param_show_info(void)
{
	param_status_t *addr;
	addr = (param_status_t *) vaddr;

	klogi("-----------------------------------------------------");
	klogi("	Information of Parameters");
	klogi("-----------------------------------------------------");
	klogi("  -     param_magic	  : 0x%x", addr->param_magic);
	klogi("  -     param_version	  : 0x%x", addr->param_version);
	klogi("  - %2d. SERIAL_SPEED	  : %d", addr->param_list[0].ident, addr->param_list[0].value);
	klogi("  - %2d. LOAD_RAMDISK	  : %d", addr->param_list[1].ident, addr->param_list[1].value);
	klogi("  - %2d. BOOT_DELAY	  : %d", addr->param_list[2].ident, addr->param_list[2].value);
	klogi("  - %2d. LCD_LEVEL	  : %d", addr->param_list[3].ident, addr->param_list[3].value);
	klogi("  - %2d. SWITCH_SEL	  : %d", addr->param_list[4].ident, addr->param_list[4].value);
	klogi("  - %2d. PHONE_DEBUG_ON	  : %d", addr->param_list[5].ident, addr->param_list[5].value);
	klogi("  - %2d. LCD_DIM_LEVEL	  : %d", addr->param_list[6].ident, addr->param_list[6].value);
	klogi("  - %2d. MELODY_LEVEL	  : %d", addr->param_list[7].ident, addr->param_list[7].value);
	klogi("  - %2d. REBOOT_MODE	  : %d", addr->param_list[8].ident, addr->param_list[8].value);
	klogi("  - %2d. NATION_SEL	  : %d", addr->param_list[9].ident, addr->param_list[9].value);
	klogi("  - %2d. SET_DEFAULT_PARAM  : %d", addr->param_list[10].ident, addr->param_list[10].value);
	klogi("  - %2d. VERSION(STR)	  : %s", addr->param_str_list[0].ident, addr->param_str_list[0].value);
	klogi("  - %2d. CMDLINE(STR)	  : %s", addr->param_str_list[1].ident, addr->param_str_list[1].value);
	klogi("-----------------------------------------------------");
}

/* test codes for debugging */
static int param_run_test(void)
{
#if 1  /* For the purpose of testing... */
	int val=3;
	if (!sec_set_param_value)
		return -1;

	sec_set_param_value(__SWITCH_SEL, &val);
#endif
	return 0;
}

static int param_lfs_run_test(void)
{
	return load_lfs_param_value();
}

static int param_lfs_set_recovery_mode(void)
{
	int val=4;
	if (!sec_set_param_value)
		return -1;

	sec_set_param_value(__REBOOT_MODE, &val);
	param_show_info();
	return 0;
}

static int param_read_proc_debug(char *page, char **start, off_t offset,
					int count, int *eof, void *data)
{
	*eof = 1;
	return sprintf(page, "0. show parameters\n\
1. initialize parameters\n\
2. run test function\n\
example: echo [number] > /proc/param/debug\n");
}

static int param_write_proc_debug(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	char *buf;

	if (count < 1)
		return -EINVAL;

	buf = kmalloc(count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	if (copy_from_user(buf, buffer, count)) {
		kfree(buf);
		return -EFAULT;
	}

	switch(buf[0]) {
		case '0':
			param_show_info();
			break;
		case '1':
			param_set_default();
			save_lfs_param_value();
			klogi("Parameters have been set as DEFAULT values...");
			param_show_info();
			break;
		case '2':
			param_run_test();
			break;
		case '3':
			param_lfs_run_test();
			break;
                case '4':
                        param_lfs_set_recovery_mode();
		default:
			kfree(buf);
			return -EINVAL;
	}

	kfree(buf);
	return count;
}
#endif  /* PARAM_PROCFS_DEBUG */

static int param_init(void)
{
	int ret;
#ifdef PARAM_PROCFS_DEBUG
	struct proc_dir_entry *ent;

	/* Creats '/proc/param' directory */
	param_dir = proc_mkdir("param", NULL);
	if (param_dir == NULL) {
		kloge("Unable to create /proc/param directory\n");
		return -ENOMEM;
	}

	/* Creats RW '/proc/param/sleep/debug' entry */
	ent = create_proc_entry("debug", 0, param_dir);
	if (ent == NULL) {
		kloge("Unable to create /proc/param/debug entry");
		ret = -ENOMEM;
		goto fail;
	}
	ent->read_proc = param_read_proc_debug;
	ent->write_proc = param_write_proc_debug;
#endif

	klogi("param_init");

	vaddr = vmalloc(PARAM_SIZE);
	if (vaddr == NULL) {
		kloge("Unable to allocate memory");
		ret = -ENOMEM;
		goto fail;
	}
#if 0
	ret = load_param_value();
#else
	ret = load_lfs_param_value();
#endif
	if (ret < 0) {
		kloge("Loading parameters failed. Parameters have been initialized as default.");
		param_set_default();
	}

	sec_set_param_value = set_param_value;
	sec_get_param_value = get_param_value;

	return 0;

#ifdef PARAM_PROCFS_DEBUG
fail:
	remove_proc_entry("param", 0);

	return ret;
#endif
}

static void param_exit(void)
{
	klogi("param_exit");
	vfree(vaddr);

#ifdef PARAM_PROCFS_DEBUG
	remove_proc_entry("debug", param_dir);
	remove_proc_entry("param", 0);
#endif
}

module_init(param_init);
module_exit(param_exit);

/* added by geunyoung for LFS. */
#define PARAM_FILE_NAME	"/efs/param.bin"
#define PARAM_RD	0
#define PARAM_WR	1

  
void erase_callback (struct erase_info *instr) {
    wake_up((wait_queue_head_t *)instr->priv);
}

void erase_sector(struct mtd_info *mtd_info, unsigned int start, unsigned int len) 
 
{
    int ret;
    struct erase_info ei = {0};
    wait_queue_head_t waitq;
    DECLARE_WAITQUEUE(wait, current);
     
    init_waitqueue_head(&waitq);
    ei.addr = start;
    ei.len = mtd_info->erasesize;
    ei.mtd = mtd_info;
    ei.callback = erase_callback;
    ei.priv = (unsigned long)&waitq;
    ret = mtd_info->erase(mtd_info, &ei);
    if(!ret)     {
        set_current_state(TASK_UNINTERRUPTIBLE);
        add_wait_queue(&waitq, &wait);
        if (ei.state != MTD_ERASE_DONE && ei.state != MTD_ERASE_FAILED)
            schedule();
        remove_wait_queue(&waitq, &wait);
        set_current_state(TASK_RUNNING);
  
        ret = (ei.state == MTD_ERASE_FAILED)?-EIO:0;
    }
}


void erase_partition(struct mtd_info *mtd_info) {
    unsigned int start;
    for(start = 0; start < mtd_info->size; start += mtd_info->erasesize) 
        erase_sector(mtd_info, start, mtd_info->erasesize);
}

static int lfs_param_op(int dir, int flags)
{

	struct mtd_info *mtd;

	unsigned int retlen;
	int ret;

	mtd = get_mtd_device_nm("param");

	if (IS_ERR(mtd)) {
		kloge("(%s)Cannot find param partition.\n",__func__);
		return -1;
	}	


	if (dir == PARAM_RD) 
		ret = mtd->read(mtd, 0, PARAM_SIZE, &retlen, (char __user *)vaddr);
	else {
		erase_partition(mtd);
                ret = mtd->write(mtd, 0, PARAM_SIZE, &retlen, (char __user *)vaddr);
	}

	put_mtd_device(mtd);

	return ret;
}

static int load_lfs_param_value(void)
{
	int ret;

	ret = lfs_param_op(PARAM_RD, O_RDONLY);

	if (ret == PARAM_SIZE) {
		pr_info("%s: param.blk read successfully.\n", __FUNCTION__);
	}
	param_check(vaddr);
	
	return ret;
}

static int save_lfs_param_value(void)
{
	int ret;
	klogi("save_lfs_param_value");
	ret = lfs_param_op(PARAM_WR, O_RDWR|O_SYNC);

	if (ret == PARAM_SIZE) {
		pr_info("%s: param.blk write successfully.\n", __FUNCTION__);
	}

	return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sebastien Dadier <s.dadier at gmail.com>");
MODULE_DESCRIPTION("Param module for s5p6442");

