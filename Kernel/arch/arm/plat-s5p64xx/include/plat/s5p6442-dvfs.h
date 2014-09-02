/* /arch/arm/plat-s5p6442/include/plat/s5P6442-dvfs.h
 *
 * Copyright (c) 2009 Samsung Electronics
  *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __PLAT_S5P6442_DVFS_H
#define __PLAT_S5P6442_DVFS_H __FILE__
#include <linux/cpufreq.h>
#define MAXIMUM_FREQ 600000
//#define MAXIMUM_FREQ 667000
//#undef USE_DVS
#define USE_DVS
#define SYSCLK_CHANGE
#define USE_DVFS_AL1_LEVEL
//#undef USE_DVFS_AL1_LEVEL
//#define CLK_DIV_CHANGE_SEQ
//#define MUXD0D1_A2M

#define KHZ_T		1000

#define MPU_CLK		"clk_cpu"

extern unsigned int S5P6442_FREQ_TAB;
extern unsigned int S5P6442_MAXFREQLEVEL;

#ifdef CONFIG_800MHZ_EDITION
#ifndef CONFIG_BLEEDING_EDGE
extern unsigned int frequency_match_666_166MHz[][4];
extern u32 s5p_cpu_pll_tab[][2];
int s5p6442_clk_set_rate(unsigned int target_freq, unsigned int index );
#endif
#endif
#define INDX_ERROR  65535

#ifndef CONFIG_800MHZ_EDITION
extern unsigned int s5p6442_cpufreq_index;
extern int set_voltage(unsigned int);
extern unsigned int s5p6442_target_frq(unsigned int pred_freq, int flag);
extern void set_dvfs_level(int flag);
extern void set_dvfs_perf_level(void);
extern void set_dvfs_doclk_level(int flag);
extern void s5p6442_changeDivider(u32, u32);
extern int cpufreq_set_policy(unsigned int cpu, const char *buf); 
extern char cpufreq_governor_name[CPUFREQ_NAME_LEN];
extern void cpufreq_get_cpufreq_name(unsigned int cpu);
extern void dvfs_set_max_freq_lock(void);
extern void dvfs_set_max_freq_unlock(void);

#else //ifndef CONFIG_800MHZ_EDITION

int set_voltage(unsigned int freq_index, bool);
extern int *FakeShmoo_UV_mV_Ptr; // Stored voltage table from cpufreq sysfs
extern void s5p6442_preclock();
extern void s5p6442_postclock();

extern unsigned int s5p6442_cpufreq_index;

extern unsigned int s5p6442_target_frq(unsigned int pred_freq, int flag);
extern void set_dvfs_level(int flag);
extern void set_dvfs_perf_level(void);
extern void set_dvfs_doclk_level(int flag);
extern void s5p6442_changeDivider(u32, u32);
extern void dvfs_set_max_freq_lock(void);
extern void dvfs_set_max_freq_unlock(void);

#ifdef CONFIG_BLEEDING_EDGE_ULTRA
#define MUXD0D1_A2M
#endif

#ifdef CONFIG_BLEEDING_EDGE
int s5p6442_clk_set_rate(unsigned int target_freq, unsigned int index );
#endif

#ifdef CONFIG_BLEEDING_EDGE_ULTRA
extern unsigned int frequency_match[][4];
extern void s5p6442_clock800(void);
#endif

#endif //ifndef CONFIG_800MHZ_EDITION
#endif /* __PLAT_S3C64XX_DVFS_H */
