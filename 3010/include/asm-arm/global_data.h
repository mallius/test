/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 */

#ifndef	__ASM_GBL_DATA_H
#define __ASM_GBL_DATA_H
/*
 * The following data structure is placed in some memory wich is
 * available very early after boot (like DPRAM on MPC8xx/MPC82xx, or
 * some locked parts of the data cache) to allow for a minimum set of
 * global variables during system initialization (until we have set
 * up the memory controller so that we can use RAM).
 *
 * Keep it *SMALL* and remember to set CONFIG_SYS_GBL_DATA_SIZE > sizeof(gd_t)
 */

typedef	struct	global_data {
	bd_t		*bd;				//指向板级信息结构
	unsigned long	flags;			//标记位
	unsigned long	baudrate;		//串口波特率
	unsigned long	have_console;	/* serial_init() was called */
	unsigned long	env_addr;	/* Address  of Environment struct */	//环境参数地址
	unsigned long	env_valid;	/* Checksum of Environment valid? */	//环境参数CRC校验有效标志
	unsigned long	fb_base;	/* base address of frame buffer */		//fb起始地址

	void		**jt;		/* jump table */						//跳转函数表
} gd_t;

/*
 * Global Data Flags
 */
#define	GD_FLG_RELOC	0x00001		/* Code was relocated to RAM*/  		//代码已经转移到RAM
#define	GD_FLG_DEVINIT	0x00002		/* Devices have been initialized	*/ 	//设备已经完成初始化
#define	GD_FLG_SILENT	0x00004		/* Silent mode				*/			//静音模式
#define	GD_FLG_POSTFAIL	0x00008		/* Critical POST test failed		*/	
#define	GD_FLG_POSTSTOP	0x00010		/* POST seqeunce aborted		*/
#define	GD_FLG_LOGINIT	0x00020		/* Log Buffer has been initialized	*/
#define GD_FLG_DISABLE_CONSOLE	0x00040		/* Disable console (in & out)	 */
//定义一个寄存器变量，占用寄存器r8，作为 gd_t 的全局指针
#define DECLARE_GLOBAL_DATA_PTR     register volatile gd_t *gd asm ("r8")

#endif /* __ASM_GBL_DATA_H */
