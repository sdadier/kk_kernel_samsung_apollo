/*
 *  linux/arch/arm/mm/nommu.c
 *
 * ARM uCLinux supporting functions.
 */
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/bootmem.h>
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <asm/sections.h>
#include <asm/page.h>
#include <asm/setup.h>
#include <asm/mach/arch.h>

#include "mm.h"

/*
 * Reserve the various regions
 */
void __init reserve_special_regions(void)
{
	/*
	 * Register the kernel text and data with bootmem.
	 * Note that this can only be in node 0.
	 */
#ifdef CONFIG_XIP_KERNEL
	reserve_bootmem(__pa(_data), _end - _data, BOOTMEM_DEFAULT);
#else
	reserve_bootmem(__pa(_stext), _end - _stext, BOOTMEM_DEFAULT);
#endif

	/*
	 * Register the exception vector page.
	 * some architectures which the DRAM is the exception vector to trap,
	 * alloc_page breaks with error, although it is not NULL, but "0."
	 */
	reserve_bootmem(CONFIG_VECTORS_BASE, PAGE_SIZE, BOOTMEM_DEFAULT);
}

/*
 * paging_init() sets up the page tables, initialises the zone memory
 * maps, and sets up the zero page, bad page and bad page tables.
 */
void __init paging_init(struct machine_desc *mdesc)
{
	bootmem_init();
}

/*
 * We don't need to do anything here for nommu machines.
 */
void setup_mm_for_reboot(char mode)
{
}

void flush_dcache_page(struct page *page)
{
	__cpuc_flush_dcache_page(page_address(page));
}
EXPORT_SYMBOL(flush_dcache_page);

void __iomem *__arm_ioremap_pfn(unsigned long pfn, unsigned long offset,
				size_t size, unsigned int mtype)
{
	if (pfn >= (0x100000000ULL >> PAGE_SHIFT))
		return NULL;
	return (void __iomem *) (offset + (pfn << PAGE_SHIFT));
}
EXPORT_SYMBOL(__arm_ioremap_pfn);

void __iomem *__arm_ioremap(unsigned long phys_addr, size_t size,
			    unsigned int mtype)
{
	return (void __iomem *)phys_addr;
}
EXPORT_SYMBOL(__arm_ioremap);

void __iounmap(volatile void __iomem *addr)
{
}
EXPORT_SYMBOL(__iounmap);
