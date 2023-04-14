// Copyright 2019 Big Time Technology Development Group.

#include "usbd_storage_if.h"
#include "stm32f1xx_it.h"

//------------------------------------------------------------------------------------------
// types

typedef struct {

	uint8_t					filename  [8];
	uint8_t					extension [3];
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

typedef void				(* ptr_io_handler			) (uint32_t * buf, uint32_t file_ofs);	// file offset in sectors

typedef struct {

	t_dir_slot				dir_entry;
	ptr_io_handler			read_handler;
	ptr_io_handler			write_handler;

} t_file;

typedef struct {

	uint8_t					company_name				[ 8];
	uint8_t					product_name				[16];
	uint8_t					version						[ 4];

} t_product;

typedef struct {

	ptr_io_handler			read_handler;
	ptr_io_handler			write_handler;
	int						relative_sector;
	uint16_t				next_cluster;
	uint16_t				used_clusters;

} t_disk_slot;

typedef t_file				(* ptr_file					);
typedef	t_dir_slot			(* ptr_dir_slot				);
typedef t_disk_slot			(* ptr_disk_slot			);

static void					blink_PC13					  (void								  );
static void					media_change				  (void								  );
static void					io_null						  (uint32_t *	buf, uint32_t file_ofs);
static void					read_boot_sector			  (uint32_t *	buf, uint32_t file_ofs);
static void					read_root_dir				  (uint32_t *	buf, uint32_t file_ofs);
static void					write_FAT					  (uint32_t *	buf, uint32_t file_ofs);
static void					read_firmware				  (uint32_t *	buf, uint32_t file_ofs);
static void					write_firmware				  (uint32_t *	buf, uint32_t file_ofs);
static void					write_firmware_2			  (uint32_t *	buf, uint32_t file_ofs);
static void					read_FAT					  (uint16_t *	buf, uint32_t file_ofs);
static void					write_root_dir				  (t_dir_slot *	buf, uint32_t file_ofs);
static void					delete_file					  (ptr_dir_slot	ptr					  );
static ptr_dir_slot			create_file					  (ptr_file		ptr					  );
static ptr_dir_slot			make_file					  (char * filename, uint32_t filesize, ptr_io_handler rd_hndlr, ptr_io_handler wr_hndlr);

static int8_t				STORAGE_Init_FS				  (uint8_t lun);
static int8_t				STORAGE_GetCapacity_FS		  (uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t				STORAGE_IsReady_FS			  (uint8_t lun);
static int8_t				STORAGE_IsWriteProtected_FS	  (uint8_t lun);
static int8_t				STORAGE_Read_FS				  (uint8_t lun, void *buf, uint32_t blk_addr, uint32_t blk_len);
static int8_t				STORAGE_Write_FS			  (uint8_t lun, void *buf, uint32_t blk_addr, uint32_t blk_len);
static int8_t				STORAGE_GetMaxLun_FS		  (void);

extern USBD_HandleTypeDef	hUsbDeviceFS;

typedef struct {

	ptr_dir_slot			(* create_file_handler		) (ptr_file ptr);
	ptr_dir_slot			(* make_file_handler		) (char * filename, uint32_t filesize, ptr_io_handler rd_hndlr, ptr_io_handler wr_hndlr);
	void					(* delete_file_handler		) (ptr_dir_slot	ptr);
	void					(* media_change_handler		) (void);

} t_kernel_functions;

typedef struct {

	uint8_t					stuff						[0x09AC];
	void					(* unlock_fpec				) (void);
	uint32_t				(* decrypt					) (uint32_t * buf, uint32_t * flash_addr);
	void					(* page_erase_handler		) (uint32_t reg_base, uint32_t * dest);
	void					(* flash_halfword_handler	) (uint32_t reg_base, uint32_t * dest, uint32_t data);
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
	t_product				product_ID;

} t_firmware_header;

typedef struct {

	t_firmware_header		application_header;
	void					(* initialization_handler	) (const t_kernel_functions * ptr);
	t_systick				systick_handler;

} t_application;

typedef struct {

	uint32_t				payload						[8192];

} t_app_payload;

typedef struct {

	unsigned short			value;
	unsigned short			reload;						// in milliseconds
	unsigned short			cycles;
	unsigned short			mode;

} t_timer;

typedef struct {

	uint32_t				shift_register				[64];
	uint32_t				zero_value;
	uint32_t				flash_position;
	t_timer					timer;

} t_app_RAM;

//------------------------------------------------------------------------------------------
// constants

#define						ROOT_DIR_MAX_ENTRIES		32	//	32 minimum; the rest constitutes a RAM disk
//#define					FAT_MAX_ENTRIES				256	//	2 minimum
#define						BOOT_BLOCK_BASE				0x08000000U
#define						APPLICATION_BASE			0x08008000U
#define						APPLICATION_END				0x0800F000U
#define						APP_RAM_BASE				0x20002800U

#define						BOOT_BLOCK					((t_boot_block  *) BOOT_BLOCK_BASE )
#define						APPLICATION					((t_application *) APPLICATION_BASE)
#define						application					((t_app_payload *) APPLICATION_BASE)
#define						APP_RAM						((t_app_RAM		*) APP_RAM_BASE	   )
#define						FIRST_FREE					disk_decode_table [0].next_cluster
#define						TIMER_DEFAULT				timer_table [3]

#define						MAX_DECODE_SLOTS			34
#define						FREE_SPACE_START_SLOT		32
#define						FREE_CLUSTER				(( FREE_SPACE_START_SLOT - 2) << 11)
#define						MAX_FAT						(((FREE_SPACE_START_SLOT - 2) <<  3) - 1)

#define						STORAGE_LUN_NBR             1
#define						STORAGE_BLK_NBR    	        MAX_DECODE_SLOTS * 2048
#define						STORAGE_BLK_SIZ             0x00200
#define						DISK_DECODE_RESERVED		5

//------------------------------------------------------------------------------------------
// ROM

static const t_timer		timer_table [8] = {

	{ 1,	 833,	 7,		0 },	// timer_block_error
	{ 1,	 833,	 7,		1 },	// timer_block_error
	{ 1,	 625,	 9,		2 },	// timer_CRC_error
	{ 1,	2000,	31,		3 },	// timer_default
	{ 1,	 625,	 9,		4 },	// timer_CRC_error
	{ 1,	 180,	13,		5 },	// timer_kernel_OK
	{ 1,	 625,	 9,		6 },	// timer_CRC_error
	{ 1,	 180,	13,		7 }		// timer_app_OK

};

static const t_disk_slot	disk_decode_init [DISK_DECODE_RESERVED] = {

	{read_boot_sector,	io_null		  ,		   0,	0xFFFF,		 0},
	{read_FAT		 ,	write_FAT	  ,		2048,	0xFFFF,		 0},
	{read_root_dir	 ,	write_root_dir,		4096,	0xFFFF,		 0},
	{io_null		 ,	write_firmware,		6144,	0xFFFF,		64},
	{read_firmware	 ,	io_null		  ,		8192,	0xFFFF,		 2}

};

static const t_disk_slot	disk_decode_null = {

	 io_null		 ,	io_null		  ,		   0,	0xFFFF,		 0

};

static const t_kernel_functions kernel_functions = {

	create_file,
	make_file,
	delete_file,
	media_change

};

static const t_firmware_header kernel_string = {

	0x02028000,
	(STANDARD_INQUIRY_DATA_LEN - 5), {

							"BTTD Grp",
							"BLUE-PILL KERNEL",
							"0.08"//"0.04"//"0.03"
}};

//------------------------------------------------------------------------------------------
// RAM

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
  STORAGE_Init_FS,
  STORAGE_GetCapacity_FS,
  STORAGE_IsReady_FS,
  STORAGE_IsWriteProtected_FS,
  STORAGE_Read_FS,
  STORAGE_Write_FS,
  STORAGE_GetMaxLun_FS,
  (int8_t *) &kernel_string
};

t_disk_slot					disk_decode_table			[MAX_DECODE_SLOTS];
t_dir_slot					root_dir					[ROOT_DIR_MAX_ENTRIES];
//uint16_t					file_allocation_table		[FAT_MAX_ENTRIES];

//------------------------------------------------------------------------------------------
// code
uint32_t		null_buf_write		[8][128];		//	place to store 4KB of temp write data
unsigned char	default_write_slot	[2];
unsigned short	null_buf_size;
//unsigned char	null_buf_dirty;

/*
unsigned char debug_ptr;
#define debug_info null_buf_write [8]
static void read_buf		(register uint32_t * buf, register uint32_t file_ofs)
{ if (9 > file_ofs)			 memcpy (buf, & null_buf_write [file_ofs], 512);}
static void read_null		(register uint32_t * buf, register uint32_t file_ofs) { memset (buf, ' ', 512); }
static void write_null		(register uint32_t * buf, register uint32_t file_ofs) { debug_info [debug_ptr] |= 128<<24; }
*/

static void flush_dirty		(register int disk_slot) {

	for (register int i = 0; i < null_buf_size; i ++)

		disk_decode_table [disk_slot].write_handler (& null_buf_write [i][0], i);

}

// buffer all unknown writes up to 4 KB
static void		write_unknown		(register uint32_t * buf, register uint32_t file_ofs)		{
//debug_info [debug_ptr] |= 64<<24;
	switch (null_buf_size) {

case 0 ... 7:	memcpy (& null_buf_write [null_buf_size], buf, 512);	null_buf_size ++;
				break;

				// flush the dirty sector buffers
case 8:			flush_dirty (default_write_slot [0]);
default:		disk_decode_table [default_write_slot [0]].write_handler (buf, null_buf_size ++);	// send the data directly to the default write handler

	}
}
static void read_root_dir		(register uint32_t	 * buf, register uint32_t file_ofs)		{ if (file_ofs < 2) memcpy (buf, &root_dir [file_ofs << 4], 512);		}
static void write_root_dir		(register t_dir_slot * buf, register uint32_t file_ofs)		{

	if (file_ofs < 2) {

		// scan the root directory for GOUTPU~1 file or a file with a FREE_CLUSTER starting address
		register int change_slot = 0;
		file_ofs <<= 4;
		if (default_write_slot [1]) for (register int i = 0; buf -> filename [i] && (i < 16); i ++) {

			register t_dir_slot * b = & buf [i];

			// ignore deleted or LFN records
			if ((b -> filename [0]	!=	0xE5)
			&&	(b -> attributes	!=	0x0F)) {

				// determine if OVERRIDE.TXT was modified
				if (b -> last_access_date != root_dir [i].last_access_date
				&&  ! memcmp (b -> filename, "OVERRIDETXT", 11))		default_write_slot [1] = (b -> starting_cluster >> 11) + 2;

				// set up the default write handler if a .goutputstream file is detected
				if (! memcmp (b -> filename, "GOUTPU~1   ", 11)) {
//debug_info [debug_ptr] |= 1<<24;
					default_write_slot [0] = default_write_slot [1];	//	switch ON the default write handler
					break;

				}
				// otherwise detect if a file has a new FREE_CLUSTER start address
				register int old_start	=	root_dir [i + file_ofs].starting_cluster;
				if	(	 		null_buf_size							&&	old_start
				&&	(			b -> starting_cluster				!=	old_start				)
				&&	(((			b -> starting_cluster >> 11) + 2)	==	FREE_SPACE_START_SLOT	)
				&&	! memcmp (& b -> filename, & root_dir [i + file_ofs].filename, 11	)) {
//debug_info [debug_ptr] |= 2<<24|old_start;
					change_slot = (old_start >> 11) + 2;
					break;

				}
			}
		}
		// if the buffer is dirty, try to find a file to match it with
		if (change_slot) {

			// get the old file's disk slot index
			if (null_buf_size <= 8)		flush_dirty (change_slot);
			null_buf_size = 0;

			// write-protect the disk slot now since the OS freed up space there
//			disk_decode_table [change_slot] = disk_decode_null;

			// switch off the default write handler
			default_write_slot [0] = 0;

		}
		memcpy (& root_dir [file_ofs], buf, 512);

	}
}

static void io_null				(register uint32_t * buf, register uint32_t file_ofs)		{																		}
static void read_boot_sector	(register uint32_t * buf, register uint32_t file_ofs)		{ if (  ! file_ofs) memcpy (buf, BOOT_BLOCK -> boot_sector, 512); 		}
static void version_txt_R		(register uint32_t * buf, register uint32_t file_ofs)		{ //if (! file_ofs) {

//		unsigned short cr = 0x0A0D;
//		memcpy ((char *) buf +  0, &APPLICATION -> application_header.product_ID.company_name,  8); memcpy ((char *) buf +  8, &cr, 2);
//		memcpy ((char *) buf + 10, &APPLICATION -> application_header.product_ID.product_name, 16); memcpy ((char *) buf + 26, &cr, 2);
//		memcpy ((char *) buf + 28, &APPLICATION -> application_header.product_ID.version	 ,  4);

	asm ("			push	{r4, r5, r6, r7}					\n\t");
	asm ("			movw	r7, 0x8008							\n\t");
	asm ("			movt	r7, 0x0800							\n\t");
	asm ("			ldmia	r7 , {r1, r2, r3, r4, r5, r6, r7}	\n\t");
	asm ("			stmia	r0!, {r1, r2}						\n\t");		//	company name
	asm ("			movw	r1, 0x2020							\n\t");
	asm ("			movt	r1, 0x0A0D							\n\t");
	asm ("			stmia	r0!, {r1, r3, r4, r5, r6}			\n\t");		//	CRLF, product name
	asm ("			stmia	r0!, {r1, r7}						\n\t");		//	CRLF, product ID
	asm ("			pop		{r4, r5, r6, r7}					\n\t");
	asm ("verskip:												\n\t");

//	}
}
/*
static void read_root_dir (register uint32_t * buf, register uint32_t file_ofs) {

	switch (file_ofs) {

case 0 ... 1:		memcpy (buf, &root_dir [file_ofs << 4], 512);
default:			return;

case 2 ... 255:		for (register int i = 0; root_dir [i].filename [0]; i ++) {

						if (disk_decode_table [i].read_handler == io_null) continue;
						if (file_ofs <  root_dir [i].starting_cluster) continue;
						if (file_ofs >= root_dir [i].starting_cluster + ((root_dir [i].file_size + 511) >> 9)) continue;
						disk_decode_table [i].read_handler (buf, file_ofs - root_dir [i].starting_cluster);
						return;

					}
					if (8 > file_ofs) memcpy (buf, &root_dir [file_ofs << 4], 512);
	}
}

static void write_root_dir (register uint32_t * buf, register uint32_t file_ofs) {

	switch (file_ofs) {

case 0 ... 1:		memcpy (&root_dir [file_ofs << 4], buf, 512);
default:			return;

case 2 ... 255:		for (register int i = 0; root_dir [i].filename [0]; i ++) {

						if (disk_decode_table [i].write_handler == io_null) continue;
						if (file_ofs <  root_dir [i].starting_cluster) continue;
						if (file_ofs >= root_dir [i].starting_cluster + ((root_dir [i].file_size + 511) >> 9)) continue;
						disk_decode_table [i].write_handler (buf, file_ofs - root_dir [i].starting_cluster);
						return;

					}
					if (8 > file_ofs) memcpy (&root_dir [file_ofs << 4], buf, 512);
	}
}
*/
static void write_FAT			(register uint32_t * buf, register uint32_t file_ofs) { /*if (  ! file_ofs) memcpy (&file_allocation_table, buf, FAT_MAX_ENTRIES * 2);*/	}
static void read_FAT			(register uint16_t * buf, register uint32_t file_ofs) {

	switch (file_ofs) {

//		case 0:					memcpy (buf, &file_allocation_table	, FAT_MAX_ENTRIES * 2);
		case 0 ... 7:			for (register int i = 0; i < 256; i ++) buf [i] = 0xfff0;
								if (! file_ofs) { buf [0] = 0xFFF8; buf [1] = 0xFFFF; }
								return;

		case 8 ... MAX_FAT:;	int			 next_link		= disk_decode_table [(file_ofs >> 3) + 2].next_cluster;
								register int cluster_cnt	= disk_decode_table [(file_ofs >> 3) + 2].used_clusters;
								register int cluster_ofs	=					 (file_ofs &  7)<< 8;
								register int FAT_addr		=					  file_ofs		<< 8;
								register int i				=									   0;
								do {

									FAT_addr++;
									cluster_ofs++;
									if		(cluster_cnt < cluster_ofs) buf [i] = 0xfff0;//0;
									else if (cluster_cnt > cluster_ofs) buf [i] = FAT_addr	;
									else 								buf [i] = next_link	;

								} while (++i < 256);
	}
}

static void read_firmware		(register uint32_t * buf, register uint32_t file_ofs) {

	switch (file_ofs) {

		case 0:					memcpy (buf, &BOOT_BLOCK -> product_ID				  , 512); return;
		case 1:					memcpy (buf, &BOOT_BLOCK -> authentication_block [472], 256);
	}
};

static void write_firmware (register uint32_t * buf, register uint32_t file_ofs) {

	// check if first sector of file
	if (!file_ofs) {

		// setup continuous load handler
		disk_decode_table [3].write_handler = write_firmware_2;

		// default timer and pointer values
		register t_app_RAM * a = APP_RAM;
		a -> zero_value = 0;
		a -> flash_position = APPLICATION_BASE;
		a -> timer = TIMER_DEFAULT;

		// set the application_systick to an upgrade_error_status_handler
		application_systick = blink_PC13;

		// unlock FPEC
		BOOT_BLOCK -> unlock_fpec ();

	  	// decrypt the first sector to get 'er going
	  	register int i = BOOT_BLOCK -> decrypt (buf, & a -> zero_value);
	  	if (i != 3) {

			a -> timer = timer_table [i];
			disk_decode_table [3].write_handler = io_null;	// lock the firmware handler from further writes

	  	}
  		// reset the CRC register
  		CRC-> CR  = 1;

	}
};

static void write_firmware_2 (register uint32_t * buf, register uint32_t file_ofs) {

	register int i = BOOT_BLOCK -> decrypt (buf, &APP_RAM -> flash_position);
	if (i == 3)

		GPIOC	-> ODR	^= 0x2000;

	else {

		APP_RAM -> timer = timer_table [i];
		disk_decode_table [3].write_handler = io_null;	// lock the firmware handler from further writes

	}
};

static void blink_PC13 (void) {

	register t_app_RAM * a = APP_RAM;
	if (!(-- a -> timer.value)) {

		// reset the timer value
		a -> timer.value = a -> timer.reload;

		// clear or set PC13 according to bit 0 of timer.cycles
		GPIOC -> BSRR = 0x20000000 >> ((a -> timer.cycles & 1) << 4);

		// when the count expires, shit gets real
		if (!(a -> timer.cycles--)) {

			__disable_irq ();
			switch (a -> timer.mode) {

			default:	NVIC_SystemReset ();
			case 5:		BOOT_BLOCK -> flash_kernel_handler (10);	//	10 failed flash retries max.

			}
		}
	}
}

ptr_dir_slot create_file (register ptr_file ptr) {

	// scan the root dir for a free entry; return 0 if none
	register uint32_t dir_index = 0;
	do {
		switch (root_dir [dir_index].filename [0]) {

			case 0x00:
			case 0xE5:	goto FOUND_DIR_ENTRY;
		}
		dir_index++;

	} while (dir_index < 32);
	return 0;									// couldn't find a free directory slot

FOUND_DIR_ENTRY:

	// examine the file size and scan thru the links to determine if enough space; return 0 if not
	for (register int j = 0, i = ptr -> dir_entry.file_size; i > 0; i -= 0x100000) {

		j = disk_decode_table [j].next_cluster;
		if (j == 0xFFFF) return 0;			//	not enough disk space available
		j = (j >> 11) + 2;

	}
	// allocate the file entry
	memcpy (&root_dir [dir_index], &ptr -> dir_entry, 32);

	// set up the decode table
	if (ptr -> dir_entry.file_size) {

		// set the starting cluster address
		root_dir [dir_index].starting_cluster = FIRST_FREE;

		// for the best compatibility with older firmware
		// we switch the default write handler if ROM.SAV or PRG.BIN is present
		// and delete FIRMWARE.BIN + SECURITY.BIN
		if (! memcmp (ptr -> dir_entry.filename , "ROM     SAV", 11)
		||  ! memcmp (ptr -> dir_entry.filename , "PRG     BIN", 11)) {

			default_write_slot [1] = (FIRST_FREE >> 11) + 2;
			root_dir [0].filename [0] = 0xE5;					//	delete FIRMWARE.BIN
			root_dir [1].filename [0] = 0xE5;					//	delete SECURITY.BIN

		}
		// scan again and set the used clusters fields to 2048 except the last cluster
		register int i = ptr -> dir_entry.file_size;
		register int j = 0;
		register int k = 0;
		register t_disk_slot * ddt = & disk_decode_table [0];
		do {
			// calculate the next link address
			j = (ddt -> next_cluster >> 11) + 2;
			ddt = & disk_decode_table [j];

			// set the read & write handlers
			ddt -> read_handler  = ptr -> read_handler;
			ddt -> write_handler = ptr -> write_handler;

			// calculate the relative_offset as absolute_disk_address - file offset
			ddt -> relative_sector = (j << 11) - k;

			// set the used clusters to the max
			ddt -> used_clusters = 2048;

			// update the file pointers
			i -= 0x100000;
			k += 2048;

		} while (i > 0);

		// set last used clusters based on i
		ddt -> used_clusters = (i + 0x1001FF) >> 9;

		// set free_disk_slot_ptr to the last link in the file
		FIRST_FREE = ddt -> next_cluster;

		// set the last link in the file to 0xFFFF
		ddt -> next_cluster = 0xFFFF;
	}
	usbd_media_reset = 1;
	return &root_dir [dir_index];
}

ptr_dir_slot make_file (register char * filename, register uint32_t filesize, register ptr_io_handler rd_hndlr, register ptr_io_handler wr_hndlr) {

	t_file f;
	memcpy (&f, &BOOT_BLOCK -> firmware_dir_entry, 32);	// copy all generic DIR stuff
	memcpy (&f.dir_entry.filename, filename, 12);
	f.dir_entry.file_size = filesize;
	f.read_handler = rd_hndlr;
	f.write_handler = wr_hndlr;
	ptr_dir_slot d = create_file (&f);
	return d;

}
/*
void set_file_size (ptr_dir_slot d, uint32_t filesize) {

	usbd_media_reset = 1;

}

void set_read_handler (ptr_dir_slot d, t_disk_io rd_hndlr) {


	usbd_media_reset = 1;

}

void set_write_handler (ptr_dir_slot d, t_disk_io wr_hndlr) {

	usbd_media_reset = 1;

}
*/
void delete_file (register ptr_dir_slot ptr) {

	// put 0xE5 in first byte of dir_entry
	ptr -> filename [0] = 0xE5;

	// check if file_size = 0
	if (ptr -> file_size) {

		// get starting cluster #

		register int i = ptr -> starting_cluster;
		register int j;
		register t_disk_slot * ddt;
		do {
			// get the disk offset value
			j = (i >> 11) + 2;

			// set all the defaults
			ddt = & disk_decode_table [j];
			ddt -> read_handler  = io_null;
			ddt -> write_handler = io_null;
			ddt -> used_clusters = 0;

			// get next link
			i = ddt -> next_cluster;

		// until cluster terminator
		} while (i != 0xFFFF);

		// link it back into FIRST_FREE
		ddt -> next_cluster = FIRST_FREE;

		// start FIRST_FREE at starting cluster
		FIRST_FREE = ptr -> starting_cluster;
	}
	usbd_media_reset = 1;

}

int8_t STORAGE_Init_FS(register uint8_t lun)
{

	// set up the first 2 FAT words, and initialize the rest of the FAT to 0
//	memset (&file_allocation_table, 0, sizeof (file_allocation_table));
	memset (&root_dir			  , 0, sizeof (root_dir));

	// initialize the root directory
	root_dir [0] = BOOT_BLOCK -> firmware_dir_entry;
	root_dir [1] = BOOT_BLOCK -> security_dir_entry;

	// initialize the disk_decode_table
	for (register int i = 0; i < MAX_DECODE_SLOTS; i++) disk_decode_table [i] = disk_decode_null;
	memcpy (&disk_decode_table, &disk_decode_init, sizeof (disk_decode_init));
	disk_decode_table [FREE_SPACE_START_SLOT].write_handler		= write_unknown;
	disk_decode_table [FREE_SPACE_START_SLOT].relative_sector	= FREE_SPACE_START_SLOT << 11;
	default_write_slot [0]	= 0;		//	dummy write slot
	default_write_slot [1]	= 3;		//	FIRMWARE.BIN slot
	null_buf_size			= 0;

	// link up free slots to free_disk_slot_ptr
	FIRST_FREE = (DISK_DECODE_RESERVED - 2) << 11;
	for (register int i = DISK_DECODE_RESERVED; i < MAX_DECODE_SLOTS - 1; i++) disk_decode_table [i].next_cluster = (i - 1) << 11;

	// check the application file size
	register t_application * a = APPLICATION;
	if (a -> application_header.file_size <= 8192) {

		// reset the CRC accumulator
		CRC -> CR = 1;

		// generate the CRC32 for the application
		for (register int i = 2; i < a -> application_header.file_size; i++)
			CRC -> DR = application -> payload [i];

		// check the CRC32 with the application header
		if (CRC -> DR == a -> application_header.CRC32
	    && !memcmp (a ->application_header.product_ID.company_name, BOOT_BLOCK -> product_ID.company_name,  8)) {

			application_systick = a -> systick_handler;
			a -> initialization_handler (&kernel_functions);
			make_file ("VERSION TXT\a", 36, version_txt_R, io_null);
		}
	}
//	make_file ("WRITEBUFBIN ", 4352, read_buf , io_null);
//	make_file ("BS      TXT ",    1, read_null, write_null);
	return (USBD_OK);
}

int8_t STORAGE_GetCapacity_FS(register uint8_t lun, register uint32_t *block_num, register uint16_t *block_size)
{
  /* USER CODE BEGIN 3 */
  *block_num  = STORAGE_BLK_NBR;
  *block_size = STORAGE_BLK_SIZ;
  return (USBD_OK);
  /* USER CODE END 3 */
}

void media_change (void)
{

	STORAGE_Init_FS (0);
	usbd_media_reset = 1;

};

int8_t STORAGE_IsReady_FS(register uint8_t lun)
{
	return (USBD_OK);
}

int8_t STORAGE_IsWriteProtected_FS(register uint8_t lun)
{
  return (USBD_OK);
}

int8_t STORAGE_Read_FS (register uint8_t lun, register void *buf, register uint32_t blk_addr, register uint32_t blk_len)
{
//	if (lun) return (USBD_FAIL);
	do {

		// ensure buffer is cleared out
		memset (buf, 0, 512);

		// bounds check: quietly ignore invalid addresses
		if (blk_addr < STORAGE_BLK_NBR) {

			disk_decode_table [blk_addr >> 11].read_handler (buf, blk_addr -
			disk_decode_table [blk_addr >> 11].relative_sector);

		}
		blk_addr++;
		buf += 512;

	} while (--blk_len);

  return (USBD_OK);
}

int8_t STORAGE_Write_FS (register uint8_t lun, register void *buf, register uint32_t blk_addr, register uint32_t blk_len)
{
//	if (lun) return (USBD_FAIL);
	do {

		// bounds check: quietly ignore invalid addresses
		if (blk_addr < STORAGE_BLK_NBR) {

			disk_decode_table [blk_addr >> 11].write_handler (buf, blk_addr -
			disk_decode_table [blk_addr >> 11].relative_sector);

		}
		blk_addr++;
		buf += 512;

	} while (--blk_len);
//debug_ptr ++;
	return (USBD_OK);
}

int8_t STORAGE_GetMaxLun_FS(void)
{
  return (STORAGE_LUN_NBR - 1);
}
