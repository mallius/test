/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * (C) Copyright 2005
 * JinHua Luo, GuangDong Linux Center, <luo.jinhua@gd-linux.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
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
#include <netdev.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <video_fb.h>

#if defined(CONFIG_CMD_NAND)
#include <linux/mtd/nand.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define FCLK_SPEED 1

//#if FCLK_SPEED==0		/* Fout = 203MHz, Fin = 12MHz for Audio */
//#define M_MDIV	0xC3
//#define M_PDIV	0x4
//#define M_SDIV	0x1


//#elif FCLK_SPEED==1		/* Fout = 202.8MHz */

//#if defined(CONFIG_S3C2410)
/* Fout = 202.8MHz */
//#define M_MDIV	0xA1    hoho ~
//#define M_PDIV	0x3
//#define M_SDIV	0x1
//#endif

#if defined(CONFIG_S3C2440)
/* Fout = 405MHz */
//输入12M, 输出405M
#define M_MDIV 0x7f	// 127
#define M_PDIV 0x2	// 2
#define M_SDIV 0x1	// 1
#endif

//#endif

#define USB_CLOCK 1

//#if USB_CLOCK==0		hoho~
//#define U_M_MDIV	0xA1
//#define U_M_PDIV	0x3
//#define U_M_SDIV	0x1
//#elif USB_CLOCK==1

//#if defined(CONFIG_S3C2410)
//#define U_M_MDIV	0x48
//#define U_M_PDIV	0x3
//#endif

#if defined(CONFIG_S3C2440)
#define U_M_MDIV 0x38
#define U_M_PDIV 0x2
#endif

#define U_M_SDIV	0x2
//#endif

static inline void delay (unsigned long loops)
{
	__asm__ volatile ("1:\n"
			  "subs %0, %1, #1\n"
			  "bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_init (void)
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	clk_power->LOCKTIME = 0xFFFFFF;	//oxffff_ffff是复位值

	/* configure MPLL */		//见PLL值选择表，Fin = 12M, Fout = 405M
	clk_power->MPLLCON = ((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV);
				//M:127, P:2, S:1
	/* some delay between MPLL and UPLL */
	delay (4000);

	/* configure UPLL */
	clk_power->UPLLCON = ((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV);

	/* some delay between MPLL and UPLL */
	delay (8000);

	/* set up the I/O ports */
	gpio->GPACON = 0x007FFFFF;

#if defined(CONFIG_SMDK2440) 
	gpio->GPBCON = 0x00295551;
//#else
//	gpio->GPBCON = 0x00044556;
#endif

	gpio->GPBUP = 0x000007FF;

#if defined(CONFIG_SMDK2440) 
	gpio->GPCCON = 0xAAAAA6AA;
	gpio->GPCDAT &= ~(1<<5);
#else
	gpio->GPCCON = 0xAAAAAAAA;
#endif
	gpio->GPCUP = 0xFFFFFFFF;		// pull up function is disabled
	gpio->GPDCON = 0xAAAAAAAA;
	gpio->GPDUP = 0xFFFFFFFF;

    	gpio->GPECON = 0xAAAAAAAA;
	gpio->GPEUP = 0x0000FFFF;
	gpio->GPFCON = 0x000055AA;
	gpio->GPFUP = 0x000000FF;
	gpio->GPGCON = 0xFF95FF3A;
	gpio->GPGUP = 0x0000FFFF;
	gpio->GPHCON = 0x0016FAAA;
	gpio->GPHUP = 0x000007FF;

	gpio->EXTINT0=0x22222222;
	gpio->EXTINT1=0x22222222;
	gpio->EXTINT2=0x22222222;

#if defined(CONFIG_S3C2410)
	/* arch number of SMDK2410-Board */
	gd->bd->bi_arch_number = MACH_TYPE_SMDK2410;
#endif

#if defined(CONFIG_S3C2440)
/* arch number of S3C2440-Board */
	gd->bd->bi_arch_number = MACH_TYPE_SMDK2440A;
	//gd->bd->bi_arch_number = 782;
#endif

	//printf("Start Linux parameters at 0x30000100\n");
	//printf("MACH_TYPE=%d\n", gd->bd->bi_arch_number);
	//printf("Linux command line is: %s\n",CONFIG_BOOTARGS);

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}


#define MVAL_USED 	(0)		//0=each frame   1=rate by MVAL
#define INVVDEN		(1)		//0=normal       1=inverted
#define BSWP		(0)		//Byte swap control
#define HWSWP		(1)		//Half word swap control
#define LCDCON5_PWREN (1<<3)   //LCD_PWREN output signal enable

//TFT 240320
#define LCD_XSIZE_TFT_240320 	(320)	
#define LCD_YSIZE_TFT_240320 	(240)

//TFT240320
#define HOZVAL_TFT_240320	(LCD_XSIZE_TFT_240320-1)
#define LINEVAL_TFT_240320	(LCD_YSIZE_TFT_240320-1)

//Timing parameter for NEC 3.5"
#if defined(CONFIG_mini2440_N35)
#define MVAL_N35				(0)
#define LCD_PIXCLOCK_N35            (4)
#define VBPD_240320_N35		(3)		
#define VFPD_240320_N35		(10)
#define VSPW_240320_N35		(1)

#define HBPD_240320_N35		(5)
#define HFPD_240320_N35		(2)
#define HSPW_240320_N35		(36)

#define HSPW_240320_NEC		(36)  //Adjust the horizontal displacement of the screen :tekkamanninja@163.com
				      //+ ： -->    - : <--
#define CLKVAL_TFT_240320_N35	(3) 	
#define LCD_CON5_N35 ( (1 << 11)| (1<<0) | (1 << 8) | (1 << 6) | (1 << 9) | ( 1<< 10))
//FCLK=101.25MHz,HCLK=50.625MHz,VCLK=6.33MHz
#endif

//Timing parameter for Toploy 3.5"
#if defined(CONFIG_mini2440_T35)
#define MVAL_T35				(0)
#define LCD_PIXCLOCK_T35 		(4)
#define VBPD_240320_T35		(3)		
#define VFPD_240320_T35		(10)
#define VSPW_240320_T35		(1)

#define HBPD_240320_T35		(5)
#define HFPD_240320_T35		(2)
#define HSPW_240320_T35		(23)

#define HSPW_240320_TD		(23)  //64MB nand mini2440 is 36 ,128MB is 23
#define LCD_CON5_T35 ( (1 << 11)| (1<<0) | (1 << 8) | (1 << 6) | (1 << 9) | ( 1<< 10))

#define CLKVAL_TFT_240320_T35	(3) 	
//FCLK=101.25MHz,HCLK=50.625MHz,VCLK=6.33MHz
#endif

//Timing parameter for sony 3.5"
#if defined(CONFIG_mini2440_X35)

#define MVAL_X35		(13)
#define LCD_PIXCLOCK_X35 	(4)
#define VBPD_240320_X35	(0)		
#define VFPD_240320_X35	(4)
#define VSPW_240320_X35		(9)

#define HBPD_240320_X35		(25)
#define HFPD_240320_X35		(0)
#define HSPW_240320_X35	(4)
#define LCD_CON5_X35 ( (1 << 11)| (1<<0) | (1 << 8) | (1 << 6) | (1 << 9) | ( 1<< 10))

#define CLKVAL_TFT_240320	(3) 	
#endif

//Timing parameter for W35 320*240"
#if defined(CONFIG_mini2440_W35)

#define MVAL_W35		(13)
#define LCD_PIXCLOCK_W35 	(4)
#define VBPD_240320_W35	(10)		
#define VFPD_240320_W35	(4)
#define VSPW_240320_W35		(1)

#define HBPD_240320_W35		(1)
#define HFPD_240320_W35		(0x1)
#define HSPW_240320_W35	(10)
#define LCD_CON5_W35 ((1<<11) | (1<<8) | (1<<9) | (1<<4)| (1<<0) )

#define CLKVAL_TFT_240320	(3) 	
#endif


void board_video_init(GraphicDevice *pGD) 
{ 
	struct s3c24x0_lcd * const lcd	 = s3c24x0_get_base_lcd(); 
	//struct s3c2410_nand * const nand = s3c2410_get_base_nand();
	
	char *s;
	char lcdtype ;
	s = getenv ("lcdtype");
	//lcdtype = *s;
	lcdtype = 'w';
	//lcdtype = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_LCDTYPE;
	printf("In board_video_init, lcdtype=%c\n",lcdtype);
	if (lcdtype == CONFIG_mini2440_X35) {
		lcd->LCDCON1 = (LCD_PIXCLOCK_X35 << 8) | (3 <<  5) | (12 << 1);
		lcd->LCDCON2 = (VBPD_240320_X35<<24)|(LINEVAL_TFT_240320<<14)|(VFPD_240320_X35<<6)|(VSPW_240320_X35); 
		lcd->LCDCON3 = (HBPD_240320_X35<<19)|(HOZVAL_TFT_240320<<8)|(HFPD_240320_X35); 

			lcd->LCDCON4 = (MVAL_X35<<8)|(HSPW_240320_X35); //Hanson add
		
		//lcd->LCDCON5 = 0x00000f09; //Hanson change
		lcd->LCDCON5 = LCD_CON5_X35 | LCDCON5_PWREN; //LCD_PWREN output signal enable
	}
	else if (lcdtype == CONFIG_mini2440_T35) {
		lcd->LCDCON1 = (LCD_PIXCLOCK_T35 << 8) | (3 <<  5) | (12 << 1);
		lcd->LCDCON2 = (VBPD_240320_T35<<24)|(LINEVAL_TFT_240320<<14)|(VFPD_240320_T35<<6)|(VSPW_240320_T35); 
		lcd->LCDCON3 = (HBPD_240320_T35<<19)|(HOZVAL_TFT_240320<<8)|(HFPD_240320_T35); 

		/*
			if ( (nand->NFCONF) & 0x08 )	{ 
			lcd->LCDCON4 = (MVAL<<8)|(HSPW_240320_TD);
			}
			else	{
			  lcd->LCDCON4 = (MVAL<<8)|(HSPW_240320_NEC);
			}
		*/
		lcd->LCDCON4 = (MVAL_T35<<8)|(HSPW_240320_T35); //Hanson add
		lcd->LCDCON5 = LCD_CON5_T35 | LCDCON5_PWREN; //LCD_PWREN output signal enable
	}
	else if (lcdtype == CONFIG_mini2440_N35) {

		lcd->LCDCON1 = (LCD_PIXCLOCK_N35 << 8) | (3 <<  5) | (12 << 1);
		lcd->LCDCON2 = (VBPD_240320_N35<<24)|(LINEVAL_TFT_240320<<14)|(VFPD_240320_N35<<6)|(VSPW_240320_N35); 
		lcd->LCDCON3 = (HBPD_240320_N35<<19)|(HOZVAL_TFT_240320<<8)|(HFPD_240320_N35); 

		lcd->LCDCON4 = (MVAL_N35<<8)|(HSPW_240320_N35); //Hanson add
		
		lcd->LCDCON5 = LCD_CON5_N35 | LCDCON5_PWREN; //LCD_PWREN output signal enable
	}
	else if (lcdtype == CONFIG_mini2440_W35) {

		lcd->LCDCON1 = (LCD_PIXCLOCK_W35 << 8) | (3 <<  5) | (12 << 1);
		lcd->LCDCON2 = (VBPD_240320_W35<<24)|(LINEVAL_TFT_240320<<14)|(VFPD_240320_W35<<6)|(VSPW_240320_W35); 
		lcd->LCDCON3 = (HBPD_240320_W35<<19)|(HOZVAL_TFT_240320<<8)|(HFPD_240320_W35); 

		lcd->LCDCON4 = (MVAL_W35<<8)|(HSPW_240320_W35); //Hanson add
		
		lcd->LCDCON5 = LCD_CON5_W35 | LCDCON5_PWREN; //LCD_PWREN output signal enable
	}
	//lcd->LPCSEL  = 0x00000000; 
	lcd->LCDINTMSK |= 3; 
	lcd->LPCSEL  &= (~7); 
	lcd->TPAL     = 0x0;
	lcd->LCDCON1 |= 0x01; //Enable LCD display by Hanson
	
} 


int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}


#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_CS8900
	rc = cs8900_initialize(0, CONFIG_CS8900_BASE);
#endif
#ifdef CONFIG_DRIVER_DM9000
	rc = dm9000_initialize(bis);
#endif
	return rc;
}
#endif
