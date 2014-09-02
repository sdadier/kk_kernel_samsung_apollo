/* linux/drivers/mtd/onenand/samsung_epic.h
 *
 * Partition Layout for Samsung Apollo
 *
 */

struct mtd_partition s3c_partition_info[] = {

 /*This is partition layout from the oneNAND it SHOULD match the pitfile on the second page of the NAND.
   It will work if it doesn't but beware to write below the adress 0x01200000 there are the bootloaders.
   Currently we won't map them, but we should keep that in mind for later things like flashing bootloader
   from Linux. There is a partition 'efs' starting @ 0x00080000 40 256K pages long, it contains data for
   the modem like IMSI we don't touch it for now, but we need the data from it, we create a partition
   for that and copy the data from it. For this you need a image from it and mount it as vfat or copy
   it on a kernel with rfs support on the phone.

   Partitions on the lower NAND adresses:

   0x00000000 - 0x0003FFFF = first stage bootloader
   0x00040000 - 0x0007FFFF = PIT for second stage bootloader
   0x00080000 - 0x00A7FFFF = EFS: IMSI and NVRAM for the modem
   0x00A80000 - 0x00BBFFFF = second stage bootloader
   0x00BC0000 - 0x00CFFFFF = backup of the second stage bootloader (should be loaded if the other fails, unconfirmed!)
   0x00D00000 - 0x011FFFFF = PARAM.lfs config the bootloader

   #########################################################################################
   #########################################################################################
   ###### NEVER TOUCH THE FIRST 2 256k PAGES! THEY CONTAIN THE FIRST STAGE BOOTLOADER ######
   #########################################################################################
   #########################################################################################*/

	{
		.name		= "boot",
		.offset		= (40*SZ_256K),
		.size		= (30*SZ_256K), 
	},
	{
		.name		= "system",
		.offset		= (70*SZ_256K),
		.size		= (880*SZ_256K), 
	},
	{
		.name		= "userdata",
		.offset		= (950*SZ_256K),
		.size		= (810*SZ_256K), //810
	},
	{
		.name		= "cache",
		.offset		= (1760*SZ_256K),
		.size		= (148*SZ_256K), 
	},
	{
		.name		= "efs",
		.offset		= (1908*SZ_256K),
		.size		= (96*SZ_256K), 
	},
/*
        {
		.name		= "legacy_modem",
		.offset		= (1940*SZ_256K),
		.size		= (64*SZ_256K), 
	},
*/
        { /* The reservoir area is used by Samsung's Block Management Layer (BML)
             to map good blocks from this reservoir to bad blocks in user
             partitions. A special tool (bml_over_mtd) is needed to write
             partition images using bad block mapping.
             Currently, this is required for flashing the "boot" partition,
             as Samsung's stock bootloader expects BML partitions.*/
                .name           = "reservoir",
                .offset         = (2004*SZ_256K),
                .size           = (43*SZ_256K), 
        },
        {
		.name		= "dgs",
		.offset		= (2047*SZ_256K),
		.size		= (1*SZ_256K), 
	},
        {
		.name		= "param",
		.offset		= (6*SZ_256K),
		.size		= (2*SZ_256K), 
	},
};


/* INFORMATIONS TAKEN FROM APOLLO BOOTLOADER

==== PARTITION INFORMATION ====
 ID         : IBL & PBL (0x0)
 ATTR       : RO SLC PREWRITING (0x9002)
 FIRST_UNIT : 0
 NO_UNITS   : 1
===============================
 ID         : SBL (0x1)
 ATTR       : RO ENTRYPOINT SLC BOOTLOADING PREWRITING (0xd402)
 FIRST_UNIT : 1
 NO_UNITS   : 5
===============================
 ID         : PARAM (0x2)
 ATTR       : RW SLC (0x1001)
 FIRST_UNIT : 6
 NO_UNITS   : 2
===============================
 ID         : EFS (0x18)
 ATTR       : RW STL SLC (0x1101)
 FIRST_UNIT : 8
 NO_UNITS   : 32
===============================
 ID         : KERNEL (0x4)
 ATTR       : RO SLC PREWRITING (0x9002)
 FIRST_UNIT : 40
 NO_UNITS   : 30
===============================
 ID         : FACTORYFS (0x15)
 ATTR       : RW STL SLC (0x1101)
 FIRST_UNIT : 70
 NO_UNITS   : 880
===============================
 ID         : DATAFS (0x16)
 ATTR       : RW STL SLC (0x1101)
 FIRST_UNIT : 950
 NO_UNITS   : 810
===============================
 ID         : CACHE (0x17)
 ATTR       : RW STL SLC (0x1101)
 FIRST_UNIT : 1760
 NO_UNITS   : 152 (*mismatch* 152 should be 150)
===============================
 ID         : TEMP (0x3)
 ATTR       : RW SLC (0x1001)
 FIRST_UNIT : 1912
 NO_UNITS   : 28 (*mismatch* 28 should be 30)
===============================
 ID         : MODEM (0x5)
 ATTR       : RO SLC PREWRITING (0x9002)
 FIRST_UNIT : 1940
 NO_UNITS   : 64
===============================

*/
