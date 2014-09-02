/*
 * param.c
 *
 * Parameter read & save driver on param file.
 *
 * COPYRIGHT(C) Samsung Electronics Co.Ltd. 2006-2010 All Right Reserved.
 *
 * Author: Jeonghwan Min <jeonghwan.min@samsung.com>
 *
 * modified by Sandor to use /efs/param.bin on GT-I8000 instead of BML partition 
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/blkpg.h>
#include <linux/hdreg.h>
#include <linux/genhd.h>
#include <linux/sched.h>
#include <linux/ctype.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

#include <linux/fs.h>
#include <linux/mtd/mtd.h>

#include <mach/hardware.h>
#include <mach/param.h>

#define PARAM_LEN                      (1024)


int is_valid_param(param_status_t *status)
{
	return (status->param_magic == PARAM_MAGIC) &&
		(status->param_version == PARAM_VERSION);
}

static param_status_t param_status;

static int load_param_value(void)
{
	unsigned char *addr = NULL;
	unsigned int ret = 0;
	struct mtd_info *mtd;
	unsigned int retlen;
	addr = vmalloc(PARAM_LEN);

	if (!addr)
		return -ENOMEM;

        preempt_enable();
	mtd = get_mtd_device_nm("param");
	if (IS_ERR(mtd)) {
		printk("(%s)Cannot find param partition.\n",__func__);
		goto fail;
	}

	ret = mtd->read(mtd, 0x30000, PARAM_LEN, &retlen, addr);


	if (is_valid_param((param_status_t *)addr)) {
		printk("param is valid\n");
		memcpy(&param_status, addr, sizeof(param_status_t));
	}

	int i;

	printk("printing bytes (read)\n");
	for (i=0; i < 1024; i++)
		printk("%x", addr[i]);

	put_mtd_device(mtd);
fail:
	vfree(addr);
        preempt_disable();

	return ret;
}

int save_param_value(void)
{
	unsigned int err = 0;
	unsigned char *addr = NULL;
	unsigned int retlen, ret;
	struct mtd_info *mtd;

	addr = vmalloc(PARAM_LEN);
	if (!addr)
		return -ENOMEM;

        preempt_enable();
	mtd = get_mtd_device_nm("param");
	if (IS_ERR(mtd)) {
		printk("(%s)Cannot find param partition.\n",__func__);
		goto fail;
	}

	ret = mtd->read(mtd, 0x30000, PARAM_LEN, &retlen, addr);

	// update MAIN
	memset(addr, 0, PARAM_LEN);
	memcpy(addr, &param_status, sizeof(param_status_t));

	ret = mtd->unlock(mtd, 0x30000, PARAM_LEN);

	ret = mtd->write(mtd, 0x30000, PARAM_LEN, &retlen, addr);

	int i;

	printk("printing bytes (write)\n");
	for (i=0; i < 1024; i++)
		printk("%x", addr[i]);

	put_mtd_device(mtd);

fail:
	vfree(addr);
        preempt_disable();

	return err;
}
EXPORT_SYMBOL(save_param_value);

void set_param_value(int idx, void *value)
{
	int i, str_i;
	
	for (i = 0; i < MAX_PARAM; i++) {
		if (i < (MAX_PARAM - MAX_STRING_PARAM)) {
			if(param_status.param_list[i].ident == idx) {
				param_status.param_list[i].value = *(int *)value;
			}
		}
		else {
			str_i = (i - (MAX_PARAM - MAX_STRING_PARAM));
			if(param_status.param_str_list[str_i].ident == idx) {
				strlcpy(param_status.param_str_list[str_i].value,
					(char *)value, PARAM_STRING_SIZE);
			}
		}
	}

	save_param_value();
}
EXPORT_SYMBOL(set_param_value);

void get_param_value(int idx, void *value)
{
	int i, str_i;
	for (i = 0 ; i < MAX_PARAM; i++) {
		if (i < (MAX_PARAM - MAX_STRING_PARAM)) {
			if(param_status.param_list[i].ident == idx) {
				*(int *)value = param_status.param_list[i].value;
			}
		}
		else {
			str_i = (i - (MAX_PARAM - MAX_STRING_PARAM));
			if(param_status.param_str_list[str_i].ident == idx) {
				strlcpy((char *)value,
					param_status.param_str_list[str_i].value, PARAM_STRING_SIZE);
			}
		}
	}
}
EXPORT_SYMBOL(get_param_value);

static int param_init(void)
{
	int ret;

	ret = load_param_value();

	if (ret < 0) {
		printk(KERN_ERR "%s -> relocated to default value!\n", __FUNCTION__);

		memset(&param_status, 0, sizeof(param_status_t));

		param_status.param_magic = PARAM_MAGIC;
		param_status.param_version = PARAM_VERSION;
		param_status.param_list[0].ident = __SERIAL_SPEED;
		param_status.param_list[0].value = SERIAL_SPEED;
		param_status.param_list[1].ident = __LOAD_RAMDISK;
		param_status.param_list[1].value = LOAD_RAMDISK;
		param_status.param_list[2].ident = __BOOT_DELAY;
		param_status.param_list[2].value = BOOT_DELAY;
		param_status.param_list[3].ident = __LCD_LEVEL;
		param_status.param_list[3].value = LCD_LEVEL;
		param_status.param_list[4].ident = __SWITCH_SEL;
		param_status.param_list[4].value = SWITCH_SEL;
		param_status.param_list[5].ident = __PHONE_DEBUG_ON;
		param_status.param_list[5].value = PHONE_DEBUG_ON;
		param_status.param_list[6].ident = __LCD_DIM_LEVEL;
		param_status.param_list[6].value = LCD_DIM_LEVEL;
		param_status.param_list[7].ident = __MELODY_MODE;
		param_status.param_list[7].value = MELODY_MODE;
		param_status.param_list[8].ident = __REBOOT_MODE;
		param_status.param_list[8].value = REBOOT_MODE;
		param_status.param_list[9].ident = __NATION_SEL;
		param_status.param_list[9].value = NATION_SEL;
		param_status.param_list[10].ident = __SET_DEFAULT_PARAM;
		param_status.param_list[10].value = SET_DEFAULT_PARAM;
		param_status.param_str_list[0].ident = __VERSION;
		strlcpy(param_status.param_str_list[0].value,
			VERSION_LINE, PARAM_STRING_SIZE);
		param_status.param_str_list[1].ident = __CMDLINE;
		strlcpy(param_status.param_str_list[1].value,
			COMMAND_LINE, PARAM_STRING_SIZE);

	}

	sec_set_param_value = set_param_value;
	sec_get_param_value = get_param_value;

	return 0;
}

static void param_exit(void)
{
}

module_init(param_init);
module_exit(param_exit);
