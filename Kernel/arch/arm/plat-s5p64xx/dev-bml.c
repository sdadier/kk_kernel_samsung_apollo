/* linux/arch/arm/plat-s5p/dev-onenand.c
 *
 * Copyright 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 *  Copyright (c) 2008-2010 Samsung Electronics
 *  Kyungmin Park <kyungmin.park@samsung.com>
 *
 * S5P series device definition for OneNAND devices
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>

#include <asm/irq.h>
#include <mach/map.h>
#include <linux/mtd/bml.h>

struct bml_partition apollo_bml_partitions[] = {
	{
		.name		= "boot",
		.offset		= (40 * SZ_256K),
	},

	{
		.name		= "modem",
		.offset		= (1940 * SZ_256K),
	},
};

struct bml_platform_data apollo_bml_pdata = {
	.parts			= apollo_bml_partitions,
	.nr_parts		= ARRAY_SIZE(apollo_bml_partitions),
};

struct platform_device apollo_bml_device = {
	.name			= "mtd-bml",
	.id			= -1,
	.dev			= {
				.platform_data = &apollo_bml_pdata,
	},
};
