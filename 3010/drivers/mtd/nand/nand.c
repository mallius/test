/*
 * (C) Copyright 2005
 * 2N Telekomunikace, a.s. <www.2n.cz>
 * Ladislav Michl <michl@2n.cz>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <nand.h>

#ifndef CONFIG_SYS_NAND_BASE_LIST
#define CONFIG_SYS_NAND_BASE_LIST { CONFIG_SYS_NAND_BASE } //0x4e00_0000
#endif

DECLARE_GLOBAL_DATA_PTR;

int nand_curr_device = -1;
nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];		//nand_info_t = mtd_info

static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
static ulong base_address[CONFIG_SYS_MAX_NAND_DEVICE] = CONFIG_SYS_NAND_BASE_LIST;	//base_address[1] = 0x4e00_0000;

static const char default_nand_name[] = "nand";
static __attribute__((unused)) char dev_name[CONFIG_SYS_MAX_NAND_DEVICE][8];

static void nand_init_chip(struct mtd_info *mtd, struct nand_chip *nand,
			   ulong base_addr)
{
	int maxchips = CONFIG_SYS_NAND_MAX_CHIPS; 		// =1
	int __attribute__((unused)) i = 0;

	if (maxchips < 1)								//maxchips = 1
		maxchips = 1;
	mtd->priv = nand;								//void *priv -> nand_chip

	nand->IO_ADDR_R = nand->IO_ADDR_W = (void  __iomem *)base_addr;
/**************************************************************************/
	if (board_nand_init(nand) == 0) {	// return 0;
		if (nand_scan(mtd, maxchips) == 0) {
			if (!mtd->name)
				mtd->name = (char *)default_nand_name;
#ifndef CONFIG_RELOC_FIXUP_WORKS
			else
				mtd->name += gd->reloc_off;
#endif

		} else
			mtd->name = NULL;
	} else {
		mtd->name = NULL;
		mtd->size = 0;
	}
/**************************************************************************/

}

void nand_init(void)
{
	int i;
	unsigned int size = 0;
	for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++) { // = 1 , include/configs/smdk2440a.h
		nand_init_chip(&nand_info[i], &nand_chip[i], base_address[i]);	//base_address = 0x4e00,0000 -->NFCONF
					//struct nand_info --> 关于芯片, struct nand_chip --> 关于操作
		size += nand_info[i].size / 1024;
		if (nand_curr_device == -1)
			nand_curr_device = i;
	}
	printf("%u MiB HOHO~\n", size / 1024);				//打印出来的MiB


}
