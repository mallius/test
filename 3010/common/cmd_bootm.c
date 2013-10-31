/*
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 */


/*
 * Boot support
 */
#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <image.h>
#include <malloc.h>
#include <u-boot/zlib.h>
#include <bzlib.h>
#include <environment.h>
#include <lmb.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>

#if defined(CONFIG_CMD_USB)
#include <usb.h>
#endif

#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#if defined(CONFIG_OF_LIBFDT)
#include <fdt.h>
#include <libfdt.h>
#include <fdt_support.h>
#endif

#ifdef CONFIG_LZMA
#include <lzma/LzmaTypes.h>
#include <lzma/LzmaDec.h>
#include <lzma/LzmaTools.h>
#endif /* CONFIG_LZMA */

#ifdef CONFIG_LZO
#include <linux/lzo.h>
#endif /* CONFIG_LZO */

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_SYS_BOOTM_LEN
#define CONFIG_SYS_BOOTM_LEN	0x800000	/* use 8MByte as default max gunzip size */
#endif

#ifdef CONFIG_BZIP2
extern void bz_internal_error(int);
#endif

#if defined(CONFIG_CMD_IMI)
static int image_info (unsigned long addr);
#endif

#if defined(CONFIG_CMD_IMLS)
#include <flash.h>
extern flash_info_t flash_info[]; /* info for FLASH chips */
static int do_imls (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif

#ifdef CONFIG_SILENT_CONSOLE
static void fixup_silent_linux (void);
#endif

static image_header_t *image_get_kernel (ulong img_addr, int verify);
#if defined(CONFIG_FIT)
static int fit_check_kernel (const void *fit, int os_noffset, int verify);
#endif

static void *boot_get_kernel (cmd_tbl_t *cmdtp, int flag,int argc, char *argv[],
		bootm_headers_t *images, ulong *os_data, ulong *os_len);
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

/*
 *  Continue booting an OS image; caller already has:
 *  - copied image header to global variable `header'
 *  - checked header magic number, checksums (both header & image),
 *  - verified image architecture (PPC) and type (KERNEL or MULTI),
 *  - loaded (first part of) image to header load address,
 *  - disabled interrupts.
 */
typedef int boot_os_fn (int flag, int argc, char *argv[],
			bootm_headers_t *images); /* pointers to os/initrd/fdt */

#ifdef CONFIG_BOOTM_LINUX
extern boot_os_fn do_bootm_linux;
#endif
#ifdef CONFIG_BOOTM_NETBSD
static boot_os_fn do_bootm_netbsd;
#endif

#if defined(CONFIG_LYNXKDI)
static boot_os_fn do_bootm_lynxkdi;
extern void lynxkdi_boot (image_header_t *);
#endif

#ifdef CONFIG_BOOTM_RTEMS
static boot_os_fn do_bootm_rtems;
#endif

#if defined(CONFIG_CMD_ELF)
static boot_os_fn do_bootm_vxworks;
static boot_os_fn do_bootm_qnxelf;
int do_bootvx (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
int do_bootelf (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
#endif


static boot_os_fn *boot_os[] = {
#ifdef CONFIG_BOOTM_LINUX
	[IH_OS_LINUX] = do_bootm_linux,		//IH_OS_LINUX = 5
#endif
};

ulong load_addr = CONFIG_SYS_LOAD_ADDR;	/* Default Load Address */
static bootm_headers_t images;		/* pointers to os/initrd/fdt images */

/* Allow for arch specific config before we boot */
void __arch_preboot_os(void)
{
	/* please define platform specific arch_preboot_os() */
}
void arch_preboot_os(void) __attribute__((weak, alias("__arch_preboot_os")));

#if defined(__ARM__)
  #define IH_INITRD_ARCH IH_ARCH_ARM
#elif defined(__avr32__)
  #define IH_INITRD_ARCH IH_ARCH_AVR32
#elif defined(__bfin__)
  #define IH_INITRD_ARCH IH_ARCH_BLACKFIN
#elif defined(__I386__)
  #define IH_INITRD_ARCH IH_ARCH_I386
#elif defined(__M68K__)
  #define IH_INITRD_ARCH IH_ARCH_M68K
#elif defined(__microblaze__)
  #define IH_INITRD_ARCH IH_ARCH_MICROBLAZE
#elif defined(__mips__)
  #define IH_INITRD_ARCH IH_ARCH_MIPS
#elif defined(__nios__)
  #define IH_INITRD_ARCH IH_ARCH_NIOS
#elif defined(__nios2__)
  #define IH_INITRD_ARCH IH_ARCH_NIOS2
#elif defined(__PPC__)
  #define IH_INITRD_ARCH IH_ARCH_PPC
#elif defined(__sh__)
  #define IH_INITRD_ARCH IH_ARCH_SH
#elif defined(__sparc__)
  #define IH_INITRD_ARCH IH_ARCH_SPARC
#else
# error Unknown CPU type
#endif

static void bootm_start_lmb(void)
{
	/*
#ifdef CONFIG_LMB
	ulong		mem_start;
	phys_size_t	mem_size;

	lmb_init(&images.lmb);

	mem_start = getenv_bootm_low();
	mem_size = getenv_bootm_size();

	lmb_add(&images.lmb, (phys_addr_t)mem_start, mem_size);

	arch_lmb_reserve(&images.lmb);
	board_lmb_reserve(&images.lmb);
#else
	*/
# define lmb_reserve(lmb, base, size)		//HOHO~
//#endif
}

static int bootm_start(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	void		*os_hdr;
	int		ret;

	memset ((void *)&images, 0, sizeof (images));
	images.verify = getenv_yesno ("verify");	//获取环境变量

	bootm_start_lmb();	//就在上面  #define lmb_reserve

	/* 获取镜像头，加载地址，长度 */
	os_hdr = boot_get_kernel (cmdtp, flag, argc, argv,	//return (void *)img_add;
			&images, &images.os.image_start, &images.os.image_len);
	if (images.os.image_len == 0) {			//没有内核镜像
		puts ("ERROR: can't get kernel image!\n");
		return 1;
	}

	/* 获取镜像参数 */
	switch (genimg_get_format (os_hdr)) {
	case IMAGE_FORMAT_LEGACY:		//这种格式

		puts("genimg_get_format >> IMAGE_FORMAT_LEGACY ... HOHO~\n");

		images.os.type = image_get_type (os_hdr);	//镜像类型
		images.os.comp = image_get_comp (os_hdr);	//压缩类型
		images.os.os = image_get_os (os_hdr);		//系统类型

		images.os.end = image_get_image_end (os_hdr);	//镜像结束地址
		images.os.load = image_get_load (os_hdr);	//加载地址
		break;

	default:
		puts ("ERROR: unknown image format type!\n");
		return 1;
	}

	/* 找到内核入口地址*/
	if (images.legacy_hdr_valid) {			//images.legacy_hdr_valid == 1
		images.ep = image_get_ep (&images.legacy_hdr_os_copy);
#if 0
#if defined(CONFIG_FIT)
	} else if (images.fit_uname_os) {
		ret = fit_image_get_entry (images.fit_hdr_os,
				images.fit_noffset_os, &images.ep);
		if (ret) {
			puts ("Can't get entry point property!\n");
			return 1;
		}
#endif
#endif
	} else {
		puts ("Could not find kernel entry point!\n");
		return 1;
	}

	if (((images.os.type == IH_TYPE_KERNEL) ||
	     (images.os.type == IH_TYPE_MULTI)) &&
	    (images.os.os == IH_OS_LINUX)) {
		/* 查询是否存在虚拟磁盘 */
		puts(" boot_get_ramdisk >>>>>>>>>>>>>HOHO~>>>>>>>>>>>>>>\n\n\n\n");
		ret = boot_get_ramdisk (argc, argv, &images, IH_INITRD_ARCH,
				&images.rd_start, &images.rd_end);
		if (ret) {
			puts ("Ramdisk image is corrupt or invalid\n");
			return 1;
		}

#if defined(CONFIG_OF_LIBFDT)
#if defined(CONFIG_PPC) || defined(CONFIG_M68K) || defined(CONFIG_SPARC)
		/* find flattened device tree */
		ret = boot_get_fdt (flag, argc, argv, &images,
				    &images.ft_addr, &images.ft_len);
		if (ret) {
			puts ("Could not find a valid device tree\n");
			return 1;
		}

		set_working_fdt_addr(images.ft_addr);
#endif
#endif
	}

	images.os.start = (ulong)os_hdr;	//赋值加载地址
	images.state = BOOTM_STATE_START;	//更新状态

	return 0;
}

#define BOOTM_ERR_RESET		-1
#define BOOTM_ERR_OVERLAP	-2
#define BOOTM_ERR_UNIMPLEMENTED	-3
static int bootm_load_os(image_info_t os, ulong *load_end, int boot_progress)
{
	uint8_t comp = os.comp;
	ulong load = os.load;
	ulong blob_start = os.start;
	ulong blob_end = os.end;
	ulong image_start = os.image_start;
	ulong image_len = os.image_len;
	uint unc_len = CONFIG_SYS_BOOTM_LEN;		//8M默认最大压缩gunzip

	const char *type_name = genimg_get_type_name (os.type);

	switch (comp) {
	case IH_COMP_NONE:		//直接加载不解压的格式
		if (load == blob_start) {
			printf ("   XIP hoho~%s ... ", type_name);
		} else {
			printf ("   Loading %s ... ", type_name);

			if (load != image_start) {
				//memmove((void *)load, (void *)image_start, image_len);
				memmove_wd ((void *)load,	//---> memmove
						(void *)image_start, image_len, CHUNKSZ);
			}
		}
		*load_end = load + image_len;
		puts("OK\n");
		break;



	default:
		printf ("Unimplemented compression type %d\n", comp);
		return BOOTM_ERR_UNIMPLEMENTED;
	}
	puts ("OK I'm Ready ~~ HOHO~\n\n\n\n");

	debug ("   kernel loaded at 0x%08lx, end = 0x%08lx\n", load, *load_end); //内核从内存的这里到那里

	if (boot_progress)
		show_boot_progress (7);

	if ((load < blob_end) && (*load_end > blob_start)) {
		debug ("************************HOHO~********************************\n");

		debug ("images.os.start = 0x%lX, images.os.end = 0x%lx\n", blob_start, blob_end);
		debug ("images.os.load = 0x%lx, load_end = 0x%lx\n", load, *load_end);

		debug ("************************HOHO~********************************\n");

		return BOOTM_ERR_OVERLAP;
	}

	return 0;
}

static int bootm_start_standalone(ulong iflag, int argc, char *argv[])
{
	char  *s;
	int   (*appl)(int, char *[]);

	/* Don't start if "autostart" is set to "no" */
	if (((s = getenv("autostart")) != NULL) && (strcmp(s, "no") == 0)) {
		char buf[32];
		sprintf(buf, "%lX", images.os.image_len);
		setenv("filesize", buf);
		return 0;
	}
	appl = (int (*)(int, char *[]))ntohl(images.ep);
	(*appl)(argc-1, &argv[1]);

	return 0;
}

/* we overload the cmd field with our state machine info instead of a
 * function pointer */
cmd_tbl_t cmd_bootm_sub[] = {
	U_BOOT_CMD_MKENT(start, 0, 1, (void *)BOOTM_STATE_START, "", ""),
	U_BOOT_CMD_MKENT(loados, 0, 1, (void *)BOOTM_STATE_LOADOS, "", ""),
#if defined(CONFIG_PPC) || defined(CONFIG_M68K) || defined(CONFIG_SPARC)
	U_BOOT_CMD_MKENT(ramdisk, 0, 1, (void *)BOOTM_STATE_RAMDISK, "", ""),
#endif
#ifdef CONFIG_OF_LIBFDT
	U_BOOT_CMD_MKENT(fdt, 0, 1, (void *)BOOTM_STATE_FDT, "", ""),
#endif
	U_BOOT_CMD_MKENT(cmdline, 0, 1, (void *)BOOTM_STATE_OS_CMDLINE, "", ""),
	U_BOOT_CMD_MKENT(bdt, 0, 1, (void *)BOOTM_STATE_OS_BD_T, "", ""),
	U_BOOT_CMD_MKENT(prep, 0, 1, (void *)BOOTM_STATE_OS_PREP, "", ""),
	U_BOOT_CMD_MKENT(go, 0, 1, (void *)BOOTM_STATE_OS_GO, "", ""),
};

int do_bootm_subcommand (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 0;
	int state;
	cmd_tbl_t *c;
	boot_os_fn *boot_fn;

	c = find_cmd_tbl(argv[1], &cmd_bootm_sub[0], ARRAY_SIZE(cmd_bootm_sub));

	if (c) {
		state = (int)c->cmd;

		/* treat start special since it resets the state machine */
		if (state == BOOTM_STATE_START) {
			argc--;
			argv++;
			return bootm_start(cmdtp, flag, argc, argv);
		}
	}
	/* Unrecognized command */
	else {
		cmd_usage(cmdtp);
		return 1;
	}

	if (images.state >= state) {
		printf ("Trying to execute a command out of order\n");
		cmd_usage(cmdtp);
		return 1;
	}

	images.state |= state;
	boot_fn = boot_os[images.os.os];

	switch (state) {
		ulong load_end;
		case BOOTM_STATE_START:
			/* should never occur */
			break;
		case BOOTM_STATE_LOADOS:
			ret = bootm_load_os(images.os, &load_end, 0);
			if (ret)
				return ret;

			lmb_reserve(&images.lmb, images.os.load,
					(load_end - images.os.load));
			break;


#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_SYS_BOOTMAPSZ)
		case BOOTM_STATE_FDT:
		{
			ulong bootmap_base = getenv_bootm_low();
			ret = boot_relocate_fdt(&images.lmb, bootmap_base,
				&images.ft_addr, &images.ft_len);
			break;
		}
#endif

		case BOOTM_STATE_OS_CMDLINE:
			ret = boot_fn(BOOTM_STATE_OS_CMDLINE, argc, argv, &images);
			if (ret)
				printf ("cmdline subcommand not supported\n");
			break;
		case BOOTM_STATE_OS_BD_T:
			ret = boot_fn(BOOTM_STATE_OS_BD_T, argc, argv, &images);
			if (ret)
				printf ("bdt subcommand not supported\n");
			break;
		case BOOTM_STATE_OS_PREP:
			ret = boot_fn(BOOTM_STATE_OS_PREP, argc, argv, &images);
			if (ret)
				printf ("prep subcommand not supported\n");
			break;
		case BOOTM_STATE_OS_GO:
			disable_interrupts();
			arch_preboot_os();
			boot_fn(BOOTM_STATE_OS_GO, argc, argv, &images);
			break;
	}

	return ret;
}

/*******************************************************************/
/* bootm - boot application image from image in memory */
/*******************************************************************/

int do_bootm (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong		iflag;
	ulong		load_end = 0;
	int		ret;
	boot_os_fn	*boot_fn;

	/* determine if we have a sub command */
	if (argc > 1) {					//（1）参数检查
		char *endp;

		simple_strtoul(argv[1], &endp, 16);
	
		if ((*endp != 0) && (*endp != ':') && (*endp != '#'))
			return do_bootm_subcommand(cmdtp, flag, argc, argv);
	}

	int res;
	res = bootm_start(cmdtp, flag, argc, argv);	//获取镜像信息
	if(res)
		return 1;	

	iflag = disable_interrupts();			//(2)关闭中断

	usb_stop();					//关闭usb设备

	ret = bootm_load_os(images.os, &load_end, 1);	//加载内核

	lmb_reserve(&images.lmb, images.os.load, (load_end - images.os.load));

	if (images.os.type == IH_TYPE_STANDALONE) {	//关闭内核的串口
		if (iflag)
			enable_interrupts();
		/* This may return when 'autostart' is 'no' */
		bootm_start_standalone(iflag, argc, argv);
		return 0;
	}

	show_boot_progress (8);

	boot_fn = boot_os[images.os.os];		//获取启动函数

	arch_preboot_os();				//启动前准备

	boot_fn(0, argc, argv, &images);		//启动，不再返回

	show_boot_progress (-9);
#ifdef DEBUG
	puts ("\n## Control returned to monitor - resetting...\n");
#endif
	do_reset (cmdtp, flag, argc, argv);

	return 1;
}

/**
 * image_get_kernel - verify legacy format kernel image
 * @img_addr: in RAM address of the legacy format image to be verified
 * @verify: data CRC verification flag
 *
 * image_get_kernel() verifies legacy image integrity and returns pointer to
 * legacy image header if image verification was completed successfully.
 *
 * returns:
 *     pointer to a legacy image header if valid image was found
 *     otherwise return NULL
 */
static image_header_t *image_get_kernel (ulong img_addr, int verify)
{
	image_header_t *hdr = (image_header_t *)img_addr;

	if (!image_check_magic(hdr)) {
		puts ("Bad Magic Number\n");
		show_boot_progress (-1);
		return NULL;
	}
	show_boot_progress (2);

	if (!image_check_hcrc (hdr)) {
		puts ("Bad Header Checksum\n");
		show_boot_progress (-2);
		return NULL;
	}

	show_boot_progress (3);
	image_print_contents (hdr);

	if (verify) {
		puts ("   Verifying Checksum ... ");
		if (!image_check_dcrc (hdr)) {
			printf ("Bad Data CRC\n");
			show_boot_progress (-3);
			return NULL;
		}
		puts ("OK\n");
	}
	show_boot_progress (4);

	if (!image_check_target_arch (hdr)) {
		printf ("Unsupported Architecture 0x%x\n", image_get_arch (hdr));
		show_boot_progress (-4);
		return NULL;
	}
	return hdr;
}

/**
 * fit_check_kernel - verify FIT format kernel subimage
 * @fit_hdr: pointer to the FIT image header
 * os_noffset: kernel subimage node offset within FIT image
 * @verify: data CRC verification flag
 *
 * fit_check_kernel() verifies integrity of the kernel subimage and from
 * specified FIT image.
 *
 * returns:
 *     1, on success
 *     0, on failure
 */
#if defined (CONFIG_FIT)
static int fit_check_kernel (const void *fit, int os_noffset, int verify)
{
	fit_image_print (fit, os_noffset, "   ");

	if (verify) {
		puts ("   Verifying Hash Integrity ... ");
		if (!fit_image_check_hashes (fit, os_noffset)) {
			puts ("Bad Data Hash\n");
			show_boot_progress (-104);
			return 0;
		}
		puts ("OK\n");
	}
	show_boot_progress (105);

	if (!fit_image_check_target_arch (fit, os_noffset)) {
		puts ("Unsupported Architecture\n");
		show_boot_progress (-105);
		return 0;
	}

	show_boot_progress (106);
	if (!fit_image_check_type (fit, os_noffset, IH_TYPE_KERNEL)) {
		puts ("Not a kernel image\n");
		show_boot_progress (-106);
		return 0;
	}

	show_boot_progress (107);
	return 1;
}
#endif /* CONFIG_FIT */

/**
 * boot_get_kernel - find kernel image
 * @os_data: pointer to a ulong variable, will hold os data start address
 * @os_len: pointer to a ulong variable, will hold os data length
 *
 * boot_get_kernel() tries to find a kernel image, verifies its integrity
 * and locates kernel data.
 *
 * returns:
 *     pointer to image header if valid image was found, plus kernel start
 *     address and length, otherwise NULL
 */
static void *boot_get_kernel (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
		bootm_headers_t *images, ulong *os_data, ulong *os_len)
{
	image_header_t	*hdr;
	ulong		img_addr;
#if defined(CONFIG_FIT)
	void		*fit_hdr;
	const char	*fit_uname_config = NULL;
	const char	*fit_uname_kernel = NULL;
	const void	*data;
	size_t		len;
	int		cfg_noffset;
	int		os_noffset;
#endif
	debug ("************************************\n");
	debug ("This is >> boot_get_kernel << HOHO~\n\n\n");
	debug ("************************************\n");

	/* find out kernel image address */
	if (argc < 2) {
		img_addr = load_addr;
		debug ("*  kernel: default image load address = 0x%08lx HOHO~\n\n\n\n",
				load_addr);
#if defined(CONFIG_FIT)
	} else if (fit_parse_conf (argv[1], load_addr, &img_addr,
							&fit_uname_config)) {
		debug ("**  kernel: config '%s' from image at 0x%08lx HOHO~\n\n\n\n",
				fit_uname_config, img_addr);
	} else if (fit_parse_subimage (argv[1], load_addr, &img_addr,
							&fit_uname_kernel)) {
		debug ("***  kernel: subimage '%s' from image at 0x%08lx HOHO~\n\n\n\n",
				fit_uname_kernel, img_addr);
#endif
	} else {
		img_addr = simple_strtoul(argv[1], NULL, 16);
		debug ("****  kernel: cmdline image address = 0x%08lx HOHO~\n\n\n\n", img_addr);
	}

	show_boot_progress (1);

	/* copy from dataflash if needed */
	img_addr = genimg_get_image (img_addr);		//直接return img_add自己

	/* check image type, for FIT images get FIT kernel node */
	*os_data = *os_len = 0;
	switch (genimg_get_format ((void *)img_addr)) {
	case IMAGE_FORMAT_LEGACY:
		printf ("## Booting kernel from Legacy Image at %08lx ...HOHO ~ \n\n\n\n",
				img_addr);
		hdr = image_get_kernel (img_addr, images->verify);
		if (!hdr)
			return NULL;
		show_boot_progress (5);

		/* get os_data and os_len */
		switch (image_get_type (hdr)) {
		case IH_TYPE_KERNEL:
			debug (" case IH_TYPE_KERNEL \n");
			*os_data = image_get_data (hdr);
			*os_len = image_get_data_size (hdr);
			break;
		case IH_TYPE_MULTI:
			debug (" case IH_TYPE_MULTI \n");
			image_multi_getimg (hdr, 0, os_data, os_len);
			break;
		case IH_TYPE_STANDALONE:
			debug (" case IH_TYPE_STANDALONE \n");
			*os_data = image_get_data (hdr);
			*os_len = image_get_data_size (hdr);
			break;
		default:
			printf ("Wrong Image Type for %s command\n", cmdtp->name);
			show_boot_progress (-5);
			return NULL;
		}

		/*
		 * copy image header to allow for image overwrites during kernel
		 * decompression.
		 */
		memmove (&images->legacy_hdr_os_copy, hdr, sizeof(image_header_t));

		/* save pointer to image header */
		images->legacy_hdr_os = hdr;

		images->legacy_hdr_valid = 1;
		show_boot_progress (6);
		break;

#if 0			//用不到的
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		fit_hdr = (void *)img_addr;
		printf ("## Booting kernel from FIT Image at %08lx ...\n",
				img_addr);

		if (!fit_check_format (fit_hdr)) {
			puts ("Bad FIT kernel image format!\n");
			show_boot_progress (-100);
			return NULL;
		}
		show_boot_progress (100);

		if (!fit_uname_kernel) {
			/*
			 * no kernel image node unit name, try to get config
			 * node first. If config unit node name is NULL
			 * fit_conf_get_node() will try to find default config node
			 */
			show_boot_progress (101);
			cfg_noffset = fit_conf_get_node (fit_hdr, fit_uname_config);
			if (cfg_noffset < 0) {
				show_boot_progress (-101);
				return NULL;
			}
			/* save configuration uname provided in the first
			 * bootm argument
			 */
			images->fit_uname_cfg = fdt_get_name (fit_hdr, cfg_noffset, NULL);
			printf ("   Using '%s' configuration\n", images->fit_uname_cfg);
			show_boot_progress (103);

			os_noffset = fit_conf_get_kernel_node (fit_hdr, cfg_noffset);
			fit_uname_kernel = fit_get_name (fit_hdr, os_noffset, NULL);
		} else {
			/* get kernel component image node offset */
			show_boot_progress (102);
			os_noffset = fit_image_get_node (fit_hdr, fit_uname_kernel);
		}
		if (os_noffset < 0) {
			show_boot_progress (-103);
			return NULL;
		}

		printf ("   Trying '%s' kernel subimage\n", fit_uname_kernel);

		show_boot_progress (104);
		if (!fit_check_kernel (fit_hdr, os_noffset, images->verify))
			return NULL;

		/* get kernel image data address and length */
		if (fit_image_get_data (fit_hdr, os_noffset, &data, &len)) {
			puts ("Could not find kernel subimage data!\n");
			show_boot_progress (-107);
			return NULL;
		}
		show_boot_progress (108);

		*os_len = len;
		*os_data = (ulong)data;
		images->fit_hdr_os = fit_hdr;
		images->fit_uname_os = fit_uname_kernel;
		images->fit_noffset_os = os_noffset;
		break;
#endif
#endif
	default:
		printf ("Wrong Image Format for %s command\n", cmdtp->name);
		show_boot_progress (-108);
		return NULL;
	}

	debug ("   kernel data at 0x%08lx, len = 0x%08lx (%ld) HOHO ~ \n\n\n\n",
			*os_data, *os_len, *os_len);

	return (void *)img_addr;
}

U_BOOT_CMD(
	bootm,	CONFIG_SYS_MAXARGS,	1,	do_bootm,
	"boot application image from memory",
	"[addr [arg ...]]\n    - boot application image stored in memory\n"
	"\tpassing arguments 'arg ...'; when booting a Linux kernel,\n"
	"\t'arg' can be the address of an initrd image\n"
#if defined(CONFIG_OF_LIBFDT)
	"\tWhen booting a Linux kernel which requires a flat device-tree\n"
	"\ta third argument is required which is the address of the\n"
	"\tdevice-tree blob. To boot that kernel without an initrd image,\n"
	"\tuse a '-' for the second argument. If you do not pass a third\n"
	"\ta bd_info struct will be passed instead\n"
#endif
#if defined(CONFIG_FIT)
	"\t\nFor the new multi component uImage format (FIT) addresses\n"
	"\tmust be extened to include component or configuration unit name:\n"
	"\taddr:<subimg_uname> - direct component image specification\n"
	"\taddr#<conf_uname>   - configuration specification\n"
	"\tUse iminfo command to get the list of existing component\n"
	"\timages and configurations.\n"
#endif
	"\nSub-commands to do part of the bootm sequence.  The sub-commands "
	"must be\n"
	"issued in the order below (it's ok to not issue all sub-commands):\n"
	"\tstart [addr [arg ...]]\n"
	"\tloados  - load OS image\n"
#if defined(CONFIG_PPC) || defined(CONFIG_M68K) || defined(CONFIG_SPARC)
	"\tramdisk - relocate initrd, set env initrd_start/initrd_end\n"
#endif
#if defined(CONFIG_OF_LIBFDT)
	"\tfdt     - relocate flat device tree\n"
#endif
	"\tcmdline - OS specific command line processing/setup\n"
	"\tbdt     - OS specific bd_t processing\n"
	"\tprep    - OS specific prep before relocation or go\n"
	"\tgo      - start OS"
);

/*******************************************************************/
/* bootd - boot default image */
/*******************************************************************/
#if defined(CONFIG_CMD_BOOTD)
int do_bootd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int rcode = 0;

#ifndef CONFIG_SYS_HUSH_PARSER
	if (run_command (getenv ("bootcmd"), flag) < 0)
		rcode = 1;
#else
	if (parse_string_outer (getenv ("bootcmd"),
			FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) != 0)
		rcode = 1;
#endif
	return rcode;
}

U_BOOT_CMD(
	boot,	1,	1,	do_bootd,
	"boot default, i.e., run 'bootcmd'",
	""
);

/* keep old command name "bootd" for backward compatibility */
U_BOOT_CMD(
	bootd, 1,	1,	do_bootd,
	"boot default, i.e., run 'bootcmd'",
	""
);

#endif


/*******************************************************************/
/* iminfo - print header info for a requested image */
/*******************************************************************/
#if defined(CONFIG_CMD_IMI)
int do_iminfo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int	arg;
	ulong	addr;
	int	rcode = 0;

	if (argc < 2) {
		return image_info (load_addr);
	}

	for (arg = 1; arg < argc; ++arg) {
		addr = simple_strtoul (argv[arg], NULL, 16);
		if (image_info (addr) != 0)
			rcode = 1;
	}
	return rcode;
}

static int image_info (ulong addr)
{
	void *hdr = (void *)addr;

	printf ("\n## Checking Image at %08lx ...\n", addr);

	switch (genimg_get_format (hdr)) {
	case IMAGE_FORMAT_LEGACY:
		puts ("   Legacy image found\n");
		if (!image_check_magic (hdr)) {
			puts ("   Bad Magic Number\n");
			return 1;
		}

		if (!image_check_hcrc (hdr)) {
			puts ("   Bad Header Checksum\n");
			return 1;
		}

		image_print_contents (hdr);

		puts ("   Verifying Checksum ... ");
		if (!image_check_dcrc (hdr)) {
			puts ("   Bad Data CRC\n");
			return 1;
		}
		puts ("OK\n");
		return 0;
#if defined(CONFIG_FIT)
	case IMAGE_FORMAT_FIT:
		puts ("   FIT image found\n");

		if (!fit_check_format (hdr)) {
			puts ("Bad FIT image format!\n");
			return 1;
		}

		fit_print_contents (hdr);

		if (!fit_all_image_check_hashes (hdr)) {
			puts ("Bad hash in FIT image!\n");
			return 1;
		}

		return 0;
#endif
	default:
		puts ("Unknown image format!\n");
		break;
	}

	return 1;
}

U_BOOT_CMD(
	iminfo,	CONFIG_SYS_MAXARGS,	1,	do_iminfo,
	"print header information for application image",
	"addr [addr ...]\n"
	"    - print header information for application image starting at\n"
	"      address 'addr' in memory; this includes verification of the\n"
	"      image contents (magic number, header and payload checksums)"
);
#endif


/*******************************************************************/
/* imls - list all images found in flash */
/*******************************************************************/
#if defined(CONFIG_CMD_IMLS)
int do_imls (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	flash_info_t *info;
	int i, j;
	void *hdr;

	for (i = 0, info = &flash_info[0];
		i < CONFIG_SYS_MAX_FLASH_BANKS; ++i, ++info) {

		if (info->flash_id == FLASH_UNKNOWN)
			goto next_bank;
		for (j = 0; j < info->sector_count; ++j) {

			hdr = (void *)info->start[j];
			if (!hdr)
				goto next_sector;

			switch (genimg_get_format (hdr)) {
			case IMAGE_FORMAT_LEGACY:
				if (!image_check_hcrc (hdr))
					goto next_sector;

				printf ("Legacy Image at %08lX:\n", (ulong)hdr);
				image_print_contents (hdr);

				puts ("   Verifying Checksum ... ");
				if (!image_check_dcrc (hdr)) {
					puts ("Bad Data CRC\n");
				} else {
					puts ("OK\n");
				}
				break;
#if defined(CONFIG_FIT)
			case IMAGE_FORMAT_FIT:
				if (!fit_check_format (hdr))
					goto next_sector;

				printf ("FIT Image at %08lX:\n", (ulong)hdr);
				fit_print_contents (hdr);
				break;
#endif
			default:
				goto next_sector;
			}

next_sector:		;
		}
next_bank:	;
	}

	return (0);
}

U_BOOT_CMD(
	imls,	1,		1,	do_imls,
	"list all images found in flash",
	"\n"
	"    - Prints information about all images found at sector\n"
	"      boundaries in flash."
);
#endif

