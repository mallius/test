/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 *
 ********************************************************************
 * NOTE: This header file defines an interface to U-Boot. Including
 * this (unmodified) header file in another file is considered normal
 * use of U-Boot, and does *not* fall under the heading of "derived
 * work".
 ********************************************************************
 */

#ifndef _U_BOOT_H_
#define _U_BOOT_H_	1

typedef struct bd_info {					//指向板级信息结构
    int			bi_baudrate;				/* serial console baudrate */
    unsigned long	bi_ip_addr;				/* IP Address */
    struct environment_s	       *bi_env;
    ulong	        bi_arch_number;			/* unique id for this board */
    ulong	        bi_boot_params;			/* where this board expects params */
    struct									/* RAM configuration */
    {
	ulong start;
	ulong size;
    }			bi_dram[CONFIG_NR_DRAM_BANKS];		// 只有1个 BANK
} bd_t;

#define bi_env_data bi_env->data
#define bi_env_crc  bi_env->crc

#endif	/* _U_BOOT_H_ */
