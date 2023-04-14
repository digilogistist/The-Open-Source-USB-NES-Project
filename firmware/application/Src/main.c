// USB-NES Lite 0.79 FIRM-proof (TM) application
// Copyright 2019 Big Time Technology Development Group.

/* Includes */
#include <string.h>
#include "stm32f1xx_hal.h"

#define						DIRTY_TIMEOUT				1000
#define						MSEC_const					(72000000 / 1000)
#define						SAVE_SIZE					1024
#define						BCMTABLE_SIZE				1024
#define						OVERRIDE_SIZE				4096
#define						RAM_BASE					0x20002800U
#define						RAM_USED					4608
#define						BOOT_BLOCK_BASE				0x08000000U
#define						APPLICATION_BASE			0x08008000U
#define						SETTINGS_BASE				0x0800E800U
#define						BCMTABLE_BASE				0x0800EC00U
#define						OVERRIDE_BASE				0x0800F000U
#define						DEMO_BASE					0x08010000U
#define						BOOT_BLOCK					((t_boot_block *) BOOT_BLOCK_BASE )
#define						APPLICATION					((t_app_header *) APPLICATION_BASE)
#define						DEMO_NES					((t_demo	   *) DEMO_BASE		  )
#define						SETTINGS_BIN				((t_settings   *) SETTINGS_BASE	  )
#define						OVERRIDE_BIN				((t_override   *) OVERRIDE_BASE	  )
#define						BCMTABLE_BIN				((t_bcmtable   *) BCMTABLE_BASE	  )
#define						simple_types				BCMTABLE_BIN
#define						SETTINGS_ITEMS				(sizeof (t_settings_int) / sizeof (short))

typedef	union {

	unsigned char			byte						[2][16];
	short					half						[16];

} t_bcmtable;

typedef union {

	char					byte						[SAVE_SIZE  ];
	uint16_t				half						[SAVE_SIZE/2];

} t_settings;

typedef union {

	unsigned char *			byte;
	unsigned short *		half;
	unsigned int *			word;

} ptr_all_types;

typedef struct {

	uint8_t					company_name				[ 8];
	uint8_t					product_name				[16];
	uint8_t					version						[ 4];

} t_product;

typedef struct {

	uint8_t					filename					[11];
	uint8_t					attributes;
	uint8_t					lowercase_letters;			//	bit 3 for lowercase filename; bit 4 for lowercase extension
	uint8_t					creation_time_cs;
	uint16_t				creation_time;
	uint16_t				creation_date;
	uint16_t				last_access_date;
	uint16_t				starting_cluster_hi;
	uint16_t				modification_time;
	uint16_t				modification_date;
	uint16_t				starting_cluster;
	uint32_t				file_size;

} t_dir_slot;

typedef struct {

	uint8_t					stuff						[0x09AC];
	void					(* unlock_fpec				) (void);
	uint32_t				(* decrypt					) (uint32_t * buf, uint32_t * flash_addr);
	void					(* page_erase_handler		) (uint32_t reg_base, t_settings * dest);
	void					(* flash_halfword_handler	) (uint32_t reg_base, t_settings * dest, uint32_t data);
	void					(* flash_kernel_handler		) (uint32_t retry_count);
	t_dir_slot				firmware_dir_entry;
	t_dir_slot				security_dir_entry;
	uint8_t					boot_sector					[512];
	t_product				product_ID;
	uint8_t					hardware_ID					[ 12];
	uint8_t					authentication_block		[728];
	uint32_t				encryption_key				[ 64];

} t_boot_block;

typedef struct {

	uint32_t				file_size;					//	in 32-bit words
	uint32_t				CRC32;
	t_product				product;

} t_app_header;

typedef void				(* ptr_io_handler			) (ptr_all_types buf, uint32_t file_ofs);	// file offset in sectors

typedef struct {

	t_dir_slot				dir_entry;
	ptr_io_handler			read_handler;
	ptr_io_handler			write_handler;

} t_file;

typedef t_file				(* ptr_file					);
typedef	t_dir_slot			(* ptr_dir_slot				);
typedef uint8_t				t_demo						[65536];

typedef struct {

	ptr_dir_slot			(* create_file_handler		) (ptr_file	ptr);
	ptr_dir_slot			(* make_file_handler		) (char * filename, uint32_t filesize, ptr_io_handler rd_hndlr, ptr_io_handler wr_hndlr);
	void					(* delete_file_handler		) (ptr_dir_slot	ptr);
	void					(* media_change_handler		) (void);

} t_kernel_functions;

extern void					init						(t_kernel_functions * ptr);
extern void					systick						(void);

//---------------------------------------------------------------------------------------------------------
//timing stuff for cart io access

typedef struct {

	unsigned short			wait;
	unsigned short			max_wait;
	char *					name;

} t_wait_item;

typedef struct {

	unsigned short			unit_power_warmup;
	unsigned short			idle_powerdown_ms;
	unsigned short			battery_power_down;
	unsigned short			quick_discharge_enable;
	unsigned short			battery_test_enable;
	unsigned short			bus_disable;
//	unsigned short			MMC3_emulation_on;

	unsigned short			ASIC_map_testing_on;
	unsigned short			alt_bus_con_test_on;
	unsigned short			conflict_testing_on;
	unsigned short			_16KB_PRG_ROM_test;
	unsigned short			override_testing_on;
	unsigned short			override_mode_index;
	unsigned short			NES_2_0_enable;

	unsigned short			README_TXT_enable;
	unsigned short			REPORT_TXT_enable;
	unsigned short			RESETNES_TXT_on;
	unsigned short			OVERRIDE_TXT_on;
	unsigned short			OVERRIDE_BIN_on;
	unsigned short			BCMTABLE_BIN_on;
	unsigned short			debug_enable;
	unsigned short			DEMO_NES_enable;

} t_settings_int;

const t_wait_item			ch_init						[SETTINGS_ITEMS] = {

	{	100,	 1000,		"powerup_warmup_ms"			},
	{	500,	 1000,		"idle_powerdown_ms"			},
	{  1000,	10000,		"batt_powerdown_ms"			},
	{	  0,		1,		"PD_discharge_enable"		},
	{	  1,		1,		"battery_test_enable"		},
	{	  0,		1,		"ROM.W_bus_disable"			},
//	{	  1,		1,		"map_268_MMC3_mode"			},

	{	  1,		1,		"ASIC_map_testing_on"		},
	{	  1,		1,		"alt_buscon_tests_on"		},
	{	  1,		1,		"conflict_testing_on"		},
	{	  1,		1,		"16_KB_PRG-ROM_test"		},
	{	  1,		1,		"override_testing_on"		},
	{	  0,	  128,		"default_map_index"			},
	{	  0,	  	9,		"NES_2.0___enable"			},

	{	  1,		1,		"README.TXT_enable"			},
	{	  0,		1,		"REPORT.TXT_enable"			},
	{	  1,		1,		"DETECT.TXT_enable"			},
	{	  1,		1,		"OVERRIDE.TXT_enable"		},
	{	  0,		1,		"OVERRIDE.BIN_enable"		},
	{	  1,		1,		"BCMTABLE.BIN_enable"		},
	{	  0,		1,		"BCMTABLE_reporting"		},
	{	  0,		1,		"DEMO.NES__enable"			},

};

const char					readme_txt					[] = {
							"Visit usbnes.com for the latest info, updates, user's guide, legal and more."
//							"\r\n\r\nUSB-NES (TM)"
//							"\r\nCopyright (C) 2019 Big Time Technology Development Group."
//							"\r\nAll rights reserved."
};

union {

	unsigned short			item						[SETTINGS_ITEMS];
	t_settings_int			items;

} ch;

unsigned char *				fpos;
t_kernel_functions *		ptr_krnl;

typedef struct {

	// iNES header
	unsigned int		iNES_signature;
	unsigned char		PRG_ROM_size;
	unsigned char		CHR_ROM_size;
	unsigned char		flags_6;
	unsigned char		flags_7;
	unsigned char		submapper;
	unsigned char		PRG_CHR_ROM_size_MSB;
	unsigned char		PRG_RAM_size;
	unsigned char		CHR_RAM_size;
	unsigned char		stuff [4];

	// override header
	unsigned short		PRG_bank_ofs;
	unsigned short		CHR_bank_ofs;
	unsigned short		PRG_bank_ofs_SRAM;
	unsigned short		CHR_bank_ofs_SRAM;
	unsigned char		PRG_bank_bits;					//	size of bank in bit count
	unsigned char		CHR_bank_bits;
	unsigned char		PRG_bank_bits_SRAM;
	unsigned char		CHR_bank_bits_SRAM;
	unsigned char		SEQ_present;					//	bits 0, 1: INIT & DOWN sequence following any PRG, CHR, SAV section
	unsigned char		mode;							//	bit 7: MMC1 mode (serialize 5 bits, ctrl_reg = 01100b)
	unsigned short		CRLF;

} t_h_override;

typedef struct {

	unsigned char		address_map			[16];
	unsigned char		data_dir;
	unsigned char		data_map			[ 8];
	unsigned char		SP2;
	unsigned char		ID_string			[ 4];
	unsigned short		CRLF;

} t_s_override;

typedef struct {

	unsigned char		text				[16];
	unsigned char		reserved			[12];
	unsigned int		signature;

} t_n_override;

typedef union {

	t_h_override		header				[OVERRIDE_SIZE / 32];
	t_s_override		sequence			[OVERRIDE_SIZE / 32];
	t_n_override		name				[OVERRIDE_SIZE / 32];
	t_bcmtable			data				[OVERRIDE_SIZE / 32];
	unsigned short		half				[OVERRIDE_SIZE /  2];
	unsigned char		byte				[OVERRIDE_SIZE	   ];

} t_override;

union {

	unsigned char		buf					[1024];
	unsigned short		Buf					[512];

} debug;

//---------------------------------------------------------------------------------------------------------
// these functions maintain the settings.txt file and the timing parameters

int next_delimiter () {

	// loop to find delimiters
	int i = SAVE_SIZE;
	do {
		fpos++;
		switch (fpos [0]) {

		case   0:
		case 255:	return -1;
		case   9:
		case  10:
		case  13:
		case  32:	return  0;

		}
	} while (--i);
	return -1;
}

int next_string () {

	// loop while in delimiters
	int i = SAVE_SIZE;
	do {
		fpos++;
		switch (fpos [0]) {

		default :	return  0;
		case   0:
		case 255:	return -1;
		case   9:
		case  10:
		case  13:
		case  32:;

		}
	} while (--i);
	return -1;
}

int next_number (uint32_t * num) {

	// find size of number string
	unsigned char * endpos = fpos;
	next_delimiter ();
	if ((int) fpos - (int) endpos > 10) return -1;

	// extract the digits
	unsigned char * curpos = fpos;
	unsigned int i = 1;
	num [0] = 0;
	do {

		curpos--;
		char tmp = curpos [0];
		switch (tmp) {

			default				: return -1;
			case '0' ... '9'	: num [0] += (tmp - '0') * i;

		}
		i *= 10;

	} while (curpos != endpos);
	return 0;
}

// will import all existing settings from file
uint8_t settings_items;
void import_settings (register t_settings * buf, int init_) {

	// find the new settings
	int ctr = 0;
	buf -> byte [SAVE_SIZE - 1] = 0;					//	inject a null char in there to terminate strstr ()
	for (register uint32_t j = 0; j < settings_items; j++) {

		// find the string
		fpos = (unsigned char *) strstr ((char *) buf, ch_init [j].name);
		if (!fpos)										goto FAIL;

		// advance past the name string
		if (next_delimiter ())							goto FAIL;
		if (next_string	   ())							goto FAIL;

		// check next param for '='
		unsigned char * oldpos = fpos;
		if (next_delimiter ())							goto FAIL;
		if ( 1  != (int) fpos - (int) oldpos	)		goto FAIL;
		if ('=' !=					  oldpos [0])		goto FAIL;

		// advance to number
		if (next_string	())								goto FAIL;

		// get the number
		uint32_t tmp_u;
		if (next_number (&tmp_u))						goto FAIL;

		// test against maximum parameter
		if (tmp_u > ch_init [j].max_wait)				goto FAIL;

		// assign the setting
		ch.item [j] = tmp_u;
		ctr++;
		continue;

		// load the default setting if no new one found
FAIL:	ch.item [j] = ch_init [j].wait;
	}

	// import parameters from new data, check to see if all parameters are present
	if (ctr != settings_items) {

		// rewrite settings file if not all parameters there
		register int pos = 0;
		for (register uint32_t j = 0; j < settings_items; j++)

			// write each setting parameter out to the buffer line by line
			pos += sprintf (& buf -> byte [pos], "%s\t=\t%u\r\n", ch_init [j].name, ch.item [j]);

	// don't update flash if running from init
	} else if (init_) return;

	// unlock FPEC
	BOOT_BLOCK -> unlock_fpec ();

  	// erase existing block
	BOOT_BLOCK -> page_erase_handler (0x40022000, SETTINGS_BIN);	// erase last 1KB of 64KB segment

	// copy new data into flash
	for (register uint32_t j = 0; j < (SAVE_SIZE / 2); j++)

		BOOT_BLOCK -> flash_halfword_handler (0x40022000, (t_settings *) & SETTINGS_BIN -> half [j], buf -> half [j]);

}

//---------------------------------------------------------------------------------------------------------
/*
const t_bcmtable simple_types [] = {
//const unsigned char simple_types [][2][16] = {
	{{ 	  3, 0x1C, 0x00,		0x00,		0x00, 0x00, 0x00, 0x00,		0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F }, "CNROM           " },
	{{ 	 66, 0x1C, 0x00,		0x00,		0xF0, 0xF0, 0xF0, 0xF0,		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F }, "GNROM           " },
	{{ 	  7, 0x0C, 0x00,		0x10,		0x0F, 0x0F, 0x0F, 0x0F,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "AOROM           " },
	{{ 	 34, 0x0C, 0x00,		0x00,		0x3F, 0x3F, 0x3F, 0x3F,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "BNROM           " },
	{{ 	  2, 0x04, 0x00,		0x00,		0x0F, 0x0F, 0x00, 0x00,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "UNROM           " },
	{{ 	180, 0x06, 0x00,		0x00,		0x00, 0x00, 0x7F, 0x7F,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "UNROM + HC08    " },
	{{ 	152, 0x14, 0x00,		0x80,		0x70, 0x70, 0x00, 0x00,		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F }, "Arkanoid 2      " },
	{{ 	 78, 0x14, 0x00,		0x08,		0x07, 0x07, 0x00, 0x00,		0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0 }, "Holy Diver      " },
	{{ 	 11, 0x1C, 0x00,		0x00,		0x03, 0x03, 0x03, 0x03,		0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0 }, "Color Dreams    " },
	{{ 	148, 0x1C, 0x00,		0x00,		0x08, 0x08, 0x08, 0x08,		0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07 }, "Tengen 800008   " },
	{{ 	149, 0x1C, 0x00,		0x00,		0x00, 0x00, 0x00, 0x00,		0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 }, "Mahjong 16      " },
	{{ 	 77, 0x4C, 0x00,		0x00,		0x0F, 0x0F, 0x0F, 0x0F,		0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "Napoleon Senki  " },
	{{ 	 89, 0x14, 0x00,		0x08,		0x70, 0x70, 0x00, 0x00,		0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87, 0x87 }, "Mito Koumon     " },
	{{ 	 93, 0x04, 0x01,		0x00,		0xF0, 0xF0, 0x00, 0x00,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "Fantasy Zone    " },
	{{ 	107, 0x1C, 0x00,		0x00,		0x3E, 0x3E, 0x3E, 0x3E,		0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F }, "Magic Dragon    " },
	{{ 	 29, 0x04, 0x00,		0x00,		0x1C, 0x1C, 0x00, 0x00,		0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03 }, "Sealie Computing" },
	{{ 	 96, 0x0C, 0x00,		0x00,		0x03, 0x03, 0x03, 0x03,		0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 }, "Oeka Kids       " },
	{{ 	188, 0x04, 0x00,		0x20,		0x0F, 0x0F, 0x00, 0x00,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "Karaoke Studio  " },
	{{ 	 13, 0x3C, 0x00,		0x00,		0x00, 0x00, 0x00, 0x00,		0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03 }, "CPROM           " },
	{{ 	 30, 0x04, 0x00,		0x00,		0x1F, 0x1F, 0x00, 0x00,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "UNROM-512  8K   " },
	{{ 	 30, 0x04, 0x00,		0x80,		0x1F, 0x1F, 0x00, 0x00,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "UNROM-512  8K MM" },
	{{ 	 30, 0x04, 0x00,		0x00,		0x1F, 0x1F, 0x00, 0x00,		0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60 }, "UNROM-512 32K   " },
	{{ 	 30, 0x04, 0x00,		0x80,		0x1F, 0x1F, 0x00, 0x00,		0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60 }, "UNROM-512 32K MM" },
	{{ 	 94, 0x04, 0x00,		0x00,		0x1C, 0x1C, 0x00, 0x00,		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, "UN1ROM          " },
	{{ 	 70, 0x14, 0x00,		0x00,		0xF0, 0xF0, 0x00, 0x00,		0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F }, "Family Trainer  " }
};
*/
//#define			simple_cnt (sizeof (simple_types) / 32)
/*
union {

		unsigned char	byte	[8];
		unsigned short	hword	[4];
		unsigned int	word	[2];

} error_info [BCMTABLE_SIZE / 32];
*/
//---------------------------------------------------------------------------------------------------------

#define			CHR_KEYS			16
const uint16_t	CHR_keys [4][CHR_KEYS] = {
	{ 0x0010, 0x014A, 0x0001, 0x024D, 0x03D0, 0x0040, 0x0076, 0x0238, 0x02B1, 0x0360, 0x03FF,      0,      0,      0,      0,      0 },		//	1K
	{ 0x0232, 0x0014, 0x03DA, 0x0541, 0x0001, 0x07D0, 0x0040, 0x00E0, 0x0238, 0x0240, 0x02B1, 0x0330, 0x0474, 0x0760, 0x07FF,      0 },		//	2K
	{ 0x0412, 0x0014, 0x01CB, 0x0001, 0x0A50, 0x0E2D, 0x0040, 0x0760, 0x0FD0, 0x0238, 0x0330, 0x0474, 0x0AB1, 0x0FFF,      0,      0 },		//	4K
	{ 0x1C46, 0x131B, 0x0430, 0x1B42, 0x0000, 0x0040, 0x0238, 0x0760, 0x0800, 0x1AB1, 0x1D00, 0x1FC7, 0x1FFF,      0,      0,      0 }};	//	8K

#define			PRG_KEYS			 8
const uint16_t  PRG_keys [3][PRG_KEYS] = {
	{ 0x1FFD, 0x0100, 0x0040, 0x0025, 0x10C1, 0x1400, 0x1FA5, 0x1FC8 },		//	 8K
	{ 0x0105, 0x2040, 0x3FFB, 0x0025, 0x10C1, 0x3400, 0x3FA5, 0x3FC8 },		//	16K
	{ 0x1040, 0x4025, 0x7400, 0x10C1,      0,      0,      0,      0 }};	//	32K

#define ROMSEL					0x0080
#define M2						0x0100
#define CPU_RW					0x0200
#define CART_POWER				0x2000
#define NT_A10					0x2000
#define PPU_RD					0x4000
#define PPU_WR					0x8000
#define b31						0x80000000

typedef void	(*cart_io) (uint8_t *buf, uint32_t ROM_addr);
typedef void	(*bank_sw) (uint32_t addr);
typedef void	(*save_en) (void);

//typedef int		(*mirr_sw) (uint32_t d);
typedef union {

		void	(*mirr) (uint32_t d);
		int		(*test) (uint32_t d);

} mirr_sw;

static void scan_PRG	(void);
static void NUL_xxx		(void		) {	  };
static void NUL_xXx		(uint32_t b	) {	  };
static int  goto_bank	(uint32_t b		  );
static void PRG_xxx		(uint32_t ROM_addr);
static void CHR_xxx		(uint32_t ROM_addr);
static void null_io		(register ptr_all_types buf, register uint32_t file_ofs) {}



typedef struct { uint8_t data [26]; uint16_t first_cluster; uint32_t size; } DIRinfo;
typedef struct {
	uint8_t		map_no;			//	iNES mapper #
	uint8_t		init;			//	contains an init value, usually a bank shift value (or SRAM enable for MMC3/MMC6)
	uint16_t	SAV_base;		//	used to detect SRAM when smaller than 8K
	uint8_t		SAV_msb;		//	bit offset to start SRAM size test at
	uint8_t		PRG_msb;		//	bit offset to start PRG  size test at
	uint8_t		PRG_bank_size;	//	in bits from the LSB
	uint8_t		PRG_ofs;		//	offset value in bank size units
	uint8_t		CHR_msb;
	uint8_t		CHR_bank_size;
	uint8_t		CHR_ofs;
	uint8_t		reserved_1;
	uint32_t	reserved_2;
	uint32_t	reserved_3;
	save_en		INIT_;			//	sets the banking modes & enables SRAM
	save_en		DOWN_;			//	disables the SRAM
	mirr_sw		MIR_;			//	mirroring control abstract used to identify most ASIC types
	bank_sw		SAV_B;			//	save RAM banking abstraction
	bank_sw		PRG_B;			//	PRG ROM banking abstraction
	bank_sw		CHR_B;			//	CHR ROM banking abstraction
	save_en		TEST_;			//	mapper-specific tests (i.e., IRQ, 4-screen NTRAM, etc.)
	char		text [16];		//	mapper type designation
} MAPinfo;

// define the output parameters from autodetection
typedef struct {

	unsigned int	PRG_ROM_size;
	unsigned int	PRG_bank_mask;
	unsigned short	PRG_mask;
	unsigned char	PRG_B_mask;
	unsigned char	reserved_1;

	unsigned int	CHR_ROM_size;
	unsigned int	CHR_bank_mask;
	unsigned short	CHR_mask;
	unsigned char	CHR_B_mask;
	unsigned char	reserved_2;

	unsigned int	SRAM_size;
	unsigned int	SAV_bank_mask;
	unsigned short	SAV_mask;
	unsigned char	SAV_B_mask;
	unsigned char	SAV_B_ofs;

	unsigned short	SAV_ofs;
	unsigned short	base;		//	register base for mapper 268

//	unsigned short	PRG_ofs;
//	unsigned char	PRG_B_ofs;
//	unsigned short	CHR_ofs;
//	unsigned char	CHR_B_ofs;

	// the index values here are always grouped as 8 in this sequence!
	unsigned char	header_override_index;
	unsigned char	PRG_override_index;
	unsigned char	CHR_override_index;
	unsigned char	SAV_override_index;
	unsigned char	INIT_override_index;
	unsigned char	DOWN_override_index;
	unsigned char	TEST_override_index;
	unsigned char	name_override_index;

	// reserved for future expansion
	unsigned char	reserved_4 [5];
	unsigned char	SRAM_err;
	unsigned char	init_mode;
	unsigned char	mode;

} t_v;
t_v v;

const uint8_t	NES_header_init [32]	= { 0x4E, 0x45, 0x53, 0x1A, 0x02, 0x01, 0x00, 0x00, 0,0,0,0,0,0,0,0,
										0, 0x80, 0, 0, 0, 0, 0, 0, 15,13, 0,0,0,0, 13, 10};

const MAPinfo	map_init			= { 0,0,	0x6000,12,	14,15,1,	12,13,0,	0,0,0,	NUL_xxx, NUL_xxx, {NUL_xXx}, NUL_xXx, PRG_xxx, CHR_xxx,  NUL_xxx,	"NROM 128/256    " };

char			report_txt	[512];
uint16_t		data_table	[256];
uint16_t		root_table	[256];
uint8_t			PRG_bank_tbl[256];
uint8_t			CHR_bank_tbl[256];
uint8_t			bank_sig	[CHR_KEYS];

union {

	uint8_t			b			[32];
	t_h_override	r;

} info;

uint8_t			score		[ 16];		// for simple_types bit-bang tests

uint8_t			PRG_erase	[128];
uint8_t			CHR_erase	[128];

MAPinfo			map;

uint32_t		files_dirty;

uint32_t		CPU_W_enable;
uint32_t		CPU_W_disable;
uint32_t		bs_helper;
uint32_t		old_PRG_bank;			// current PRG bank
uint32_t		old_CHR_bank;			// current CHR bank
uint32_t		old_SAV_bank;			// current SAV bank

uint32_t		timeout;
uint32_t		(* chr_src		)	(register uint32_t addr);
uint32_t		(* sram_src_R	)	(register uint32_t addr);
void			(* sram_src_W	)	(register uint32_t addr, register uint32_t data);

unsigned short	seq_ctr;
unsigned char *	src_pos;
unsigned char *	dest_pos;

short			settings_old_mod_time;

uint8_t			cur_bank;
uint8_t			SAV_size;				//	total amount of SRAM in 2^n, battery backed or not
uint8_t			CHR_RAM;				//	non-zero if CHR-RAM detected
uint8_t			override_erase;
uint8_t			bcmtable_erase;
uint8_t			settings_dirty;
uint8_t			pull;
uint8_t			bus_contention;
uint8_t			info_4_mask;			//	indicates validity of info [4] after call to scan PRG
uint8_t			info_5_mask;			//	indicates validity of info [5] after call to scan CHR
uint8_t			simple_cnt;

//---------------------------------------------------------------------------------------------------------
// at 72 MHz, delay provides CPU-clock accurate delays for up to a minute.
void chill (register uint32_t cycles) {

	register uint32_t i = DWT -> CYCCNT;
	while ((DWT -> CYCCNT - i) < cycles);

}

// disable any SRAM, tri-state all GPIO lines, and cut the power to the cart
static void shut_down (void) {
  	map.DOWN_ ();				// disable SRAM
	GPIOA->CRL = 0x44444444;	// disable A8-A14, ROMSEL
	GPIOB->CRL = 0x44444444;	// disable A0-A7
	GPIOB->CRH = 0x44444444;	// disable D0-D7
	GPIOA->CRH = 0x44444444;	// disable M2, R/W, PPU_RD, PPU_WR
	GPIOC->BSRR = CART_POWER;	// shut off cart power
	old_PRG_bank = 0xFFFFFFFF;
	old_CHR_bank = 0xFFFFFFFF;
	old_SAV_bank = 0xFFFFFFFF;
	timeout = 0;
}

// tries to discharge the cart by intentionally causing bus shorts
void discharge () {

	GPIOA -> ODR = 0x0000 | CPU_RW | PPU_WR;
	GPIOB -> ODR = 0x0000;
	GPIOB -> CRL = 0x33333333;				// enable A0-A7 outputs
	GPIOB -> CRH = 0x33333333;				// enable D0-D7
	GPIOA -> CRL = 0x33333333;				// enable A8-A14, ROMSEL outputs
	GPIOA -> CRH = 0x43444443;				// enable M2, PPU_RD

}

void systick (void) {

	if (timeout) {
		timeout --;
		if (! timeout)
			shut_down ();

	}
	if (files_dirty) {
		files_dirty --;
		if (! files_dirty) {

			if (settings_dirty)				import_settings ((t_settings *) debug.buf, 0);
			ptr_krnl -> media_change_handler ();

		}
	}
}

// the value of timeout does not change inside STORAGE_xx_FS methods.
static void check_power_OK (void) {

	if (!timeout) {

		timeout = ch.items.idle_powerdown_ms + 1;			// reset timeout counter
		GPIOC->BRR = CART_POWER;							// energize cart
//		chill (ch.items.unit_power_warmup * MSEC_const >> 1);
		GPIOA -> ODR = 0x2400 | ROMSEL | CPU_RW | PPU_RD | PPU_WR;
		GPIOB -> ODR = 0x0000;
		GPIOA -> CRH = 0x33444833;							// enable M2, R/W, PPU_RD, PPU_WR outputs
		GPIOA -> CRL = 0x33333333;							// enable A8-A14, ROMSEL outputs
		GPIOB -> CRL = 0x33333333;							// enable A0-A7 outputs
		GPIOB -> CRH = 0x44444444;							// disable D0-D7
		old_PRG_bank = 0xFFFFFFFF;
		old_CHR_bank = 0xFFFFFFFF;
		old_SAV_bank = 0xFFFFFFFF;
		chill (ch.items.unit_power_warmup * MSEC_const);	// waste some time while cart energizes
		map.INIT_ ();										// configure banking modes and enable SRAM
		return;

	}
	timeout = ch.items.idle_powerdown_ms + 1;				// reset timeout counter

}

uint32_t CPU_R (register uint32_t addr) {

	// phase 1: data & address setup
	check_power_OK ();									// phase 0: turn on cart power if not already on
	GPIOB->ODR = (pull << 8) | (addr & 255);			// low address
	GPIOA->ODR = ((addr >> 8) & 0x7F)					// high address
				| ROMSEL | CPU_RW | PPU_RD | PPU_WR		// control signals
				| 0x2400;								// pull-up for IRQ & NT.A10
	chill (10);

	// phase 2: assert clock line(s)
	GPIOA->BSRR = ((addr << 8) & 0x800000) | M2;		// enable M2 and /ROMSEL if A15 = 1
	chill (20);

	//phase 3: sample data byte and negate clock lines
	register uint32_t i = GPIOB->IDR >> 8 & 255;
	GPIOA->BSRR = M2 << 16 | ROMSEL;					// clears M2 and sets ROMSEL
	return (i);
}

uint32_t CPU_W (register uint32_t addr, register uint32_t data) {

	// phase 1: data & address setup
	check_power_OK ();									// phase 0: turn on cart power if not already on
	GPIOB->ODR = (data << 8) | (addr & 255);			// data and low address
	GPIOA->ODR = ((addr >> 8) & 0x7F)					// high address
				| ROMSEL | PPU_RD | PPU_WR				// control signals
				| 0x2400;								// pull-up for IRQ & NT.A10
	chill (10);

	// phase 2: assert clock line(s) and enable output register
	if (addr & 0x8000)	GPIOB->CRH = CPU_W_enable;
	else				GPIOB->CRH = 0x33333333;		// enable data bus if addr in $0000 - $7FFF
	GPIOA->BSRR = ((addr << 8) & 0x800000) | M2;		// enable M2 and /ROMSEL if A15 = 1
	chill (20);

	//phase 3: negate clock line and disable output register
	register uint32_t i = GPIOB->IDR >> 8 & 255;
//	GPIOA->BSRR = M2 << 16 | ROMSEL;					// clear M2 and set ROMSEL

	// fixes a bug with mappers that sample /ROMSEL on the falling edge of M2
	register GPIO_TypeDef *	t = GPIOA;
	register unsigned int	s = ROMSEL;
	t -> BRR  = M2;
	t -> BSRR = s;
//	chill (ch.items.CPU_W_bus_settle);

	//phase 4: cool down
	GPIOB->CRH = CPU_W_disable;							// disable data bus
	GPIOA->BSRR = CPU_RW;								// clear CPU R/W line
	return (i);
}

uint32_t PPU_R (register uint32_t addr) {

	// phase 1: data & address setup
	check_power_OK ();									// phase 0: turn on cart power if not already on
	GPIOB->ODR = addr & 255;							// data and low address
	GPIOA->ODR = ((~addr >> 7) & 0x40)					// inverted A13
				| ((addr >> 8) & 0x3F)					// high address
				| ROMSEL | CPU_RW | PPU_RD | PPU_WR		// control signals
				| 0x2400;  								// pull-up for IRQ & NT.A10
	chill (10);

	// phase 2: assert clock line
	GPIOA->BRR = PPU_RD;
	chill (20);

	//phase 3: sample data byte and negate clock lines
	register uint32_t i = GPIOB->IDR >> 8 & 255;
	GPIOA->BSRR = PPU_RD;
	return (i);
}

void PPU_W (register uint32_t addr, register uint32_t data) {

	// phase 1: data & address setup
	check_power_OK ();									// phase 0: turn on cart power if not already on
	GPIOB->ODR = (data << 8) | (addr & 255);			// data and low address
	GPIOA->ODR = ((~addr >> 7) & 0x40)					// inverted A13
				| ((addr >> 8) & 0x3F)					// high address
				| ROMSEL | CPU_RW | PPU_RD | PPU_WR		// control signals
				| 0x2400;  								// pull-up for IRQ & NT.A10
	chill (10);

	// phase 2: assert clock line and enable output register
	GPIOB->CRH = 0x33333333;							// enable data bus
	GPIOA->BRR = PPU_WR;
	chill (20);

	//phase 3: negate clock line
	GPIOA->BSRR = PPU_WR;
//	chill (ch.items.PPU_W_bus_settle);

	//phase 4: cool down
	GPIOB->CRH = CPU_W_disable;							// disable output register

}

//***************************************************************************************
//*************** Manual override cart access methods ***********************************
//***************************************************************************************

static int verify_sequence (register unsigned int * j) {

	register char t;
	do {

		// check the bounds
		if (j [0] >= (OVERRIDE_SIZE / 32))	return 1;

		//
		register t_s_override * seq = & OVERRIDE_BIN -> sequence [j [0]];

		// check the main signatures
		t = seq -> ID_string [3];
		if (seq -> CRLF		 	  != 0x0A0D
		||  seq -> SP2			  != '.'
		|| (seq -> data_dir		  != '.' && seq -> data_dir		 != '=' && seq -> data_dir != '?')
		|| (seq -> ID_string [0]  != 'P' && seq -> ID_string [0] != 'C')
		|| (seq -> ID_string [1]  != 'R' && seq -> ID_string [1] != 'H')
		|| (seq -> ID_string [2]  != 'G' && seq -> ID_string [2] != 'R')
		|| (t					  != ' ' && t					 != '!'))

			return 1;

		// check the address string
		for (register int i = 0; i < 16; i ++)	switch (seq -> address_map [i]) {

			default:			return 1;
			case '0' ... '1':
			case 'a' ... 'z':
			case 'A' ... 'Z':;

		}
		// check the data string
		for (register int i = 0; i < 8; i ++)	switch (seq -> data_map [i]) {

			default:			return 1;
			case '-':			bus_contention = 1;
			case '0' ... '1':
			case 'a' ... 'z':
			case 'A' ... 'Z':;

		}
		j [0] ++;

	} while (t == ' ');
	return 0;

}

static void MMC1 (register int a, register int d) {

	CPU_R (a); CPU_W (a, d		);
	CPU_R (a); CPU_W (a, d >> 1	);
	CPU_R (a); CPU_W (a, d >> 2	);
	CPU_R (a); CPU_W (a, d >> 3	);
	CPU_R (a); CPU_W (a, d >> 4	);

}

static int manual_bankswitch (register int i, register int bank_addr) {

	do {
CMP_LP:;
		// --------------------------------------------------------------------------------
		// construct the address
		register int addr = 0;
		register int bs_mask = 0xffff;
		for (register int j = 0; j < 16; j ++) {

			register int tmp = OVERRIDE_BIN -> sequence [i].address_map [j];
			switch (tmp) {

case '1':			addr += 0x8000 >> j;	break;								//	set the address bit
case 'A' ... 'Z':	addr += 0x8000 >> j;	tmp ^= 0x20;
case 'a' ... 'z':	addr += (bank_addr >>  (tmp - 'a') & 1) << (15 - j);
					bs_mask &= ~ (1 << (tmp - 'a'));

			}
		}
		// formulate the instruction
		register int data;
		register int mask;
		register int opcode = OVERRIDE_BIN -> sequence [i].data_dir
							+ OVERRIDE_BIN -> sequence [i].ID_string [0];

		// --------------------------------------------------------------------------------
		// * READ CYCLES *

		// update the bank_addr register
		switch (opcode) {

				// load bank address with read data based on data map
case 'C'+'.':	data = PPU_R (addr);	goto LOAD_BA;		//	PPU read sequence
case 'P'+'.':	data = CPU_R (addr);						//	CPU read sequence
LOAD_BA:		for (register int j = 0; j < 8; j ++) {

					register int tmp = OVERRIDE_BIN -> sequence [i].data_map [j];
					switch (tmp) {

case 'A' ... 'Z':		data		^= 128 >> j;	tmp ^= 0x20;	//	invert the data
case 'a' ... 'z':		tmp			-= 'a';
						mask		 = 1 << tmp;
						bank_addr	 = (bank_addr & ~ mask) | ((data >> j) << tmp & mask);

					}
				}
				continue;	//	skip the rest

		}
		// --------------------------------------------------------------------------------
		// * WRITE CYCLES and COMPARE FOR EQUALITY *

		// construct the data to write or compare
		data = 0;
		mask = 0;
		for (register int j = 0; j < 8; j ++) {

			register int tmp = OVERRIDE_BIN -> sequence [i].data_map [j];
			switch (tmp) {

default:			mask |=   128 >> j;		break;		//	unknown bits will force a read
case '1':			data |=   128 >> j;		break;		//	set the data bit
case 'A' ... 'Z':	data |=   128 >> j;		tmp ^= 0x20;
case 'a' ... 'z':	data ^=  (bank_addr >> (tmp - 'a') & 1) << (7 - j);	//break;
					bs_mask &= ~ (1 << (tmp - 'a'));
case '0':;//		data &= ~(128 >> j);	break;

			}
		}
		// determine the write type
		switch (opcode) {

				// PPU write sequence
case 'C'+'=':	if (mask				)	data |= mask & PPU_R (addr);
				PPU_W (addr, data);
				break;

				// CPU write sequence
case 'P'+'=':	if (mask				)	data |= mask & CPU_R (addr);
				if ((v.init_mode & 128	)
				&& (addr & 0x8000		)
				&& (~data & 128)		)	MMC1  (addr, data);
				else 						CPU_W (addr, data);
				break;

				// CHR compare sequenece
case 'C'+'?':	if (mask == 255)							return 0;	//	unconditional return
				data ^= PPU_R (addr);
				goto CMP_SEQ;

				// PRG compare sequence
case 'P'+'?':	if (mask == 255)							return 0;	//	unconditional return
				switch (addr) {

case 0 ... 31:		data ^= info.b [addr];					break;		//	iNES header @ CPU [$0000 - $001F]
case 256:			data ^= GPIOA -> IDR >> 10 & 1;			break;		//	IRQ port @ CPU [$0100]
case 257:			GPIOA -> BSRR = 0x540028;							//	A11 = A13 = 1; A10 = A12 = /A13 = 0
					chill (40);
					data ^= GPIOA -> IDR >> 13 & 1;			break;		//	NT.A10 port @ CPU [$0101]
default:			data ^= CPU_R (addr);					break;
case 272 ... 511:	chill (72 * (addr >> 4 & 15) << (addr & 15));		//	delay generator macros

				}
CMP_SEQ:		if (data & (mask ^ 255))					return 1;	//	fail if not an exact match
				bank_addr = (bank_addr | bs_mask) + 1;
				if		(bank_addr < 0x10000)				goto CMP_LP;//	repeat the same instruction until done
				else	 bank_addr = 0;									//	be sure to clear the register when done

		}

	} while (OVERRIDE_BIN -> sequence [i ++].ID_string [3] == ' ');
	return 0;

}

static void INIT_override (void) {

	if (v.init_mode & 128	)	CPU_W (0x8000, 0x80);		//	a shortcut for dealing with the MMC1
	if (v.init_mode & 1		)	manual_bankswitch (v.INIT_override_index, 0);
	if (v.init_mode & 4		)	{

		CPU_R (0x4800);			//	LO 1 screen mirroring mode
		CPU_R (0x5088);			//	enable CHR-FLASH access mode, enable CHR-RAM, set RAM bank 0
		for (register int i = 0; i < 256; i ++)	  PPU_W (i, i);

	}
}

static void DOWN_override (void					) { manual_bankswitch (v.DOWN_override_index,						0); }
static void PRG_override  (register int ROM_addr) { manual_bankswitch (v.PRG_override_index , ROM_addr >> info.r.PRG_bank_bits); }
static void CHR_override  (register int ROM_addr) { manual_bankswitch (v.CHR_override_index , ROM_addr >> info.r.CHR_bank_bits); }
static void SAV_override  (register int ROM_addr) { manual_bankswitch (v.SAV_override_index , ROM_addr >> v.SAV_B_ofs); }

// scan to find a mapper abstract header signature
// set c to index of overrides to skip over, or -1 to find any override with a TEST sequence
int find_map_header (register unsigned int * i, register int c) {

	// scan to end of file
	do {

		if (OVERRIDE_BIN -> header [i [0]].iNES_signature == 0x1A53454E) {

			if ((c < 0) && (OVERRIDE_BIN -> header [i [0]].SEQ_present & 4))	return 2;
			c --;
			if (c == 0)															return 1;

		}

	} while ((++ i [0]) < (OVERRIDE_SIZE / 32));
	return 0;

}

// verify mapper abstract
int check_map_abstract (register unsigned int * i) {

	register t_v * _v = & v;

	// erase all the index values
	memset (& _v -> header_override_index, 0, 8);

	// save the header index
	register unsigned int j = i [0] ++;		//	if (j >= (OVERRIDE_SIZE / 32))		return 11;
	register t_h_override *	hdr = & OVERRIDE_BIN -> header [j];

	// check header signatures
	if (hdr -> iNES_signature != 0x1A53454E)									return 1;
	if (hdr -> CRLF != 0x0A0D)													return 2;

	// check PRG-ROM size not zero
	register unsigned int t = (hdr -> PRG_ROM_size << 14) | ((hdr -> PRG_CHR_ROM_size_MSB & 15) << 22);
	if (! t)																	return 3;

	// any PRG bank sequence?
	if (1 << (hdr -> PRG_bank_bits) < t) {

		_v -> PRG_override_index = i [0];	if (verify_sequence (i))			return 6; }

	// any CHR bank sequence?
	t = (hdr -> CHR_ROM_size << 13) | ((hdr -> PRG_CHR_ROM_size_MSB >> 4) << 21);
	if (1 << (hdr -> CHR_bank_bits) < t) {

		_v -> CHR_override_index = i [0];	if (verify_sequence (i))			return 7; }

	// any PRG SAV bank sequence?
					  	  t = hdr -> PRG_RAM_size >> 4;
	register unsigned int u = hdr -> CHR_RAM_size >> 4;
	if		( t
	&&		((t + 6)
	>		hdr -> PRG_bank_bits_SRAM)) {
		_v -> SAV_override_index = i [0];	if (verify_sequence (i))			return 8; }

	// any CHR SAV bank sequence?
	else if ( u
	&&		((u + 6)
	>		hdr -> CHR_bank_bits_SRAM)) {
		_v -> SAV_override_index = i [0];	if (verify_sequence (i))			return 8; }

	// any INIT sequence?
	if (hdr -> SEQ_present & 1) {
		_v -> INIT_override_index = i [0];	if (verify_sequence (i))			return 4; }

	// any DOWN sequence?
	if (hdr -> SEQ_present & 2) {
		_v -> DOWN_override_index = i [0];	if (verify_sequence (i))			return 5; }

	// any TEST sequence?
	if (hdr -> SEQ_present & 4) {
		_v -> TEST_override_index = i [0];	if (verify_sequence (i))			return 9; }

	// test the bounds on the NAME slot (even if the name isn't present)
	_v -> name_override_index = i [0];		if (i [0] >= (OVERRIDE_SIZE / 32))	return 10;

	// assign the header index
	_v -> header_override_index = j;
	return 0;

}
/*
const char check_err_codes [][32]	= {

	"mapper abstract not found       ",
	"header signature 1 not found    ",
	"header signature 2 not found    ",
	"PRG-ROM size is set to zero     ",
	"bad initialization sequence     ",
	"bad power-down sequence         ",
	"bad PRG bankswitching sequence  ",
	"bad CHR bankswitching sequence  ",
	"bad SAV bankswitching sequence  ",
	"bad TEST sequence               ",
	"mapper abstract out of bounds   ",
//	"bad MAP name entry              "

};
*/
void setup_override () {

	// set up the mapper header index
//	register int				i = v.header_override_index;
	register t_h_override *		j = & OVERRIDE_BIN -> header [v.header_override_index];

	// set up iNES header
	memcpy (& info, j, 32);

	// calculate the PRG-ROM size
	v.PRG_ROM_size			= (info.b [4] << 14) | (info.b [9] & 15) << 22;	//	PRG 16KB bank count

	// set up the PRG params
//	info.r.PRG_bank_ofs		= j -> PRG_bank_ofs;
//	info.r.PRG_bank_bits	= j -> PRG_bank_bits;
	v.PRG_mask				= (1 << j -> PRG_bank_bits) - 1;
	v.PRG_bank_mask			= v.PRG_ROM_size - v.PRG_mask - 1;
	if (v.PRG_bank_mask) {

		// check the PRG banking sequence
		map.PRG_B					 = (bank_sw) PRG_override;
		v.PRG_bank_mask				|= b31;

	}
	// compute the CHR-ROM size
	v.CHR_ROM_size = (info.b [5] << 13) | (info.b [9] & 240) << 17;	//	CHR  8KB bank count
	if (v.CHR_ROM_size) {

		// set up the CHR params
//		info.r.CHR_bank_ofs		= j -> CHR_bank_ofs;
//		info.r.CHR_bank_bits	= j -> CHR_bank_bits;
		v.CHR_mask				= (1 << j -> CHR_bank_bits) - 1;
		v.CHR_bank_mask			= v.CHR_ROM_size - v.CHR_mask - 1;
		if (v.CHR_bank_mask) {

			// check the CHR banking sequence
			map.CHR_B					 = (bank_sw) CHR_override;
			v.CHR_bank_mask				|= b31;

		}
	}
	// set up the PRG SAV
	v.SAV_ofs				= 0;	//	not used for manual mode
	if ((info.b [10] & 240) && j -> PRG_bank_bits_SRAM) {

		// set up the PRG save file parameters
		v.SAV_B_ofs				= j -> PRG_bank_bits_SRAM;
		map.SAV_base			= j -> PRG_bank_ofs_SRAM;
//		v.SRAM_size				= 64 << (info.b [10] >> 4);
		SAV_size				= (info.b [10] >> 4) + 6;
		sram_src_R				= CPU_R;
		sram_src_W				= (void *) CPU_W;

	} else if ((info.b [11] & 240) && j -> CHR_bank_bits_SRAM) {

		// set up the CHR save file parameters
		v.SAV_B_ofs				= j -> CHR_bank_bits_SRAM;
		map.SAV_base			= j -> CHR_bank_ofs_SRAM;
//		v.SRAM_size				= 64 << (info.b [11] >> 4);
		SAV_size				= (info.b [11] >> 4) + 6;
		sram_src_R				= PPU_R;
		sram_src_W				= PPU_W;

	}
	v.SRAM_size				= 1 << SAV_size;
	v.SAV_mask				= (1 << v.SAV_B_ofs) - 1;
	v.SAV_bank_mask			= v.SRAM_size - v.SAV_mask - 1;
	if (v.SAV_bank_mask) {

		// check the SAV banking sequence
		map.SAV_B					 = (bank_sw) SAV_override;
		v.SAV_bank_mask				|= b31;

	}
	// get the mode bits
	v.mode = j -> mode;

	// test for bs helper flag
	if (v.mode & 32) bs_helper = b31;

	// test for read CHR-ROM from PRG-ROM
	if (v.mode & 64) chr_src = CPU_R;

	// check for INIT sequence, MMC1 or NES-RED-PILL modes
	v.init_mode = (j -> SEQ_present & 1) | (j -> mode & 0x84);
	if (v.init_mode)				map.INIT_ = (save_en) INIT_override;

	// check the powerdown sequence
	if (j -> SEQ_present & 2)		map.DOWN_ = (save_en) DOWN_override;

	// check if the mapper footer present
	if (OVERRIDE_BIN -> name [v.name_override_index].signature == 0x0A0D2140)
			memcpy (map.text, & OVERRIDE_BIN -> name [v.name_override_index], 16);
	else {
			memcpy (map.text, "NO NAME MAPPER ", 15);	map.text [15] = ' ' + ch.items.override_mode_index; }

	// configure the GPIO
	CPU_W_enable	= 0x33333333 + (0x11111111 * (ch.items.bus_disable & bus_contention));
	CPU_W_disable	= 0x44444444;
	old_PRG_bank	= 0xFFFFFFFF;
	old_CHR_bank	= 0xFFFFFFFF;
	old_SAV_bank	= 0xFFFFFFFF;

}

//***************************************************************************************
//*************** Mapper-specific cart access methods ***********************************
//***************************************************************************************
// These methods provide a byte-granular address bankswitching interface to the mapper hardware.

//---------------------------------------------------------------------------------------
// simple_types

static void PRG_xxx (register uint32_t ROM_addr) { goto_bank (PRG_bank_tbl [ROM_addr >> info.r.PRG_bank_bits & v.PRG_B_mask]); }
static void CHR_xxx (register uint32_t ROM_addr) { goto_bank (CHR_bank_tbl [ROM_addr >> info.r.CHR_bank_bits & v.CHR_B_mask]); }

//---------------------------------------------------------------------------------------
// MMC1

static void INIT_001  (void						 ) { CPU_W (0x8000, 0x80); MMC1 (0xE000, 0x00); MMC1 (0x8000, 0x1E); }
static void DOWN_001  (void						 ) { MMC1  (0xE000, 0x10); }
static void MIR_001   (register uint32_t d		 ) { MMC1  (0x8000, (d & 1) | 0x1E); }
static void SAV_001	  (register uint32_t ROM_addr) { int i = ROM_addr >> map.init; MMC1 (0xA000, i); MMC1 (0xC000, i); }
static void PRG_001	  (register uint32_t ROM_addr) { ROM_addr >>= 14; MMC1  (0xE000, ROM_addr & 15); MMC1 (0xA000, ROM_addr); MMC1 (0xC000, ROM_addr); }
static void CHR_001	  (register uint32_t ROM_addr) { MMC1  (0xA000, ROM_addr >> 12 & 31); }
static void TEST_001  (void						 ) { if (SAV_size == 14) map.init = 10; if (info.b [4] <= 2) { v.PRG_mask = 0x7FFF; info.r.PRG_bank_ofs = 0x8000; v.PRG_bank_mask = b31; }}

//---------------------------------------------------------------------------------------
// MMC2 and MMC4

static void MIR_010 (register uint32_t d	   ) { CPU_W (0xF000, d &  1		   ); CPU_W (0xE000,			  0); }
static void PRG_010 (register uint32_t ROM_addr) { CPU_W (0xA000, ROM_addr >> map.init); }
static void CHR_010	(register uint32_t ROM_addr) { CPU_W (0xB000, ROM_addr >> 12); CPU_W (0xC000, ROM_addr >> 12); }

static void TEST_010 (void) {

	if (info.b [4] < 3) {
		map.PRG_ofs		= 4;
		map.init		= 13;
		map.text [12]	= '2';
		info.b [6]		= 0x90;
		scan_PRG ();
	}
	else {
		info.r.PRG_bank_ofs	 = 0x8000;
		v.PRG_mask			^= 0x2000;
		v.PRG_bank_mask		^= 0x2000;
	}
}

//---------------------------------------------------------------------------------------
// MMC3

//static void INIT_M25  (void)					   { CPU_W (0x5000,	0x00); CPU_W (0x5100, 0x00); CPU_W (0x8000, 0x20); CPU_W (0xA001, 0xB0); }
static void INIT_004  (void						 ) { CPU_W (0x8000,  0x20); CPU_W (0xA001, map.init); }
static void DOWN_004  (void						 ) { CPU_W (0xA001,  0x00); }
static void MIR_004   (register uint32_t d		 ) { CPU_W (0xA000, d & 1); CPU_W (0x8000, 0); }
//static void PRG_M25   (register uint32_t ROM_addr) { CPU_W (0x8000,	0x26); CPU_W (0x8001, ROM_addr >> 13 ^ info.b [5]); }
static void PRG_004   (register uint32_t ROM_addr) { CPU_W (0x8000,  0x26); CPU_W (0x8001, ROM_addr >> 13); }
static void CHR_004   (register uint32_t ROM_addr) { CPU_W (0x8000,  0x22); CPU_W (0x8001, ROM_addr >> 10); }

static void TEST_004  (void				) {

	// set save port base back to standatd location
	map.SAV_base			 = 0x6000;
	info.r.PRG_bank_ofs_SRAM = 0x6000;

	// test for RAMBO-1
	CPU_W (0xC000, 1);
	CPU_W (0xC001, 1);
	CPU_W (0xE000, 0);
	CPU_W (0xE001, 0);
	for (int i = 0; i < 8; i ++)	CPU_R (0x8000);					//	prime the IRQ counter
	if (!(GPIOA -> IDR & 0x0400)) {

		memcpy (map.text, "TENGEN RAMBO-   ", 16);
		map.text [13] = map.reserved_2;
		map.text [14] = map.reserved_3;
		info.b [6] = (info.b [6] & 0x0F) | (map.reserved_1 <<	4);		//	set low nibble of alt. mapper number
		info.b [7] = (info.b [7] & 0x0F) | (map.reserved_1 & 0xF0); 	//	set high nibble of alt. mapper number
		CPU_W (0xE000, 0);												//	clear IRQ line
		return;

	}
	// test for MMC6
	switch (SAV_size) {

case 10:
		info.r.PRG_bank_bits_SRAM = 10;
		map.init = 0xF0;
		CPU_W (0xA001, map.init);
		map.text [12] = '6';
		info.b [8] = 0x10;			//	.NES 2.0 submapper number for MMC6
		v.SAV_ofs = 0x0C00;			//	fix for handling 8KB file writes to MMC6
		return;

	}
	// test for "NES 240" (aka MOTHER 25th anniversary)
	register int i;

	// test for TQROM
	CPU_W (0x8000, 0x22);
	CPU_W (0x8001, 0x00); i =  PPU_R (0x1000);		 PPU_W (0x1000, ~i); if (PPU_R (0x1000) != i)	return;	//	test the ROM bank
	CPU_W (0x8001, 0x40); i = ~PPU_R (0x1000) & 255; PPU_W (0x1000,  i); if (PPU_R (0x1000) != i)	return;	//	test the RAM bank

	// setup header for TQROM
	memcpy (& map.text [9], "TQROM", 5);
	info.b [5] >>= 1;
	info.b [6] = (info.b [6] & 0x0F) | 0x70;	//	set low nibble of alt. mapper number
	info.b [7] = (info.b [7] & 0x0F) | 0x70; 	//	set high nibble of alt. mapper number

}

static void MIR_118   (register uint32_t d) {

	 CPU_W (0x8000, 0x82); CPU_W (0x8001, 0);
	 CPU_W (0x8000, 0x83); CPU_W (0x8001, d << 7 ^ 0x80);
	 CPU_W (0x8000, 0x84); CPU_W (0x8001, d << 7);
	 CPU_W (0x8000, 0x85); CPU_W (0x8001, 1);

}

//---------------------------------------------------------------------------------------
// MMC5

static void INIT_005 (void) {

	CPU_W (0x2000, 0);		//	set MMC5's sprite size bit to 0
	CPU_W (0x2001, 0);		//	clear MMC5's rendering bits
	CPU_W (0x5100, 3);		//	PRG bank mode = 8K
	CPU_W (0x5101, 0);		//	CHR bank mode = 8K
	CPU_W (0x5104, 2);		//	ExRAM mode = ordinary RAM
	CPU_W (0x5105, 0x44);	//	mirroring mode = vertical
	CPU_W (0x5113, 0);		//	SRAM bank setup
	CPU_W (0x5102, 2);		//	SRAM lock 1
	CPU_W (0x5103, 1);		//	SRAM lock 2

}

static void DOWN_005  (void						 ) { CPU_W (0x5102, 0); CPU_W (0x5103, 0);	}	//	SRAM lock 2
static void MIR_005   (register uint32_t d		 ) { CPU_W (0x5105, (d & 1) * 12 + 0x44 ); PPU_R (0x2000); }
static void SAV_005   (register uint32_t ROM_addr) { CPU_W (0x5113, ROM_addr >> 13);		}	// map.init); }
static void PRG_005   (register uint32_t ROM_addr) { CPU_W (0x5114, ROM_addr >> 13 | 128);	}
static void CHR_005   (register uint32_t ROM_addr) { CPU_W (0x5127, ROM_addr >> 13		);	}	//	{ CPU_W (0x5130, ROM_addr >> 18	   ); CPU_W (0x5120, ROM_addr >> 10);}
//static void TEST_005  (void						 ) { if (SAV_size == 14) map.init = 11	; }

//---------------------------------------------------------------------------------------
// ANROM

static void MIR_007	  (register uint32_t d		 ) { CPU_W (0x8000, d << 4 & 16	  ); }
static void PRG_007   (register uint32_t ROM_addr) { CPU_W (0x8000, ROM_addr >> 15); }

//---------------------------------------------------------------------------------------
// JALECO SS88006

static void INIT_018  (void						 ) { CPU_W (0x9002, 3); }
static void DOWN_018  (void						 ) { CPU_W (0x9002, 0); }
static void MIR_018   (register uint32_t d		 ) { CPU_W (0xF002, (d & 1) ^ 1	 ); CPU_W (0x8000,				0); }
static void PRG_018   (register uint32_t ROM_addr) { CPU_W (0x8000, ROM_addr >> 13); CPU_W (0x8001, ROM_addr >> 17); }
static void CHR_018   (register uint32_t ROM_addr) { CPU_W (0xA000, ROM_addr >> 10); CPU_W (0xA001, ROM_addr >> 14); }

//---------------------------------------------------------------------------------------
// NAMCO 163

static void INIT_019 (void						 ) { CPU_W (0xE800, 0xC0); CPU_W (0xF800, 0x40); }
static void DOWN_019 (void						 ) { CPU_W (0xF800, 0x00); }
static void MIR_019  (register uint32_t d		 ) { CPU_W (0xC000, 0xFE); CPU_W (0xC800, (d ^ 1) | 0xFE); CPU_W (0xD000, d | 0xFE); CPU_W (0xD800, 0xFF); }
static void PRG_019  (register uint32_t ROM_addr ) { CPU_W (0xE800, ROM_addr >> 13 | 0xC0); }
static void CHR_019  (register uint32_t ROM_addr ) { CPU_W (0x8000, ROM_addr >> 10		); }

//---------------------------------------------------------------------------------------
// VRC6a

static void INIT_024  (void						 ) { CPU_W (0xB003, 0xA0); }
static void DOWN_024  (void						 ) { CPU_W (0xB003, 0x20); }
static void MIR_024   (register uint32_t d		 ) { CPU_W (0xB003, ((d << 2) & 4) | 0xA0); CPU_W (0x8000, 0); }
static void PRG_024   (register uint32_t ROM_addr) { CPU_W (0xC000, ROM_addr >> 13); }
static void CHR_024   (register uint32_t ROM_addr) { CPU_W (0xD000, ROM_addr >> 10); }

//---------------------------------------------------------------------------------------
// VRC4b

static void INIT_025  (void						 ) { CPU_W (0x9003, 0); }
static void MIR_025   (register uint32_t d		 ) { CPU_W (0x9000, d &  1		 ); CPU_W (0x9003, 0			 ); }
static void PRG_025   (register uint32_t ROM_addr) { CPU_W (0x8000, ROM_addr >> 13); }
static void CHR_025   (register uint32_t ROM_addr) { CPU_W (0xB000, ROM_addr >> 10); CPU_W (0xB002, ROM_addr >> 14); }

//---------------------------------------------------------------------------------------
// TAITO TC0190

static void MIR_033   (register uint32_t d		 ) { CPU_W (0x8000, d << 6 & 0x40); CPU_W (0x8001, 0); }
static void CHR_033   (register uint32_t ROM_addr) { CPU_W (0x8002, ROM_addr >> 11); }
#define PRG_033 PRG_025
//static void PRG_033   (register uint32_t ROM_addr) { CPU_W (0x8000, ROM_addr >> 13); }

//---------------------------------------------------------------------------------------
// FME-7

static void INIT_069  (void						 ) { CPU_W (0x8000,  8); CPU_W (0xA000, 0xC0); }
static void DOWN_069  (void						 ) { CPU_W (0x8000,  8); CPU_W (0xA000, 0x00); }
static void MIR_069   (register uint32_t d		 ) { CPU_W (0x8000, 12); CPU_W (0xA000, d &  1); CPU_W (0x8000, 12); }
static void SAV_069   (register uint32_t ROM_addr) { CPU_W (0x8000,  8); CPU_W (0xA000, ROM_addr >> 13 | 0xC0); }
static void PRG_069   (register uint32_t ROM_addr) { CPU_W (0x8000,  9); CPU_W (0xA000, ROM_addr >> 13); }
static void CHR_069   (register uint32_t ROM_addr) { CPU_W (0x8000,  0); CPU_W (0xA000, ROM_addr >> 10); }

//---------------------------------------------------------------------------------------
// VRC7a

//static void TEST_085  (void				) { if (info [5] < 2) info [5] = 0; }
static void INIT_085  (void						 ) { CPU_W (0xE000, 0x80		   ); }
static void DOWN_085  (void						 ) { CPU_W (0xE000, 0x00		   ); }
static void MIR_085   (register uint32_t d		 ) { CPU_W (0xE000, d & 1	   ); CPU_W (0xE018, 0); }
static void CHR_085   (register uint32_t ROM_addr) { CPU_W (0xA000, ROM_addr >> 10); }
#define PRG_085 PRG_025
//static void PRG_085   (register uint32_t ROM_addr) { CPU_W (0x8000, ROM_addr >> 13); }

//---------------------------------------------------------------------------------------
// SUNSOFT-4

static void INIT_068  (void						 ) { CPU_W (0xF000, 0x10 ); }
static void DOWN_068  (void						 ) { CPU_W (0xF000, 0x00	); }
static void MIR_068   (register uint32_t d		 ) { CPU_W (0xE000, d & 1); CPU_W (0xD000, 0); }
static void PRG_068   (register uint32_t ROM_addr) { CPU_W (0xF000, (ROM_addr >> 14 & 15) | 0x10); }
#define CHR_068 CHR_033
//static void CHR_068   (register uint32_t ROM_addr) { CPU_W (0x8000,  ROM_addr >> 11); }

//---------------------------------------------------------------------------------------
// 115-in-1

static void MIR_225   (register uint32_t d		 ) { CPU_W (0x8000 | ((d << 13) & 0x2000), 0); }
static void PRG_225   (register uint32_t ROM_addr) { CPU_W (0x8000 | ((ROM_addr >>  8) & 0x0FC0) | ((ROM_addr >> 6) & 0x4000) | 0x1000, 0); }
static void CHR_225   (register uint32_t ROM_addr) { CPU_W (0x8000 | ((ROM_addr >> 13) & 0x003F) | ((ROM_addr >> 5) & 0x4000)		  , 0); }

//---------------------------------------------------------------------------------------
// AVE NINA-03/06

static void PRG_079   (register uint32_t ROM_addr) { CPU_W (0x5FFF, ROM_addr >> 12 & 8); }
static void CHR_079   (register uint32_t ROM_addr) { CPU_W (0x5FFF, ROM_addr >> 13 & 7); }

//---------------------------------------------------------------------------------------
// Tengen MIMIC-1

static void PRG_206   (register uint32_t ROM_addr) { CPU_W (0x8000,  0x06); CPU_W (0x8001, ROM_addr >> 13); }
#define CHR_206 CHR_004
//static void CHR_206   (register uint32_t ROM_addr) { CPU_W (0x8000,  0x02); CPU_W (0x8001, ROM_addr >> 10); }

//---------------------------------------------------------------------------------------
static void PRG_111   (register uint32_t ROM_addr) { CPU_W (0x5000, ROM_addr >> 15 & 15); }

//---------------------------------------------------------------------------------------
static void PRG_030   (register uint32_t ROM_addr) { CPU_W (0xC000, ROM_addr >> 15 &   31); }
static void MIR_030	  (register uint32_t d		 ) { CPU_W (0xC000, d << 7 & 128); }

//---------------------------------------------------------------------------------------
// NAMCOT-340

static void MIR_210   (register uint32_t d		 ) { CPU_W (0xE000, d << 7);		 }
static void TEST_210  (void						 ) { info.b [8] = 0x20;				 }	// submapper 2
static void PRG_210   (register uint32_t ROM_addr) { CPU_W (0xE000, ROM_addr >> 13); }
static void CHR_210   (register uint32_t ROM_addr) { CPU_W (0x8000, ROM_addr >> 10); }
//#define CHR_210 CHR_019		// test this and save some memory

//---------------------------------------------------------------------------------------
// KONAMI VRC1
static void MIR_075   (register uint32_t d		 ) { CPU_W (0x9000, d & 1		  ); }
static void CHR_075   (register uint32_t ROM_addr) { CPU_W (0x9000, ROM_addr >> 15); CPU_W (0xE000, ROM_addr >> 12); }
#define PRG_075 PRG_025
//static void PRG_075   (register uint32_t ROM_addr) { CPU_W (0x8000, ROM_addr >> 13); }

//---------------------------------------------------------------------------------------
// Big Time Technology Development Group RED-PILL

static void INIT_RED  (void) {

	CPU_R (0x4800);						//	LO 1 screen mirroring mode
	CPU_R (0x5088);						//	enable CHR-FLASH access mode, enable CHR-RAM, set RAM bank 0
	for (register int i = 0; i < 128; i ++)
		PPU_W (i, i | 128);				//	write data to corresponding address in CHR-RAM

}

static void TEST_RED (void) {

	// read the PRG flash chip parameters
	CPU_W (0xD555, 0xAA);
	CPU_W (0xAAAA, 0x55);
	CPU_W (0xD555, 0x90);	chill (40);						//	3 byte software ID entry sequence
	register int tmp0 = CPU_R (0x8000);						//	read software ID
	register int tmp1 = CPU_R (0x8001);						//	get device ID
	CPU_W (0x8000, 0xF0);	chill (40);						//	exit the software ID mode
	if (tmp0 == 0xBF && tmp1 >= 0xb5 && tmp1 <= 0xb7) {

		v.PRG_ROM_size	=  1 << (tmp1 + 17 - 0xb5);
		info.b [4]		=  v.PRG_ROM_size >> 14;				//	16KB PRG units
		v.PRG_bank_mask	= (v.PRG_ROM_size - v.PRG_mask - 1) | b31;
		v.mode |= 1;

	}
	// read the CHR flash chip parameters
	PPU_R (0x2005);	PPU_W (0x1555, 0xAA);
	PPU_R (0x2002);	PPU_W (0x1AAA, 0x55);
	PPU_R (0x2005);	PPU_W (0x1555, 0x90);	chill (40);		//	3 byte software ID entry sequence
	tmp0 = PPU_R (0x1000);									//	read software ID
	tmp1 = PPU_R (0x1001);									//	get device ID
	PPU_W (0x1000, 0xF0);	chill (40);						//	exit the software ID mode
	if (tmp0 == 0xBF && tmp1 >= 0xb5 && tmp1 <= 0xb7) {

		v.CHR_ROM_size	=  1 << (tmp1 + 17 - 0xb5);
		info.b [5]		=  v.CHR_ROM_size >> 13;				//	8KB PRG units
		v.CHR_bank_mask	= (v.CHR_ROM_size - v.CHR_mask - 1) | b31;
		v.mode |= 2;

	}
}

//	enable CHR-RAM, set NT-HI bank to 0
static void MIR_RED   (register uint32_t d		 ) {	CPU_R ((d		 <<  7 & 0x80) | 0x4800); }
static void PRG_RED   (register uint32_t ROM_addr) {	CPU_R ((ROM_addr >> 15 & 0x0F) | 0x4800); }
static void CHR_RED   (register uint32_t ROM_addr) {	PPU_R ((ROM_addr >> 12 & 0x7F) | 0x2000); }

//---------------------------------------------------------------------------------------
// mapper 268

static void INIT_268 (void) {

	bs_helper = b31;
	for (register int i = 0; i < 65536; i ++)	CPU_W (0x5001, 0x80);	//	make the mapper wait to trick its copy protection
	if (v.base == 0x6004)						map.SAV_base = 0;		//	disable any SRAM tests
	else										CPU_W (0xA001, 0x80);	//	enable any SRAM on cart
	register int a = v.base;
	CPU_W (a	 , 0x40);
	CPU_W (a + 1 , 0x80);
	CPU_W (a + 2 , 0x70);
	CPU_W (a + 3 , 0x00);
	CPU_W (0x8000, 0x26);
	CPU_W (0x8001,  255);		//	set the last bank

}

static void INIT_268a (void) {

	for (register int i = 0; i < 65536; i ++)	CPU_W (0x5001, 0x80);
	register int a = v.base;
	CPU_W (a	 , 0x00);
	CPU_W (a + 1 , 0x60);
	CPU_W (a + 2 , 0x70);
	CPU_W (a + 3 , 0x00);
//	CPU_W (0x8000, 0x26);
//	CPU_W (0x8001,  255);
	CPU_W (0xA001, 0x80);

}

// in order to not falsely detect MMC3 carts as SMD133s
static void PRG_268 (register uint32_t b) {

	// bankswitch A17 - A24 as if it were A13 - A20
	register uint32_t a = v.base;
	CPU_W (a	, 0x40 + (b >> 15 & 0x30) + (b >> 13 & 0x07));	// 01ji 0edc
	CPU_W (a + 1, 0x80 + (b >> 15 & 0x0C) + (b >> 12 & 0x10));	// 100f hg00

}

// this abstract is needed because some games don't wana rip properly thru the SMD133 bs regs
static void PRG_268a (register uint32_t b) {

	// bankswitch A13 - A20
	CPU_W (0x8000, 0x26);
	CPU_W (0x8001, b >> 13);	//	bankswitch A13 - A20

	// bankswitch A21 - A24
	register uint32_t a = v.base;
	CPU_W (a	, 0x00 + (b >> 19 & 0x30));	// 00ji 0000
	CPU_W (a + 1, 0x60 + (b >> 19 & 0x0C));	// 0110 hg00

}

static void CHR_268 (register uint32_t b) {

	CPU_W (0x8000, 0x20);
	CPU_W (0x8001, b >> 10);

}

// test for mapper 268
static int TEST_SMD133 (register int a) {

	v.base = a;
	INIT_268 ();
	for (register int i = 0; i < 0x200000; i += 0x2000) {

		PRG_268 (i);
		for (register int j = 0; j < PRG_KEYS; j ++)

			if (i) {

				if (report_txt [j] != CPU_R (0x8000 + PRG_keys [0][j]))		return 1;

			} else	report_txt [j]  = CPU_R (0x8000 + PRG_keys [0][j]);

	}
	return 0;

}

static int MIR_268 (register uint32_t d) {

	if (TEST_SMD133 (0x5004)) {

		// these registers will swap out the CHR-RAM loader for the actual game
		for (register int i = 0; i < 16; i ++)		report_txt [i] =  CPU_R (0xfff0 + i);
		CPU_W (0x5000, 0);	CPU_W (0x5100, 0);
		for (register int i = 0; i < 16; i ++)	if (report_txt [i] != CPU_R (0xfff0 + i)) {

			// a really big hack to advance q in main to next slot
			asm ("	ldr		r0, [r7, #20]	\n\t");
			asm ("	adds	r0, #1			\n\t");
			asm ("	str		r0, [r7, #20]	\n\t");
			break;

		}
		return 1;

	}
	if (TEST_SMD133 (0x6004))	return 1;
	return 0;

}

static void TEST_268 () {

	register t_h_override * r	=	& info.r;
	register MAPinfo *		m	=	& map;

	// fix the PRG size
	r -> PRG_CHR_ROM_size_MSB	  = r -> PRG_ROM_size >> 4;
	r -> PRG_ROM_size			<<= 4;

	// setup the new init and bs abstracts
	m -> INIT_ = INIT_268a;
	m -> PRG_B = PRG_268a;

	// set the NES 2.0 flag
	r -> flags_7 |= 8;
	if (v.base == 0x5004) {

		r -> submapper = 0x11;

	} else {

		memcpy (& m -> text [7], "COOLBOY ", 8);
		r -> submapper = 0x01;

	}
}

//---------------------------------------------------------------------------------------
// NES 240 (hybrid MMC3)

static void INIT_M25	() { chr_src = CPU_R; CPU_W (0x5000, 0); CPU_W (0x5100, 0); CPU_W (0xa001, 0xb0); }
static void PRG_M25		(register uint32_t ROM_addr) { CPU_W (0x8000,	0x26); CPU_W (0x8001, ROM_addr >> 13 ^ 32); }
static int MIR_M25		(register uint32_t d) { return 0; }

//---------------------------------------------------------------------------------------
// mapper 234
const unsigned char maxi_bs_tbl [16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
										0x08, 0x0c, 0x0a, 0x0b, 0x09, 0x0e, 0x0d, 0x0d};

static void CHR_234 (register uint32_t r) {

	// get to the root bank and pulse the reset line
	CPU_R (0x0000);		chill (1200000);	//	1/60th of a second to be sure A0 cap is drained
	CPU_R (0xffff);		chill (	120000);	//	shorter values here may work

	// calculate the inner bankswitch value
	register int b = (CPU_R (0xFF80 + maxi_bs_tbl [r >> 15]) & 0x40)? (r >> 12 & 14) + (r >> 15 & 1) : r >> 13 & 3;

	// set the inner bankswitch register
	CPU_R (0xFFE8 + b);

}

static void PRG_234 (register uint32_t ROM_addr) {

	// test for bankswitch address range
	if (ROM_addr == 0x7f80) {

		// PRG bank 0 registers
		CPU_R (0x0000);
		chill (1200000);	//	1/60th of a second to be sure A0 cap is drained
		return;

	}
	if (ROM_addr < 0x08000) {

		v.PRG_bank_mask	= 0x7ffff | b31;
		return;

	}
	v.PRG_bank_mask		= 0x78000 | b31;
	CHR_234 (ROM_addr);

}

//---------------------------------------------------------------------------------------
// ASIC mapper type descriptor table

const MAPinfo ASIC_types [] = {

	//	map_no  init SAV_base SAV_msb 	PRG_msb PRG_bank_size PRG_ofs	CHR_msb CHR_bank_size CHR_ofs				INIT_xxx  DOWN_xxx  MIR_xXx  SAV_xXx  PRG_xXx  CHR_xXx  TEST_xxx
	{        1,   11,  0x6000,     14,	     18,           14,      2,	     16,           12,      0,	 0,  0,  0, INIT_001, DOWN_001, {MIR_001}, SAV_001, PRG_001, CHR_001, TEST_001, "NINTENDO MMC1   " },	//	MMC1 MUST be first test!!!
	{       25,    0,  0x6000,     12,       17,           13,      4,       18,           10,      0,	 0,  0,  0, INIT_025,  NUL_xxx, {MIR_025}, NUL_xXx, PRG_025, CHR_025,  NUL_xxx, "KONAMI VRC4b    " },
	{       12,    0,  0x6000,     12,       20,           13,      4,       17,           11,      0,   0,  0,128, INIT_268, DOWN_004, {MIR_268}, NUL_xXx, PRG_268, CHR_268, TEST_268, "SMD133 MINDKIDS " },
	{        4,    0,  0x6000,     12,       17,           13,      4,       17,           13,      4,   0,  0,128, INIT_M25, DOWN_004, {MIR_M25}, NUL_xXx, PRG_M25, PRG_004,  NUL_xxx, "NES 240         " },
	{        4, 0xB0,  0x7000,     12,       18,           13,      4,       17,           10,      4,  64,'1',' ', INIT_004, DOWN_004, {MIR_004}, NUL_xXx, PRG_004, CHR_004, TEST_004, "NINTENDO MMC3   " },	//	due to SMB2J not implementing the PPU A12 flip bit properly ($8000.D7), we use PPU address $1000 as the CHR_ofs
	{      118, 0xB0,  0x6000,     12,       18,           13,      4,       17,           10,      4, 158,'A','S', INIT_004, DOWN_004, {MIR_118}, NUL_xXx, PRG_004, CHR_004, TEST_004, "NINTENDO TxSROM " },
	{        5,   13,  0x6000,     15,       19,           13,      4,       19,           13,      0,	 0,  0,  0, INIT_005, DOWN_005, {MIR_005}, SAV_005, PRG_005, CHR_005,  NUL_xxx, "NINTENDO MMC5   " },
	{       18,    0,  0x6000,     12,       18,           13,      4,       17,           10,      0,	 0,  0,  0, INIT_018, DOWN_018, {MIR_018}, NUL_xXx, PRG_018, CHR_018,  NUL_xxx, "JALECO SS88006  " },
	{       19,    0,  0x6000,     12,       18,           13,      5,       17,           10,      0,	 0,  0,  0, INIT_019, DOWN_019, {MIR_019}, NUL_xXx, PRG_019, CHR_019,  NUL_xxx, "NAMCOT-163      " },
	{       24,    0,  0x6000,     12,       17,           13,     	6,       17,           10,      0,	 0,  0,  0, INIT_024, DOWN_024, {MIR_024}, NUL_xXx, PRG_024, CHR_024,  NUL_xxx, "KONAMI VRC6a    " },
	{       10,   14,  0x6000,     12,       17,           13,      5,       16,           12,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {MIR_010}, NUL_xXx, PRG_010, CHR_010, TEST_010, "NINTENDO MMC4   " },
	{       33,    0,  0x6000,     12,       18,           13,      4,       18,           11,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {MIR_033}, NUL_xXx, PRG_033, CHR_033,  NUL_xxx, "TAITO TC0190    " },
	{       69,    0,  0x6000,     18,       18,           13,      4,       17,           10,      0,	 0,  0,  0, INIT_069, DOWN_069, {MIR_069}, SAV_069, PRG_069, CHR_069,  NUL_xxx, "SUNSOFT FME-7   " },
	{       85,    0,  0x6000,     12,       18,           13,      4,       17,           10,      0,	 0,  0,  0, INIT_085, DOWN_085, {MIR_085}, NUL_xXx, PRG_085, CHR_085,  NUL_xxx, "KONAMI VRC7a    " },
	{       68,    0,  0x6000,     12,       17,           14,      2,       17,           11,      0,	 0,  0,  0, INIT_068, DOWN_068, {MIR_068}, NUL_xXx, PRG_068, CHR_068,  NUL_xxx, "SUNSOFT-4       " },	//	due to a conflict with the VRC7 mirroring test, this test comes after VRC7.
	{        0,    0,       0,      0,       18,           15,      1,       18,           12,      1,	 0,  0,  0, INIT_RED,  NUL_xxx, {MIR_RED}, NUL_xXx, PRG_RED, CHR_RED, TEST_RED, "NES-RED-PILL    " },
	{      225,    0,       0,      0,       20,           14,      2,       19,           13,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {MIR_225}, NUL_xXx, PRG_225, CHR_225,  NUL_xxx, "115-in-1        " },
	{      210,    0,       0,      0,       18,           13,      4,       17,           10,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {MIR_210}, NUL_xXx, PRG_210, CHR_210, TEST_210, "NAMCOT-340      " },
	{       75,    0,       0,      0,       16,           13,      4,       16,           12,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {MIR_075}, NUL_xXx, PRG_075, CHR_075,  NUL_xxx, "KONAMI VRC1     " },
	{        7,    0,       0,      0,       18,           15,      1,        0,           13,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {MIR_007}, NUL_xXx, PRG_007, NUL_xXx,  NUL_xxx, "ANROM           " },

	// special section for ASIC mappers with no mirroring control
	{      111,    0,       0,      0,       18,           15,      1,       14,           13,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {NUL_xXx}, NUL_xXx, PRG_111, NUL_xXx,  NUL_xxx, "Membler's GTROM " },
	{       30,    0,       0,      0,       18,           14,      2,       14,           13,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {MIR_030}, NUL_xXx, PRG_030, NUL_xXx,  NUL_xxx, "UNROM-512 FLASH " },
	{      206,    0,       0,      0,       16,           13,      4,       15,           10,      4,	 0,	 0,	 0,  NUL_xxx,  NUL_xxx, {NUL_xXx}, NUL_xXx, PRG_206, CHR_206,  NUL_xxx, "TENGEN MIMIC-1  " },

	// special section for bus conflict mappers with registers outside the ROM area
	{       79,    0,       0,      0,       15,           15,      1,       15,           13,      0,	 0,  0,  0,  NUL_xxx,  NUL_xxx, {NUL_xXx}, NUL_xXx, PRG_079, CHR_079,  NUL_xxx, "AVE NINA-03/06  " }};

#define		ASIC_cnt		(sizeof (ASIC_types) / sizeof (MAPinfo))
#define		MMC5_SLOT		6					//	it's a hack!  Be careful!!
#define		ASIC_NO_MIRR	4
#define		NINA_SLOT		(ASIC_cnt - 1)

//---------------------------------------------------------------------------------------------------------
//***************************************************************************************
//*************** file io access methods ************************************************
//***************************************************************************************

void scoretbl_R (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (buf.byte		, &score		, sizeof (score			)); }
void root_tbl_R (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (buf.byte		, &root_table	, sizeof (root_table	)); }
void data_tbl_R (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (buf.byte		, &data_table	, sizeof (data_table	)); }
/*
void push_debug (register int m) {

	register int i = debug.Buf [255];
	if (i < 255) {

		debug.Buf [255] ++;
		debug.Buf [i] = m;

	}
}
void debug_R	(register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (buf.byte		, &debug		, 512); }
void PRG__tbl_R (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (buf.byte		, &PRG_bank_tbl	, sizeof (PRG_bank_tbl	)); }
void CHR__tbl_R (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (buf.byte		, &CHR_bank_tbl	, sizeof (CHR_bank_tbl	)); }

void errorlog_R	(register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (buf.byte		, &error_info	, sizeof (error_info	)); }

void root_tbl_W (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (&root_table	, buf.byte		, sizeof (root_table	)); }
void data_tbl_W (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (&data_table	, buf.byte		, sizeof (data_table	)); }
void PRG__tbl_W (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (&PRG_bank_tbl	, buf.byte		, sizeof (PRG_bank_tbl	)); }
void CHR__tbl_W (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (&CHR_bank_tbl	, buf.byte		, sizeof (CHR_bank_tbl	)); }
void outparam_R (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (buf.byte		, &v			, sizeof (v				)); }
void outparam_W (register ptr_all_types buf, register uint32_t file_ofs) { if (!file_ofs) memcpy (&v			, buf.byte		, sizeof (v				)); }

void mediaset_R (register ptr_all_types buf, register uint32_t file_ofs) { ptr_krnl -> media_change_handler (); if (! file_ofs)	memcpy (buf.byte, & mediaset_txt, sizeof (mediaset_txt)); }
void mediaset_W (register ptr_all_types buf, register uint32_t file_ofs) { ptr_krnl -> media_change_handler (); }
*/
void settings_R (register ptr_all_types buf, register uint32_t file_ofs) { if (file_ofs < 2) memcpy (buf.byte, & SETTINGS_BIN -> byte [file_ofs << 9], 512); }
void settings_W (register ptr_all_types buf, register uint32_t file_ofs) {

	if (file_ofs < 2)				memcpy (& debug.buf [file_ofs << 9], buf.byte, 512);
	files_dirty						= DIRTY_TIMEOUT;
	settings_dirty					= 1;

}

typedef struct {

	uint32_t		erase_bmp [4];	//	128 slots in 4 x 1 KB sectors
	uint32_t		acu;			//	accumulator
	uint8_t			dest;			//	OVERRIDE.BIN slot pointer
	uint8_t			byte_cnt;		//	byte counter
	uint8_t			acu_cnt;		//	accumulator size
	uint8_t			mode;			//	mode (0: unknown;  1: raw;  2: hex digit;  3: hex whitespace;  4: wait for CRLF;  5: wait in CRLF)

} t_wb;

t_wb			_wb;

void OVERRIDE_erase () {

	// unlock FPEC
	BOOT_BLOCK -> unlock_fpec ();

	// erase 4KB (8 sectors)
	BOOT_BLOCK -> page_erase_handler (0x40022000, (t_settings *) & OVERRIDE_BIN -> byte [0x0000]);
	BOOT_BLOCK -> page_erase_handler (0x40022000, (t_settings *) & OVERRIDE_BIN -> byte [0x0400]);
	BOOT_BLOCK -> page_erase_handler (0x40022000, (t_settings *) & OVERRIDE_BIN -> byte [0x0800]);
	BOOT_BLOCK -> page_erase_handler (0x40022000, (t_settings *) & OVERRIDE_BIN -> byte [0x0C00]);

}

void override_R (register ptr_all_types buf, register uint32_t file_ofs) { if (file_ofs < 8) memcpy (buf.byte, & OVERRIDE_BIN -> half [file_ofs << 8], 512); }
void override_W (register ptr_all_types buf, register uint32_t file_ofs) {

	if (file_ofs < 8) {

		// check if the sector has not been erased yet
		files_dirty = DIRTY_TIMEOUT;
		if (! (override_erase >> file_ofs & 1)) {

			override_erase = 255;
			OVERRIDE_erase ();

		}
		// clear the erase flag
		override_erase &= ~ (1 << file_ofs);
		register uint32_t k = file_ofs << 8;
		for (register uint32_t j = 0; j < 256; j ++, k ++)

			// copy new data into flash
			BOOT_BLOCK -> flash_halfword_handler (0x40022000, (t_settings *) & OVERRIDE_BIN -> half [k], buf.half [j]);

	}
}

void bcmtable_R (register ptr_all_types buf, register uint32_t file_ofs) { if (file_ofs < 2) memcpy (buf.byte, & BCMTABLE_BIN [file_ofs << 4], 512); }
void bcmtable_W (register ptr_all_types buf, register uint32_t file_ofs) {

	if (file_ofs < 2) {

		// check if the sector has not been erased yet
		files_dirty = DIRTY_TIMEOUT;
		if (! (bcmtable_erase >> file_ofs & 1)) {

			// unlock FPEC
			BOOT_BLOCK -> unlock_fpec ();

			// erase 1KB (2 sectors)
			BOOT_BLOCK -> page_erase_handler (0x40022000, (t_settings *) & BCMTABLE_BIN [0]);
			bcmtable_erase = 3;

		}
		// clear the erase flag
		bcmtable_erase &= ~ (1 << file_ofs);
		register uint32_t k = file_ofs << 8 & 0x0100;
		for (register uint32_t j = 0; j < 256; j ++, k ++)

			// copy new data into flash
			BOOT_BLOCK -> flash_halfword_handler (0x40022000, (t_settings *) & BCMTABLE_BIN [0].half [k], buf.half [j]);

	}
}

void crlf_8 () {

	dest_pos [0]	 = 13;
	dest_pos [1]	 = 10;
	dest_pos		+=  2;

}

void pad_8 (register int cnt) {

	do {

		dest_pos [0]	 = ' ';
		dest_pos		++;

	} while (-- cnt);
}

void hex_8 (register int cnt, register int size) {

	do {

		pad_8 (2);
		register int c = size;
		do {

			register int u	 =	src_pos [-- c];
			register int t	 =	u >> 4;		u &= 15;
			dest_pos [0]	 =	(t > 9)?	t + '7'	:	t + '0';
			dest_pos [1]	 =	(u > 9)?	u + '7'	:	u + '0';
			dest_pos		+=	2;

		} while (c);
		src_pos += size;

	} while (-- cnt);
}

void override_txt_R (register ptr_all_types buf, register uint32_t file_ofs) {

	// assign the destination pointer
	dest_pos = & buf.byte [0];

	// reset the source pointer if the first sector is requested
	if (! file_ofs) {

		seq_ctr = 0;
		src_pos = & OVERRIDE_BIN -> byte [0];

	}
	for (register int i = 0; i < 16; i ++, seq_ctr --) {

		switch (seq_ctr) {

		default:	//	check for the iNES signature
					switch (src_pos [3]) {

					// test for ASCII char
					case ' ' ... '~':	memcpy (dest_pos, src_pos, 32);
										src_pos  += 32;
										dest_pos += 32;
										continue;

					}
					//	CR+LF, 3 ASCII, 5 hex-8
					seq_ctr = 4;
					crlf_8	( );	//	 2
					hex_8	(1, 4);	//	10
					dest_pos [- 10] = '$';
					hex_8	(4, 1);	//	16
					pad_8	(4);	//	 4
					continue;

		case 3:		//	8 hex-8
					hex_8	(8, 1);	//	32
					continue;

		case 2:		//	5 spaces, 4 hex-16, 3 spaces
					pad_8	(4);	//	 4
					hex_8	(4, 2);	//	24
					pad_8	(4);	//	 4
					continue;

		case 1:		//	6 hex-8, CR+LF
					hex_8	(4, 1);	//	16
					pad_8	(4);	//	 4
					hex_8	(2, 1);	//	 8
					pad_8	(2);	//	 2
					crlf_8	( );	//	 2
					src_pos += 2;

		}
	}
}

void override_txt_W (register ptr_all_types buf, register uint32_t file_ofs) {

	// use a register pointer to be more efficient
	register t_wb *		wb	=	& _wb;
	register int		bc	=	wb -> byte_cnt;

	// reset the destination pointer if the first sector is requested
	files_dirty = DIRTY_TIMEOUT;
	if (! file_ofs) {

		wb -> mode	= 5;
		wb -> dest	= 0;

	}
	for (register int cnt = 0; cnt < 512 && wb -> dest < (OVERRIDE_SIZE / 32); cnt ++) {

		register int t = buf.byte [cnt];
		switch (wb -> mode) {

		// wait for end of line (CR or LF)
		case 4:		switch (t) {

//					case 13:
					case 10:	wb -> mode = 5;

					} continue;

		// wait for new line
		case 5:		switch (t) {

					case 10:
					case 13:	continue;

					}

		// unknown mode
		case 0:		bc	= 0;
					if (t == '$') {

						wb -> mode		= 3;
						continue;

					}
					wb -> mode = 1;

		// raw mode
		case 1:		switch (t) {

					// blank-pad the rest of the line
					case 10:	do debug.buf [bc ++] = ' ';
								while (bc < 30);
								wb -> mode = 5;
								goto CR_LF;

					// handle pure ascii data up to 30 chars
					default:	debug.buf [bc ++] = t;
								if (bc >= 30)		goto CR_LF2;

					case 13:;

					} continue;

		// hex whitespace mode
		case 3:		switch (t) {

					// do nothing while in whitespace
					case ' ':	case ',':	case ';':	case   9:	case  10:	case  13:			continue;

					}
					// reset the hex stuff
					wb -> acu_cnt	= 0;
					wb -> acu		= 0;
					wb -> mode		= 2;

		// hex digit mode
		case 2:		switch (t) {

					case 'a' ... 'f':	t ^= 32;	//	shift the caps
					case 'A' ... 'F':	t -=  7;	//	delete the alpha offset
					case '0' ... '9':	wb -> acu = (wb -> acu << 4) + t - '0';	wb -> acu_cnt ++;	continue;

					// found the end of the number
					case ' ':	case ',':	case ';':	case   9:	case  10:	case  13:

						// write in the number
						wb -> acu_cnt >>= 1;		//	divide nibble count by 2 to get bytes, ignore partials
						memcpy (& debug.buf [bc], & wb -> acu, wb -> acu_cnt);
						bc	+= wb -> acu_cnt;

						// check the byte counter
						wb -> mode = 3;
						if (bc >= 30) {

CR_LF2:						wb -> mode		 =  4;
CR_LF:						debug.Buf [15]	 =	0x0a0d;

							// calculate the OVERRIDE.BIN offset;
							register int ofs = wb -> dest ++;	//	increment the destination index slot
							register int sh  = ofs & 31;

							// check the erase_bmp
							if ((wb -> erase_bmp [ofs >> 5] >> sh & 1) == 0) {

								memset (wb -> erase_bmp, 255, sizeof (_wb.erase_bmp));
								OVERRIDE_erase ();

							}
							// clear the erase bit
							wb -> erase_bmp [ofs >> 5] &= ~ (1 << sh);

							// write 32 bytes to OVERRIDE.BIN
							for (register int j = 0; j < 16; j++) {

								BOOT_BLOCK -> flash_halfword_handler (0x40022000, (t_settings *) & OVERRIDE_BIN -> data [ofs].half [j], debug.Buf [j]);

							}
						}
					}
		}
	}
	wb -> byte_cnt = bc;

}

//---------------------------------------------------------------------------------------------------------

void readme_txt_R (register ptr_all_types buf, register uint32_t file_ofs) {

	if (!file_ofs)
		memcpy (buf.byte, &readme_txt, sizeof (readme_txt));

}

void report_txt_R (register ptr_all_types buf, register uint32_t file_ofs) {

	if (!file_ofs)
		memcpy (buf.byte, &report_txt, 512);

}

void resetnes_txt_R (register ptr_all_types buf, register uint32_t file_ofs) {

	ptr_krnl -> media_change_handler ();
	if (!file_ofs)
		memcpy (buf.byte, &report_txt, 512);

}

void DEMO_NES_R (register ptr_all_types buf, register uint32_t file_ofs) {

	if (file_ofs < 128)
		memcpy (buf.byte, (char *) DEMO_BASE + (file_ofs << 9), 512);

}

void ROM_NES_R (register ptr_all_types buf, register uint32_t file_ofs) {

	old_PRG_bank |= bs_helper;		//	mostly for mapper 268
	file_ofs <<= 9;
	unsigned int cnt = 512 / 16;
	do {

		register unsigned int ROM_addr = file_ofs;
		file_ofs += 16;

		if (ROM_addr < 16) {

			// NES header
			memcpy (buf.byte, &info, 16);
			buf.byte += 16;
			continue;

		}
		ROM_addr -= 16;
		if (ROM_addr < v.PRG_ROM_size) {

			// PRG-ROM handler
			if ((ROM_addr ^ old_PRG_bank) & v.PRG_bank_mask) {

				old_PRG_bank = ROM_addr;
				old_CHR_bank |= b31;
				map.PRG_B (ROM_addr);
			}
			ROM_addr &= v.PRG_mask;
			ROM_addr += info.r.PRG_bank_ofs;
			register unsigned int i = 16;
			do {

				buf.byte [0] = CPU_R (ROM_addr++);
				buf.byte++;

			} while (--i);
			continue;

		}
		ROM_addr -= v.PRG_ROM_size;
		if (ROM_addr < v.CHR_ROM_size) {

			// CHR-ROM handler
			if ((ROM_addr ^ old_CHR_bank) & v.CHR_bank_mask) {

				old_CHR_bank = ROM_addr;
				old_PRG_bank |= b31;
				map.CHR_B (ROM_addr);
			}
			ROM_addr &= v.CHR_mask;
			ROM_addr += info.r.CHR_bank_ofs;
			register unsigned int i = 16;
			do {

				buf.byte [0] = chr_src (ROM_addr ++); //PPU_R (ROM_addr++);
				buf.byte++;

			} while (--i);
		}

	} while (--cnt);
}
/*
void CHR_SAV_R (register ptr_all_types buf, register uint32_t ROM_addr) {

	ROM_addr <<= 9;
	if ((ROM_addr ^ old_CHR_bank) & v.CHR_bank_mask) {

		old_CHR_bank = ROM_addr;
		old_PRG_bank |= b31;
		map.CHR_B (ROM_addr);

	}
	ROM_addr &= v.CHR_mask;
	ROM_addr += v.CHR_ofs;
	register unsigned int i = 0;
	do buf.byte [i] = PPU_R (ROM_addr + i);
	while (++i < 512);

}

void CHR_SAV_W (register ptr_all_types buf, register uint32_t ROM_addr) {

	ROM_addr <<= 9;
	if ((ROM_addr ^ old_CHR_bank) & v.CHR_bank_mask) {

		old_SAV_bank = ROM_addr;
		old_PRG_bank |= b31;
		map.CHR_B (ROM_addr);

	}
	ROM_addr &= v.CHR_mask;
	ROM_addr += v.CHR_ofs;
	register unsigned int i = 0;
	do PPU_W (ROM_addr + i, buf.byte [i]);
	while (++i < 512);

}
*/
void ROM_SAV_R (register ptr_all_types buf, register uint32_t ROM_addr) {

	ROM_addr <<= 9;
	if ((ROM_addr ^ old_SAV_bank) & v.SAV_bank_mask) {

		old_SAV_bank = ROM_addr;
		map.SAV_B (ROM_addr);

	}
	ROM_addr &= v.SAV_mask;
	ROM_addr += map.SAV_base;
	register unsigned int i = 0;
	do buf.byte [i] = sram_src_R (ROM_addr + i);
	while (++i < 512);

}

void ROM_SAV_W (register ptr_all_types buf, register uint32_t ROM_addr) {

	ROM_addr <<= 9;
	if ((ROM_addr ^ old_SAV_bank) & v.SAV_bank_mask) {

		old_SAV_bank = ROM_addr;
		map.SAV_B (ROM_addr);

	}
	ROM_addr += v.SAV_ofs;
	ROM_addr &= v.SAV_mask;
	ROM_addr += map.SAV_base;
	register unsigned int i = 0;
	do sram_src_W (ROM_addr + i, buf.byte [i]);
	while (++i < 512);

}

static void PRG_W_byte (register unsigned int addr, register unsigned int data) {

	if ((addr ^ old_PRG_bank) & v.PRG_bank_mask) {

		old_PRG_bank = addr;
//		old_CHR_bank |= b31;
		map.PRG_B (addr);

	}
	addr &= v.PRG_mask;
	addr += info.r.PRG_bank_ofs;

	// phase 1: data & address setup
	GPIOB->ODR = (data << 8) | (addr & 255);			// data and low address
	GPIOA->ODR = ((addr >> 8) & 0x7F)					// high address
				| ROMSEL | PPU_RD | PPU_WR				// control signals
				| 0x2400;								// pull-up for IRQ & NT.A10

	// phase 2: assert clock line(s) and enable output register
	GPIOB->CRH = 0x33333333;							// enable data bus if addr in $0000 - $7FFF
	GPIOA->BSRR = ((addr << 8) & 0x800000) | M2;		// enable M2 and /ROMSEL if A15 = 1

	//phase 3: negate clock line and disable output register
	GPIOA->BSRR = M2 << 16 | ROMSEL;					// clear M2 and set ROMSEL
	GPIOB->CRH = 0x44444444;							// disable data bus

}

static void PRG_BIN_W (register ptr_all_types buf, register uint32_t file_ofs) {

	// check the power situation
	check_power_OK ();

	// precalculations
	register uint32_t ROM_addr		= file_ofs << 9;
	register uint32_t CPU_addr		= info.r.PRG_bank_ofs + (ROM_addr & v.PRG_mask);
	register uint32_t flash__sector	= file_ofs >> 3;
	register uint32_t offset_sector	= file_ofs &  7;

	// check if sector has been erased
	if (!(PRG_erase [flash__sector] >> offset_sector & 1)) {					//	check if sector has already been erased

		// invoke the sector erase command
		PRG_W_byte (0x5555, 0xAA);	PRG_W_byte (0x2AAA, 0x55);	PRG_W_byte (0x5555	, 0x80);
		PRG_W_byte (0x5555, 0xAA);	PRG_W_byte (0x2AAA, 0x55);	PRG_W_byte (ROM_addr, 0x30);

		// wait for command to complete
		while ((CPU_R (CPU_addr) ^ CPU_R (CPU_addr)) & 0x40);					//	wait using toggle bit
		PRG_erase [flash__sector] = 0xFF;										//	set 8 bits to indicate 4KB is now erased & ready to program

	}
	//	do the byte programming
	PRG_erase [flash__sector] &= ~(1 << offset_sector);							//	clear the erased bit
	for (register int a = 0; a < 512; a ++, CPU_addr ++) {

		// invoke the programming sequence
		PRG_W_byte (0x5555		, 0xAA			);
		PRG_W_byte (0x2AAA		, 0x55			);
		PRG_W_byte (0x5555		, 0xA0			);
		PRG_W_byte (ROM_addr + a, buf.byte [a]	);

		// wait for command to complete
		while ((CPU_R (CPU_addr) ^ CPU_R (CPU_addr)) & 0x40);					//	wait using toggle bit

	}
}

static void CHR_W_byte (register int addr, register int data) {

	if ((addr ^ old_CHR_bank) & v.CHR_bank_mask) {

		old_CHR_bank = addr;
//		old_PRG_bank |= b31;
		map.CHR_B (addr);

	}
	addr &= v.CHR_mask;
	addr += info.r.CHR_bank_ofs;

	// phase 1: data & address setup
	GPIOB->ODR = (data << 8) | (addr & 255);			// data and low address
	GPIOA->ODR = ((~addr >> 7) & 0x40)					// inverted A13
				| ((addr >> 8) & 0x3F)					// high address
				| ROMSEL | CPU_RW | PPU_RD | PPU_WR		// control signals
				| 0x2400;  								// pull-up for IRQ & NT.A10

	// phase 2: assert clock line and enable output register
	GPIOB->CRH = 0x33333333;							// enable data bus
	asm ("nop\n nop\n");								// fixup to match PRG section timing
	GPIOA->BRR = PPU_WR;

	//phase 3: negate clock line
	GPIOA->BSRR = PPU_WR;

	//phase 4: cool down
	GPIOB->CRH = 0x44444444;

}

static void CHR_BIN_W (register ptr_all_types buf, register uint32_t file_ofs) {

	// check the power situation
	check_power_OK ();

	// precalculations
	register uint32_t ROM_addr		= file_ofs << 9;
	register uint32_t PPU_addr		= info.r.CHR_bank_ofs + (ROM_addr & v.CHR_mask);
	register uint32_t flash__sector	= file_ofs >> 3;
	register uint32_t offset_sector	= file_ofs &  7;

	// check if sector has been erased
	if (!(CHR_erase [flash__sector] >> offset_sector & 1)) {					//	check if sector has already been erased

		// invoke the sector erase command
		CHR_W_byte (0x5555, 0xAA);	CHR_W_byte (0x2AAA, 0x55);	CHR_W_byte (0x5555	, 0x80);
		CHR_W_byte (0x5555, 0xAA);	CHR_W_byte (0x2AAA, 0x55);	CHR_W_byte (ROM_addr, 0x30);

		// wait for command to complete
		while ((PPU_R (PPU_addr) ^ PPU_R (PPU_addr)) & 0x40);					//	wait using toggle bit
		CHR_erase [flash__sector] = 0xFF;										//	set 8 bits to indicate 4KB is now erased & ready to program

	}
	//	do the byte programming
	CHR_erase [flash__sector] &= ~(1 << offset_sector);							//	clear the erased bit
	for (register int a = 0; a < 512; a ++, PPU_addr ++) {

		// invoke the programming sequence
		CHR_W_byte (0x5555		, 0xAA			);
		CHR_W_byte (0x2AAA		, 0x55			);
		CHR_W_byte (0x5555		, 0xA0			);
		CHR_W_byte (ROM_addr + a, buf.byte [a]	);

		// wait for command to complete
		while ((PPU_R (PPU_addr) ^ PPU_R (PPU_addr)) & 0x40);					//	wait using toggle bit

	}
}

//***************************************************************************************
//*************** Misc. *****************************************************************
//***************************************************************************************

uint32_t PPU_mirroring_test (register int i) {
	register uint32_t t = 0xC5;		//	test pattern - zero terminating
	register uint32_t v = t << 13;	//	result buffer
	if (ASIC_types [i].reserved_3 & 128)	return ASIC_types [i].MIR_.test (0);		//	mirroring test override
	do {
		ASIC_types [i].MIR_.mirr (t);
		GPIOA->BSRR = 0x540028;		//	A11 = A13 = 1; A10 = A12 = /A13 = 0
		chill (40);
		v ^= GPIOA->IDR & NT_A10; v >>= 1;
	} while (t >>= 1);
	return (!v);
}

uint32_t PPU_mirroring_test_2 (register uint32_t A10, register uint32_t A11) {
	A10 <<= 4;
	A11 <<= 4;
	register uint32_t j = 0;
	for (register int i = 0; i < 8; i++) {
		GPIOA->BSRR = 0x40000 >> (A10 & 16) | 0x80000 >> (A11 & 16) | 0x500020; // sets A13 = 1 & A12,/A13 = 0
		A10 >>= 1;
		A11 >>= 1;
		chill (40);
		j >>= 1; j |= GPIOA->IDR & NT_A10;
	}
	return (j >> 6);
}

void scan_CHR (void) {

	info_5_mask = 0xff;
//	CHR_bank_mask = 0;
	register uint32_t bank_size	= 1 << map.CHR_bank_size;
	v.CHR_mask					= bank_size - 1;
	register uint32_t bank_bits	= map.CHR_bank_size - 10;
	info.r.CHR_bank_ofs			= map.CHR_ofs << map.CHR_bank_size;
	info.r.CHR_bank_bits		= map.CHR_bank_size;
	for (register uint32_t i = 1 << map.CHR_msb; i > bank_size; i >>= 1)
		for (register uint32_t j = 0; j < i; j += bank_size) {
			map.CHR_B (i+j); for (register uint32_t k = 0; k < CHR_KEYS; k++)	  bank_sig [k]  = chr_src (info.r.CHR_bank_ofs + CHR_keys [bank_bits][k]);
			map.CHR_B (  j); for (register uint32_t k = 0; k < CHR_KEYS; k++) if (bank_sig [k] != chr_src (info.r.CHR_bank_ofs + CHR_keys [bank_bits][k])) {
				info.b [5] = i >> 12;
				v.CHR_bank_mask = (i + i - bank_size) | b31;
				goto RAM_TEST;
			}
		}

	info_5_mask = 0;
RAM_TEST:;

	// early exit for CHR-bypass types
	if (chr_src == CPU_R)	return;

	//	try to read complimented data back
	char i = PPU_R (0x0000);						//	read in data from CHR-RAM test addresses
	char j = PPU_R (0x0001);						//	2 addresses to remedy open-bus capacitance
			 PPU_W (0x0000, ~i);					//	write out compliment
			 PPU_W (0x0001,  i);					//	write true data to next address
	char k = PPU_R (0x0000) ^ 0xFF;
	if 	(k == i) switch (map.map_no) {

	//	bypass the test for RAM on RED PILL + MMC5 (experimental)
	case 0:
	case 5:		break;

	// disable the CHR ROM by setting it to 0
	default:	info.r.CHR_RAM_size		= 38 - __CLZ (info.b [5]);
				info.b [5]				= 0;

	}
	PPU_W (0x0000, i);								//	restore test bytes
	PPU_W (0x0001, j);								//	these have no effect on CHR-ROM
}

void scan_PRG (void) {

	info_4_mask					= 0xFF;
	register uint32_t bank_size	= 1 << map.PRG_bank_size;
	v.PRG_mask					= bank_size - 1;
	register uint32_t bank_bits	= map.PRG_bank_size - 13;
	info.r.PRG_bank_bits		= map.PRG_bank_size;
	info.r.PRG_bank_ofs			= map.PRG_ofs << map.PRG_bank_size;
	for (register uint32_t i = 1 << map.PRG_msb; i > bank_size; i >>= 1)
		for (register uint32_t j = 0; j < i; j += bank_size) {
			map.PRG_B (i+j); for (register uint32_t k = 0; k < PRG_KEYS; k++)	 bank_sig [k]  = CPU_R (info.r.PRG_bank_ofs + PRG_keys [bank_bits][k]);
			map.PRG_B (  j); for (register uint32_t k = 0; k < PRG_KEYS; k++) if (bank_sig [k] != CPU_R (info.r.PRG_bank_ofs + PRG_keys [bank_bits][k])) {
				info.b [4] = i >> 13;
				v.PRG_bank_mask = (i + i - bank_size) | b31;
				return;
			}
		}

	// default for no switchable banks found
	info.b [4]		= 2;
	info_4_mask		= 0;
//	PRG_bank_mask	= 0;
}

void scan_SAV (void) {

	// read in SRAM data covering range of test, save it at the end of report_txt
	if (!map.SAV_base) return;								//	no more testing if mapper says so
	int end	= 2 << map.SAV_msb;
	register int j = 511;
	for (register int i = -1; end & i; i <<= 1, j--) {
		map.SAV_B (i);
		int a = (~i & 0x1FFF) ^ map.SAV_base;
		report_txt [j] = CPU_R (a);
	}

	// generate mask of implemented address lines
	v.SRAM_err		= 0;		//	SRAM write-back error count
	register int b	= 0;		//	battery-backed test count
	register int c	= 0;		//	SRAM test count
	int k			= -1;
	j = 511;
	int mask = 0;	//	valid SRAM bits
	for (register int i = -2; end & k; i <<= 1, k <<= 1, j--) {
		int a = (~i & 0x1FFF) ^ map.SAV_base;
		int b = (~k & 0x1FFF) ^ map.SAV_base;
		map.SAV_B (k);		CPU_W (b,  ~report_txt [j]);	//	lower (current) address
		map.SAV_B (i);		CPU_W (a,	report_txt [j]);	//	higher address
		map.SAV_B (k); if ((CPU_R (b) ^ report_txt [j]) == 0xFF) {
			mask |= -k;										//	valid SRAM bits
			c++;											//	total SRAM count
		}
	}

	// test for battery-backed save RAM
	if (!c) return;											//	no more testing if SRAM absent
	if (ch.items.battery_test_enable) {

		// shut down the cart for a moment
		shut_down ();
		chill (ch.items.battery_power_down * (MSEC_const / 2));
		if (ch.items.quick_discharge_enable) discharge ();
		chill (ch.items.battery_power_down * (MSEC_const / 2));
		check_power_OK ();

		// run through the ranges, count the compliment data matches as battery-backed SRAM present in 2^n
		j = 511;
		for (register int i = -1; end & i; i <<= 1, j--) {
			int a = (~i & 0x1FFF) ^ map.SAV_base;
			map.SAV_B (i);
			if ((mask & -i) && ((CPU_R (a) ^ report_txt [j]) == 0xFF))
				b++;											//	battery-backed SRAM count

			CPU_W (a, report_txt [j]);							//	write back original data
			if (CPU_R (a) != report_txt [j])
				v.SRAM_err ++;									// verify original SRAM data written back properly
		}
	}
	else b = c;

	// if a battery presence is detected, set up the save RAM methods
	if (b == c) {

		if (b > 13)	 v.SRAM_size = 1 << b;
		else		 v.SRAM_size = 8192;
		info.b [6] |= 2;										//	set the battery-backed bit

	}
	// update iNES header and other stuff
	v.SAV_bank_mask = (mask & 0xFFFFE000) | b31;			//	SRAM bankswitch mask
	SAV_size = b;											//	total SRAM amount
	c  =  c - b - 6;	c &= -c >> 31 & 15;					//	non-battery backed SRAM amount
	b -= 		  6;	b &= -b >> 31 & 15;					//	battery backed SRAM amount
	info.b [10] = b << 4 | c;								//	.NES 2.0 PRG RAM size byte
	if (b) {

		info.r.PRG_bank_ofs_SRAM	= map.SAV_base;
		info.r.PRG_bank_bits_SRAM	= (b <= 6)? b + 6 : 13;

	}
}

void goto_root () {
	
	for (register int i = 0xffff; i >= 0x8000; i --)
		
		if (! CPU_R (i)) {
			
			CPU_W (i, 0);		//	set mapper to bank 0
			cur_bank = 0;
			return;
			
		}
}

int goto_bank (register uint32_t b) {

	// test if the bank is available
	if (data_table [b]) {

		// is there a root entry in the table for current bank?
		if (! root_table [cur_bank])

			// scan the PRG space for a null byte
			for (register int i = 0xffff; i >= 0x8000; i --)

				if (! CPU_R (i)) {

					root_table [cur_bank] = i;
					break;

				}
	
		// go back to the root bank to use the data lookup table there
		CPU_W (root_table [cur_bank], 0);	//	bankswitch back to root bank
		CPU_W (data_table [b], b);			//	bankswitch to desired bank
		cur_bank = b;
		return 0;

	}
	return -1;
}

// return the NT A10 line
uint32_t MIR_R (register uint32_t addr) {

	GPIOA -> BSRR = 0x00540028;
	chill (40);
	return GPIOA -> IDR >> 13 & 1;

}

void get_bank_score (uint8_t * t2, uint32_t (* BUS_R) (register uint32_t addr), const uint16_t * u, uint16_t ofs) {
//	t2		score byte
//	u		CPU/PPU test address
//	BUS_R	CPU_R or PPU_R

	// create the data value and data valid buffers (1 == not valid)
	register unsigned char * buf	= & debug.buf  [0];
	register		  char * nul	= & report_txt [0];

	// switch to all 256 banks and read a byte at the same address
	for (register unsigned int y = 0; y < 256; y ++) {

		// bankswitch to 256 PRG banks
		if (! (nul [y] = goto_bank (y))) {

			// record the data
			buf [y		] = BUS_R (u [0] + ofs);
			buf [y + 256] = BUS_R (u [1] + ofs);
			buf [y + 512] = BUS_R (u [2] + ofs);
			buf [y + 768] = BUS_R (u [3] + ofs);

		}
	}
	// determine which bits caused any bankswitch behaviour
	register unsigned int t = t2 [0];
	for (register unsigned int i = 128; i; i >>= 1) {

		// iterate 0 to 127
		for (register unsigned int j = 0; j < 128; j ++) {

			register unsigned int x = (- i & j) + j;
			register unsigned int y = x + i;
			if (nul [x] |  nul [y])		continue;

			if (buf [x		] != buf [y		 ])		t |= i;
			if (buf [x + 256] != buf [y + 256])		t |= i;
			if (buf [x + 512] != buf [y + 512])		t |= i;
			if (buf [x + 768] != buf [y + 768])		t |= i;

		}
	}
	t2 [0] = t;

}

//***************************************************************************************
//*************** File system init + cart present detect + cart type detect *************
//***************************************************************************************

void init (t_kernel_functions * ptr) {

	// initialize the application RAM
	memset ((char *) RAM_BASE, 0, RAM_USED);
	memcpy (&info, &NES_header_init, 32);
	src_pos				= & OVERRIDE_BIN -> byte [0];
	chr_src				= PPU_R;
	sram_src_R			= CPU_R;
	sram_src_W			= (void *) CPU_W;
	map					= map_init;
	CPU_W_enable		= 0x88888888;			// 0x33333333 for ON
	CPU_W_disable		= 0x88888888;			// 0x44444444 for HI-Z
	v.PRG_bank_mask		= b31;
	v.CHR_bank_mask		= b31;
	v.SAV_ofs			= 0x0000;
	v.PRG_mask			= 0x7FFF;
	v.CHR_mask			= 0x1FFF;
	v.SAV_mask			= 0x1FFF;
	ptr_krnl			= ptr;					// get the kernel function pointer

	// import the settings
	settings_items = SETTINGS_ITEMS - (memcmp (& info, (int *) DEMO_NES, 4) != 0);
	memcpy (& debug, SETTINGS_BIN, SAVE_SIZE);
	import_settings ((t_settings *) & debug, 1);

	// determine the size of SETTINGS.TXT by scanning until null
	register int q = 0;
	do if (SETTINGS_BIN -> byte [q] == 0)	break;
	while (++ q < (SAVE_SIZE - 1));

	// create the settings file
	ptr_krnl -> make_file_handler ("SETTINGSTXT&", q, settings_R, settings_W);

	// determine the size of the bus conflict mapper table
	for (simple_cnt = 0; (BCMTABLE_BIN [simple_cnt].byte [1][0] != 0) && (simple_cnt < 32); simple_cnt ++);

	// create the BCMTABLE.BIN file
	if (ch.items.BCMTABLE_BIN_on	)	ptr_krnl -> make_file_handler ("BCMTABLEBIN&", BCMTABLE_SIZE, bcmtable_R, bcmtable_W);

	// create a report file
	if (ch.items.REPORT_TXT_enable	)	ptr_krnl -> make_file_handler ("REPORT  TXT ", 256, report_txt_R,	null_io);
	if (ch.items.RESETNES_TXT_on	)	ptr_krnl -> make_file_handler ("DETECT  TXT ", 256, resetnes_txt_R, null_io);

	// create the OVERRIDE.TXT file
	if (ch.items.OVERRIDE_TXT_on) {

		// calculate the size of OVERRIDE.TXT
		register int s = 0;
		register int i = 0;
		do switch (OVERRIDE_BIN -> name [i].text [3]) {

		default:			s += 96;
		case ' ' ... '~':	s += 32;

		} while ((++ i) < (OVERRIDE_SIZE / 32));

		// create the override file
		ptr_krnl -> make_file_handler ("OVERRIDETXT&", s, override_txt_R, override_txt_W);

	}
	// create the debug files
	if (ch.items.debug_enable) {

//		ptr_krnl -> make_file_handler ("PRG_TBL BIN ", sizeof (PRG_bank_tbl	), PRG__tbl_R, null_io		);
//		ptr_krnl -> make_file_handler ("CHR_TBL BIN ", sizeof (CHR_bank_tbl	), CHR__tbl_R, null_io		);
//		ptr_krnl -> make_file_handler ("OUTPARAMBIN ", sizeof (v			), outparam_R, null_io		);
		ptr_krnl -> make_file_handler ("BCMSCOREBIN ", sizeof (score		), scoretbl_R, null_io		);
		ptr_krnl -> make_file_handler ("ROOT_TBLBIN ", sizeof (root_table	), root_tbl_R, null_io		);
		ptr_krnl -> make_file_handler ("DATA_TBLBIN ", sizeof (data_table	), data_tbl_R, null_io		);
//		ptr_krnl -> make_file_handler ("DEBUG   BIN ", 512, debug_R, null_io);
// 		ptr_krnl -> make_file_handler ("ERRORLOGBIN ", sizeof (error_info	), errorlog_R, null_io		);
//		debug.Buf [255] = 0;

	}
	// create a readme file
	if (ch.items.README_TXT_enable)

		ptr_krnl -> make_file_handler ("README  TXT ", sizeof (readme_txt) - 1, readme_txt_R, null_io);

	// turn on the cycle count register
	CoreDebug -> DEMCR |= 0x01000000;
	DWT -> CTRL |= 1;

	// check for the NES demo
	if  (ch.items.DEMO_NES_enable)

		ptr_krnl -> make_file_handler ("DEMO    NES ", 40976, DEMO_NES_R, null_io);

	// -------------------------------------------------------------------------------------------------
	if (ch.items.OVERRIDE_BIN_on || (ch.items.NES_2_0_enable >= 2 && ! ch.items.OVERRIDE_TXT_on))

		ptr_krnl -> make_file_handler ("OVERRIDEBIN&", OVERRIDE_SIZE, override_R, override_W);

	// check if mapper override mode is enabled
	if (ch.items.NES_2_0_enable >= 2) {
NO_CART:;
		// go to the mapper index
		unsigned int i = 0;
		ch.items.override_mode_index += ch.items.NES_2_0_enable - 1;
		if (find_map_header (& i, ch.items.override_mode_index)) {

			// check the map override for errors
			i = check_map_abstract (& i);
			if (i == 0)			{ setup_override ();		goto MAKE_NES; }

		}
		// normalize the error code
		if (i > 10 /*(sizeof (check_err_codes) / 32)*/)		i = 0;

		// print the error type to a file
		memset (report_txt, ' ', 512);
		register unsigned int * rp = (unsigned int *) & report_txt; rp [0] = 0x2e525245; report_txt [5] = i + '0';	// memcpy (report_txt, check_err_codes [i], 32);
		ptr_krnl -> make_file_handler ("ROM     NES ", 16, ROM_NES_R, null_io);
		return;

	}

// --------------------------------------------------------------------------------------
	// detect a cart by trying to read a ROM address twice with pull resistors programmed to opposite levels
	check_power_OK ();
	GPIOB->CRH = 0x88888888;				// set the outputs to pull mode
	pull = ~CPU_R (0xFFFF);					// read the data & compliment it
	if (pull == CPU_R (0xFFFF)) {			// see if the bus pulls toward the compliment
DEMO:
		ch.items.NES_2_0_enable = 2;
		goto NO_CART;

	}

// ASIC mapper class tests ---------------------------------------------------------------

	// bus conflicts test
    for (uint32_t i = 0; i<16; i++) {
    	uint8_t d = CPU_R (i + 0xFFF0);				//	get the data
    	uint8_t w = d ^ 0xFF;
    	GPIOB->ODR = w << 8;
    	chill (1000);								//	experimental
    	if (d != CPU_W (i + 0xFFF0, w)) {

    		// no bus conflicts, so test ASIC types first
    		CPU_W_disable = 0x44444444;
    		CPU_W_enable  = 0x33333333;				//	outputs are enabled
    		if (! ch.items.ASIC_map_testing_on)		goto ASIC_SKIP;

    		// process the ASIC mapper list
    		CPU_W (0x8000, 128);					//	JUST for reseting the MMC1!
    		for (q = 0; q < ASIC_cnt - ASIC_NO_MIRR; q ++)
    			if (PPU_mirroring_test (q)) {		//	short-circuit ASIC mapper type identification test
ASIC_INIT:			map = ASIC_types [q];			//	load descriptor for detected type
 	  	  			map.INIT_ ();					//	enables SRAM, sets PRG/CHR banking modes
    				scan_PRG  ();					//	info [ 4]
    				scan_CHR  ();					//	info [ 5]
    				scan_SAV  ();					//	info [10]
ASIC_SETUP_2:		info.b [6] |= map.map_no << 4;	//	low nibble
    				info.b [7] |= map.map_no & 0xF0;//	high nibble
    				map.TEST_ ();					//	run any mapper-specific tests
    				ASIC_types [q].MIR_.mirr (1);	//	set horizontal mirroring as default
    				goto DONE;
    			}

    		// ----------------------------------------------------------------------------------
    		// try and find a GTROM
    		unsigned char tmp [2];
    		map = ASIC_types [q];
    		CPU_W (0x5000, 	 32);	PPU_W (0x2000, 0x55);				//	clear NT bank bit $5000 = 0
    		CPU_W (0x5000, 	  0);	PPU_W (0x2000, 0xAA);				//	set NT bank bit $5000 = 32
    		CPU_W (0x5000, 	 32);	tmp [0] = CPU_R (0x2000);			//	clear NT bank bit $5000 = 0
    		CPU_W (0x5000, 	  0);	tmp [1] = CPU_R (0x2000);			//	set NT bank bit $5000 = 32
    		if (tmp [0] == 0x55 && tmp [1] == 0xAA)	{					//	confirm signature is read back

    			// 3 byte software ID entry sequence
    			CPU_W (0xD555, 0xAA);
    			CPU_W (0xAAAA, 0x55);
    			CPU_W (0xD555, 0x90);
    			chill (40);    											//	wait for t_IDA

    			// read software ID and device ID
    			tmp [0] = CPU_R (0x8000);
    			tmp [1] = CPU_R (0x8001);
    			if (tmp [0] == 0xBF && tmp [1] >= 0xb5 && tmp [1] <= 0xb7) {

    				// set up the parameters
    				unsigned int u			= 65536 << (tmp [1] - 0xB5);//	0xB5: 128KB;  0xB6: 256KB;  0xB7: 512KB
    	    		unsigned int bank_size	= 1 << map.PRG_bank_size;
        			v.PRG_ROM_size			= u + u;
    	    		v.PRG_mask				= bank_size - 1;
    	    		info.r.PRG_bank_ofs		= map.PRG_ofs << map.PRG_bank_size;
    	    		info.r.PRG_bank_bits	= map.PRG_bank_size;
   					v.PRG_bank_mask			= (v.PRG_ROM_size - bank_size) | b31;
   					info.b [ 4]				= u >> 13;
    				info.b [ 5]				= 0;						//	CHR-RAM not present
    				info.b [ 6]				= 0xFA;						//	set lo mapper bits, set battery bit, 4-screen
    				info.b [ 7]				= 0x60;						//	set hi mapper bits
    				info.b [10]				= (tmp [1] - 0xAA) << 4;	//	set the PRG-EEPROM shift count
    				info.b [11]				= 9;						//	set the CHR-RAM field to 32KB

    				// exit the software ID mode
    	    		CPU_W (0x8000, 0xF0);								//	software ID exit
        			chill (40);    										//	wait for t_IDA

        			// create the flash file handler
        			v.mode |= 1;
    				goto ASIC_SETUP_2;

    			}
    		}
    		q ++;

    		// ----------------------------------------------------------------------------------
    		// try and find a UNROM-512 in self-flashing mode
    		map = ASIC_types [q];

    		// verify CHR-RAM present
    		tmp [0] = ~PPU_R (0x0000);
    		tmp [1] = ~PPU_R (0x1000);
    		PPU_W (0x0000, tmp [0]);
    		PPU_W (0x1000, tmp [1]);
    		if (tmp [0] == PPU_R (0x0000) && tmp [1] == PPU_R (0x1000)) {

    			// 3 byte software ID entry sequence
    			CPU_W (0xC000, 1); CPU_W (0x9555, 0xAA);
    			CPU_W (0xC000, 0); CPU_W (0xAAAA, 0x55);
    			CPU_W (0xC000, 1); CPU_W (0x9555, 0x90);
    			chill (40);    											//	wait for t_IDA

    			// read software ID and device ID
    			CPU_W (0xC000, 0);
    			tmp [0] = CPU_R (0x8000);
    			tmp [1] = CPU_R (0x8001);
    			if (tmp [0] == 0xBF && tmp [1] >= 0xB5 && tmp [1] <= 0xB7) {

    				// set up the parameters
    				unsigned int u			= 65536 << (tmp [1] - 0xB5);//	0xB5: 128KB;  0xB6: 256KB;  0xB7: 512KB
    	    		unsigned int bank_size	= 1 << map.PRG_bank_size;
        			v.PRG_ROM_size			= u + u;
    	    		v.PRG_mask				= bank_size - 1;
    	    		info.r.PRG_bank_ofs		= map.PRG_ofs << map.PRG_bank_size;
    	    		info.r.PRG_bank_bits	= map.PRG_bank_size;
   					v.PRG_bank_mask			= (v.PRG_ROM_size - bank_size) | b31;
   					info.b [ 4]				= u >> 13;
    				info.b [ 5]				= 0;						//	CHR-RAM not present
    				info.b [ 6]				= 0xE2;
    				info.b [ 7]				= 0x10;						//	set hi mapper bits
    				info.b [10]				= (tmp [1] - 0xAA) << 4;	//	set the PRG-EEPROM shift count

        			// determine CHR-RAM size
        			CPU_W (0xC000, 0x00); PPU_W (0x0000, 0x55);
        			CPU_W (0xC000, 0x60); PPU_W (0x0000, 0xAA);
        			CPU_W (0xC000, 0x00); tmp [0] = PPU_R (0x0000);
        			CPU_W (0xC000, 0x60); tmp [1] = PPU_R (0x0000);
            		if (tmp [0] == 0x55 && tmp [1] == 0xAA)							info.b [11] = 9;
            		else															info.b [11] = 7;

        			// verify NTRAM present or 1-screen mirroring
            		tmp [0] = ~PPU_R (0x2000);
            		tmp [1] = ~PPU_R (0x3000);
            		PPU_W (0x2000, tmp [0]);
            		PPU_W (0x3000, tmp [1]);
            		if (tmp [0] == PPU_R (0x2000) && tmp [1] == PPU_R (0x3000))		info.b [6] = 0xEB;
            		else if (PPU_mirroring_test (q))								info.b [6] = 0xEA;

    				// this counteracts the mirroring test later on
    				if ((info.b [6] & 8) && PPU_mirroring_test_2 (0x35, 0x6C) == 0x35) info.b [6] --;

    				// exit the software ID mode
    	    		CPU_W (0x8000, 0xF0);								//	software ID exit
        			chill (40);    										//	wait for t_IDA

        			// create the flash file handler
        			v.mode |= 1;
    				goto ASIC_SETUP_2;

    			}
    		}
    		q ++;

    		// ----------------------------------------------------------------------------------
    		// try and find a MIMIC-1
    		map = ASIC_types [q];
    		scan_PRG ();
    		scan_CHR ();
    		if ((info.b [4] & info_4_mask) | (info.b [5] & info_5_mask)) goto ASIC_SETUP_2;
    		q ++;

    		// ----------------------------------------------------------------------------------
    		// if we get here, it means we didn't identify an ASIC type
    		// so now we setup data_table and root_table so that all entries are OK
ASIC_SKIP:
			goto NO_ASIC;

    	}
    }

// A special, second test for MMC5 ------------------------------------------------------

    //	Bus conflicts detected on an MMC5 board ?  WTF?!?!
	CPU_W (0x5205, 0x55);
	CPU_W (0x5206, 0xAA);					//	test the multiplier
	if (CPU_R (0x5205) == 0x72 && CPU_R (0x5206) == 0x38) { q = MMC5_SLOT; goto ASIC_INIT; }


// ******************************************************************************************
// ***** Super - Duper Testing  Section *****************************************************
// ******************************************************************************************

    // set up the bussing situation for bus conflict types
	CPU_W_disable = 0x44444444;
	CPU_W_enable  = 0x33333333 + (0x11111111 * ch.items.bus_disable);	//	ROM space outputs are enabled by default

NO_ASIC:

	// alternate bus conflict tests for mappers with bankswitching registers outside the ROM address range
	if (ch.items.alt_bus_con_test_on) {

		// MAXI 15 test
		CHR_234 (0x0001);	if (PPU_mirroring_test_2 (0x35, 0x6C) != 0x35)	goto MAXI_SKIP;
		CHR_234 (0x8001);	if (PPU_mirroring_test_2 (0x35, 0x6C) != 0x6c)	goto MAXI_SKIP;

		// setup MAXI 15
		v.PRG_ROM_size		= 0x80000;
		v.CHR_ROM_size		= 0x80000;
		v.PRG_mask			= 0x07fff;
		v.CHR_mask			= 0x01fff;
		v.PRG_bank_mask		= 0x78000 | b31;
		v.CHR_bank_mask		= 0x7e000 | b31;
		map.PRG_B			= PRG_234;
		map.CHR_B			= CHR_234;
		info.r.PRG_bank_ofs	= 0x08000;
		info.r.CHR_bank_ofs	= 0x00000;
		info.b [4]			=	 0x20;
		info.b [5]			=	 0x40;
		info.b [6]			=	 0xa0;
		info.b [7]			=	 0xe0;
		memcpy (& map.text, "AVE MAXI 15 in 1", 16);
		goto DONE;
MAXI_SKIP:

		// NINA-03/06 test
		q = NINA_SLOT;
		map = ASIC_types [q];
		scan_PRG ();
		scan_CHR ();
		if ((info.b [4] & info_4_mask) | (info.b [5] & info_5_mask)) goto ASIC_SETUP_2;
		q ++;

	}
	// simple_mapper class tests ------------------------------------------------------------

	// skip the bus conflict test section completely
	map = map_init;
	if (! ch.items.conflict_testing_on) goto ALTERNATE;

    //	CNROM diodes test
    //	a number of NROM & CNROM Famicom games have these diodes coming off the HC161 that connect to
    //	PPU address lines, with the intent to spoil your fun if you're trying to copy the game in 1986.
    //	(to be written at a time when everything else is done and working perfectly)


    // Bus conflict banking table setup
    //	Nearly all bus conflict type mappers have ONE 8-bit register allocated @ CPU $8000-$FFFF.  The
    //	advantage to this is the ability to probe bits and get a response for multiple types of mappers
    //	simultaneously.


    // navigate to bank 0
	cur_bank = 1;
	goto_root ();
	if (cur_bank) {
		
		// search for 0x00 failed, so file a report and move on with life.
		strcpy (report_txt, "unsupported PRG ROM");
		goto DEMO;
	
	}
	// try and build an 8-bit banking table
	int ctr = 256;
	for (int i = 0xC000; i < 0x14000; i++) {	//	start the scan at $C000 because it helps out the UNROM games

		uint8_t d = CPU_R (i | 0x8000);
		if (!data_table [d]) {

			data_table [d] = i | 0x8000;
			ctr --;
			if (! ctr) break;

		}
	}
	// allocate the root bank entry
//	root_table [0]	= data_table [0];
//	map				= map_init;
	map.INIT_		= goto_root;		// ensure the root bank is set every time the cart starts

// conduct the bit-banging tests -------------------------------------------------------------------

	// iterate the PRG banks 0 - 3
	for (register int l = 0; l < 4; l ++)

		// iterate the PRG key address
		for (register int k = 0; k < PRG_KEYS; k += 4)

			get_bank_score (& score [l + 4], CPU_R, & PRG_keys [0][k], (l << 13) + 0x8000);

	// iterate the CHR banks 0 - 7
	for (register int l = 0; l < 8; l ++)

		// iterate the CHR key address
		for (register int k = 0; k < (CHR_KEYS - 4); k += 4)

			get_bank_score (& score [l + 8], PPU_R, & CHR_keys [0][k], (l << 10));

	// get the mirroring control score
	get_bank_score (& score [3], MIR_R, & PRG_keys [0][0], 0);

// identify mapper type based on score results --------------------------------------------------

	int old_size = 0;
	for (register uint32_t i = 0; i < simple_cnt; i++) {

		if (simple_types [i].byte [0][3] != score [3]) {		//	mirroring bits must match exactly

			// error 1
//			error_info [i].byte [0] = 1;
//			error_info [i].byte [1] = score [3];
//			error_info [i].byte [2] = simple_types [i].byte [0][3];
			goto FAIL;

		}
		for (register uint32_t j = 4; j < 16; j++)
			if ((~simple_types [i].byte [0][j] &   score [j])		//	if any score bits are not part of the mapper mask
			||  ( simple_types [i].byte [0][j] && !score [j])) {	//	if the score is zero where the mapper mask has any bits set

				// error 2
//				error_info [i].byte [0] = 2;
//				error_info [i].byte [1] = score [j];
//				error_info [i].byte [2] = simple_types [i].byte [0][j];
				goto FAIL;

			}
		//	calculate final PRG mask from score
		uint8_t k = simple_types [i].byte [0][1];
		uint8_t g = k & 3;
		uint8_t l = g | 4;
		uint8_t r, cs, ps;
		switch (k & 12) {
case 12:	ps = 15; r = score [4] | score [    5] | score [6] | score [7];	break;
case  4:	ps = 14; r = score [l] | score [l + 1];							break;
default:	ps = 13; r = score [l];
		}

		// calculate actual PRG mask value
		uint8_t p = 0;
		uint8_t m = 0;
		int  ctr0 = 0;
		uint8_t id = simple_types [i].byte [0][l];	//	PRG ID mask
		if (id) {
			r = ~r & id;
			int q = 1;
			do {
				if (q & r ) break;				//	LSB clear score bit against set type bit
				if (1 & id) { p++; m |= q; }	//	mask bit COUNTS
				q  <<= 1;
				id >>= 1;
			} while (id);
			if (!p) {

				// error 3
//				error_info [i].byte [0] = 3;
//				error_info [i].byte [1] = id;
//				error_info [i].byte [2] = ps;
//				error_info [i].byte [3] = p;
//				error_info [i].byte [4] = m;
//				error_info [i].byte [5] = q;
//				error_info [i].byte [6] = k;
//				error_info [i].byte [7] = r;
				goto FAIL;						//	at least ONE of the mapper bits has to be implemented!

			}
			// build the PRG bankswitching table based on mask
			memset (&report_txt [256], 0, 256);
			for (int n = 0; n < 256; n++)
				if (!report_txt [(m & n) + 256] && data_table [n]) {
					report_txt [(m & n) + 256] = 1;
					PRG_bank_tbl [ctr0] = n;
					ctr0++;
					if (ctr0 >= 1 << p) break;
				}
		}

		//	calculate final CHR mask from score
//		uint8_t s = 0xFF;
		uint8_t s = 0;
		k >>= 4;
		if (k > 7) { 								  s  = score [k	   ]; cs = 10; }
else	if (k > 3) { k <<= 1; for (l = 0; l < 2; l++) s |= score [k + l]; cs = 11; }
else	if (k > 1) { k <<= 2; for (l = 0; l < 4; l++) s |= score [k + l]; cs = 12; }
else	if (k    ) { k <<= 3; for (l = 0; l < 8; l++) s |= score [k + l]; cs = 13; }
else			   { k   = 8;						  s  = 			   0; cs = 13; }

		// calculate actual CHR mask value
		uint8_t c = 0;
		uint8_t w = 0;
		int  ctr1 = 0;
		id = simple_types [i].byte [0][k];			//	CHR ID mask

		if (id) {
			s = ~s & id;
			int q = 1;
			do {
				if (q & s ) break;				//	LSB clear score bit against set type bit
				if (1 & id) { c++; w |= q; }	//	mask bit COUNTS
				q  <<= 1;
				id >>= 1;
			} while (id);
			if (!c) {

				// error 4
//				error_info [i].byte [0] = 4;
//				error_info [i].byte [1] = id;
//				error_info [i].byte [2] = ps;
//				error_info [i].byte [3] = cs;
//				error_info [i].byte [4] = ctr0;
//				error_info [i].byte [5] = c;
//				error_info [i].byte [6] = k;
//				error_info [i].byte [7] = s;
				goto FAIL;						//	at least ONE of the mapper bits has to be implemented!

			}
			//	build the CHR bankswitching table based on mask
			memset (&report_txt [256], 0, 256);
			for (int n = 0; n < 256; n++)
				if (!report_txt [(w & n) + 256] && data_table [n]) {
					report_txt [(w & n) + 256] = 1;
					CHR_bank_tbl [ctr1] = n;
					ctr1++;
					if (ctr1 >= 1 << c) break;
				}
		}

		//	if the PRG_SIZE + CHR_SIZE <= OLD_SIZE the mapper is a FAIL
		int PRGsize = ctr0 << ps;
		int CHRsize = ctr1 << cs;
		if (old_size >= PRGsize + CHRsize) {

			// error 5
//			error_info [i].byte  [0] = 5;
//			error_info [i].byte  [1] = id;
//			error_info [i].byte  [2] = ps;
//			error_info [i].byte  [3] = cs;
//			error_info [i].byte  [4] = ctr0;
//			error_info [i].byte  [5] = ctr1;
//			error_info [i].hword [3] = old_size >> 8;
			goto FAIL; 										//	only the largest PRG + CHR results are kept

		}
		old_size = PRGsize + CHRsize;

		//	set the mapper up as a PASS
//		error_info [i].byte [0] = 0;
		memcpy (&map.text, &simple_types [i].byte [1], 16);		//	copy out mapper text
		info.b [6] = simple_types [i].byte [0][0] << 4;				//	low mapper nibble
		info.b [7] = simple_types [i].byte [0][0] & 0xF0;			//	high mapper nibble

		//	calculate PRG stuff
		if (ctr0) {
			info.b [4]			= PRGsize >> 14;				//	16KB PRG units
			v.PRG_bank_mask		= (PRGsize - (1 << ps)) | b31;	//	used for caching current bank
			v.PRG_mask			= (1 << ps) - 1;				//	mask off the low address bits
			info.r.PRG_bank_ofs	= (g + 4) << 13;				//	offset into CPU map
			info.r.PRG_bank_bits= ps;							//	shift right amount for bank table lookup
			v.PRG_B_mask		= (1 << p) - 1;					//	bank table mask
		}
		else
			info.b [4]			= 2;

		//	calculate CHR stuff
		if (k) {
			info.b [5] 			= CHRsize >> 13;
			v.CHR_bank_mask		= (CHRsize - (1 << cs)) | b31;
			v.CHR_mask			= (1 << cs) - 1;
			info.r.CHR_bank_ofs	= (k - 8) << 10;
			info.r.CHR_bank_bits= cs;
			v.CHR_B_mask		= (1 << c) -1;
		}
		else
			info.b [5]		= 0;		//	0: 8K CHR-RAM is present

FAIL:;
	}

	//---------------------------------------------------------------------------------------------------
	// initialization for special bus-conflict mapper types
	switch ((info.b [6] >> 4 & 15) | (info.b [7] & 240)) {

// UNROM-512
case 30:

		// check if any save RAM present
		scan_SAV ();
		if (info.b [6] & 2) {

			// force the mapper to type 2
			info.b [6] &= 0x2f;
			info.b [7] &= 0x0f;

		}
		// check if the 32KB CHR-RAM variant present
		if (map.text [10] == '3')		info.b [11] = 9;		//	set the volatile CHR-RAM size to 32K in NES 2.0 flags
		else							info.b [11] = 7;

		// detect if 4-screen mirroring present
		PPU_W (0x2000,	  0xe1);	PPU_W (0x2400,	  0x96);	PPU_W (0x2800,	  0x4b);	PPU_W (0x2c00,	  0x3c); if ((
		PPU_R (0x2000) == 0xe1) && (PPU_R (0x2400) == 0x96) && (PPU_R (0x2800) == 0x4b) && (PPU_R (0x2c00) == 0x3c)) {

		   map.text [14] =  '4';			info.b [6] |= 9;

		} else if (map.text [14] == 'M')	info.b [6] = (info.b [6] & 254) | 8;

		// this counteracts the mirroring test later on
		if ((info.b [6] & 8) && PPU_mirroring_test_2 (0x35, 0x6C) == 0x35) info.b [6] --;

//	CPROM
case 13:
		info.b [5] = 0;	//	no CHR-ROM on CPROM or UNROM-512
		break;

	}

//---------------------------------------------------------------------------------------------------
ALTERNATE:

	// if default PRG-ROM size found, test for 16KB PRG-ROM
	if ((info.b [4] == 2) && ch.items._16KB_PRG_ROM_test) {

		for (int n = 0; n < PRG_KEYS; n ++)

			if (CPU_R (PRG_keys [1][n] + 0x8000) != CPU_R (PRG_keys [1][n] + 0xC000))	goto NROM_256;

		info.b [4] = 1;

	}

//---------------------------------------------------------------------------------------------------
NROM_256:
DONE:
	//
	v.PRG_ROM_size = info.b [4] << 14;		//	PRG 16KB bank count
	v.CHR_ROM_size = info.b [5] << 13;		//	CHR  8KB bank count

	//	generate a mirroring setting
	if (PPU_mirroring_test_2 (0x35, 0x6C) == 0x35) info.b [6] ++;

//////////////////////////////////////////////////////////////////////////////////////////
//////// Mapper Override Auto-detect tests ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

	// find and test auto-detection sequences in OVERRIDE.BIN
	if (ch.items.override_testing_on) {

		// run list of manual mappers with TEST sequences
		for (unsigned int i = 0; find_map_header (& i, -1); ) {

			if (! check_map_abstract (& i)) {

				if (! manual_bankswitch (v.TEST_override_index, 0)) {

					setup_override ();
					break;

				}
			}
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
MAKE_NES:

	// write out the header in hex for easy override transition
	seq_ctr = 0;
	src_pos = & info.b [0];
	override_txt_R ((ptr_all_types) { (unsigned char *) & report_txt [0] }, 1);

	//	calculate NES_2.0 header stuff or not
	if (!(info.r.flags_7 & 8)) for (register int n = 8; n < 16; n ++)	if (info.b [n])
	{
		if (ch.items.NES_2_0_enable)	info.b [7] |= 8;	//	set the NES 2.0 signature
		else							info.b [n]  = 0;	//	clear the NES 2.0 flags
	}

	// write out the ROM parameters
	memcpy (& report_txt [128], &map.text, 16);
	memset (& report_txt [144], ' ', 12);
	report_txt [156] = '@';
	report_txt [157] = '!';
	register int p = 158;
    p += sprintf (& report_txt [p], "\r\n\r\nMapper  :%13u\r\nPRG-ROM : %12u\r\nCHR-ROM :%13u\r\n",
    		(info.b [6] >> 4) | (info.b [7] & 240) | ((info.r.submapper & 15) << 8),
			v.PRG_ROM_size,
			v.CHR_ROM_size);

    // write out the SRAM info
	memset (& report_txt [p], ' ', 512 - p);
	if (info.b [6] & 2) p += sprintf (& report_txt [p], "SAVE-RAM:%13u", (int) 1 << SAV_size);
//	if (v.SRAM_err	) p += sprintf (& report_txt [p], "SRAM write-back error count: %i\r\n", v.SRAM_err);

	// create the main NES ROM abstracts
	ptr_krnl -> make_file_handler ("ROM     NES ", v.PRG_ROM_size + v.CHR_ROM_size + 16, ROM_NES_R, null_io);

	// create any SRAM abstracts
	if (info.b [6] & 2)	ptr_krnl -> make_file_handler ("ROM     SAV ", v.SRAM_size, ROM_SAV_R, ROM_SAV_W);

	// create any flash abstractions
	if (v.mode & 1) ptr_krnl -> make_file_handler ("PRG     BIN ", v.PRG_ROM_size, null_io, PRG_BIN_W);
	if (v.mode & 2) ptr_krnl -> make_file_handler ("CHR     BIN ", v.CHR_ROM_size, null_io, CHR_BIN_W);

}

//---------------------------------------------------------------------------------------------------------
// not used for application

int main () { return 0; }
