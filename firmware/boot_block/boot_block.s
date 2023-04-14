// Copyright 2019 Big Time Technology Development Group.

  .syntax unified
  .cpu cortex-m3
  .fpu softvfp
  .thumb

//-------------------------------------------------------------------------------
init_sp:		.word	0x20005000			
init_pc:		.word	0x08000009			

				ldr.n	r0, kernel_base
				ldr		r1, [r0, #0]		
				ldr.n	r2, vec_reg			
				mov		sp, r1				
				str		r0, [r2, #8]		
				ldr		r0, [r0, #4]		
				bx		r0					
				
//-------------------------------------------------------------------------------
// unlock_fpec

				.align
unlock_fpec:	ldr.n	r3, fpec_base
				ldr		r0, [r3, #16]	//	control register
				ands	r0, #128		//	test lock bit
				beq		skip_unlock
				ldr.n	r0, fpec_key1
				str		r0, [r3, #4]	//	key register
				ldr.n	r0, fpec_key2
				str		r0, [r3, #4]
skip_unlock:	bx		lr

//-------------------------------------------------------------------------------
// flash_halfword

//				r0  (= 0x40022000)
//				r1	dest. addr
//				r2	src. 16-bit data

				// set the programming bit
				.align									
pgm:			movs	r3, #1							
				str		r3, [r0, #16]					

				// write the halfword out
				strh	r2, [r1, #00]					

				// wait for the busy bit to reset
wait:			ldr		r3, [r0, #12]					
				tst		r3, #1							
				bne		wait							

				// clear the control register bits
				movs	r3, #0							
				str		r3, [r0, #16]					
				bx		lr								

//-------------------------------------------------------------------------------
// page_erase

//				r0	(= 0x40022000)
//				r1	dest. addr

				// erase a page
				.align									
erase:			movs	r3, #2							
				str		r3, [r0, #16]					
				str		r1, [r0, #20]					
				movs	r3, #66							
				str		r3, [r0, #16]					
				b		wait							

//-------------------------------------------------------------------------------
// kernel flash

//				set the retry count to r0
				.align
flash:			movs	r6, r0
				b		retry_lp

				// if not equal, decrement count and try again
flash_fail:		subs	r6, #1
				beq		rst

//				erase flash 0x1000 - 0x7FFF
retry_lp:		ldr.n	r0, fpec_base
				ldr.n	r1, kernel_base
				ldr.n	r4, app_base
erase_lp:		bl		erase
				add		r1, #1024
				cmp		r1, r4
				bcc		erase_lp

//				copy flash 0x8000-0xEFFF -> 0x1000-0x7FFF
				ldr.n	r1, kernel_base
				movs	r5, r4
flash_lp:		ldrh	r2, [r5, #0]
				uxth	r2, r2
				bl		pgm

				// confirm the flashed half word
				ldrh	r7, [r1, #0]
				uxth	r7, r7
				cmp		r2, r7			//	compare it to the source data
				bne		flash_fail

				// increment the pointers
				adds	r5, #2
				adds	r1, #2
				cmp		r1, r4
				bcc		flash_lp				

				// NVIC_SystemReset ();
rst:			dsb		sy
				ldr		r1,	vec_reg
				ldr		r3, [r1, #12]
				and		r2, r3, 0x700
				ldr		r3, rval
				orrs	r3, r2
				str		r3, [r1, #12]
				dsb		sy
				.align
ilp:			nop
				b		ilp

//-------------------------------------------------------------------------------
// sector decrypt and write to flash

// input:
//				r0	ptr to 512-byte encrypted sector data
//				r1	ptr to dest ptr (only flash addresses allowed) (if null, no data is written)

// return:
//				r0	result code

//					0:			invalid data block signature
//					2, 4, 6:	data block failed CRC check
//					3:			good data block
//					5:			good last kernel block
//					7:			good last application block

				.align
decrypt:		push	{r1, r4, r5, r6, r7, r8, r9, r10, r11, lr}

//				load the dest. ptr
				ldr		r1 , [r1, #0]

//				setup the word count
				movs	r11, #128

//				load the sector word
sector_lp:		ldmia	r0!, {r2}

//				load the bit count
				movs	r10, #32

//				test the bit going into the LFSR
lp:				rors	r2 , #1
				bcc		skip

//				r4		shift register pointer
//				r5		encryption key pointer
				ldr.n	r4 , app_RAM_base
				adr		r5 , key
				adds	r3 , r4, #256
xor_lp:			ldmia	r5!, {r8, r9}
				ldmia	r4 , {r6, r7}
				eor		r6 , r8
				eor		r7 , r9
				stmia	r4!, {r6, r7}
				cmp		r4 , r3
				bne		xor_lp

//				r4		shift register pointer + 256
//				r2		temp bit
skip:			ldr.n	r3 , app_RAM_base
				adds	r4 , r3, #256
sr_lp:			ldmdb	r4 , {r6, r7, r8, r9}
				adds	r2 , r2
				rrxs	r9 , r9
				rrxs	r8 , r8
				rrxs	r7 , r7
				rrxs	r6 , r6
				rrxs	r2 , r2
				stmdb	r4!, {r6, r7, r8, r9}
				cmp		r4 , r3
				bne		sr_lp

				// repeat the bit count
				subs	r10, #1
				bne		lp

// casing the destination address
// addresses inside flash range 8000 - FFFF require a valid data block type header,
// or the routine will early exit without taking any flash or erase action.
// invalid dest. addresses require a "FIRMprov" signanture at the end of the block.

				// range check the destination address for 08008000 - 08010000
				ldr.n	r3 , top_flash_addr
				cmp		r1 , r3
				bcs		not_write
				ldr.n	r3 , app_base
				cmp		r1 , r3
				bcc		not_write

				// last word of valid address write
				cmp		r11, #1
				bne		not_last
				pop		{r0}
				ldr.n	r3 , CRC_base
				ldr		r4 , [r3, #0]
				cmp		r4 , r2
				it		eq
				addseq	r0 , #1
				b		cleanup

				// last word of invalid address write
not_write:		cmp		r11, #1
				bne		not_last_2
				pop		{r0}
				ldr.n	r3 , init_signature + 4
				cmp		r2 , r3				//	compare for "prov" signature
				it		eq
				addseq	r0 , #1
				b		cleanup

				// second last word of invalid address write
not_last_2:		cmp		r11, #2
				bne		no_write
				movs	r4 , #2
				ldr.n	r3 , init_signature + 0
				cmp		r2 , r3				//	compare for "FIRM" signature
				it		ne
				movne	r0 , #0
				bne		cleanup				//	early exit if not found
				push	{r4}
				b		no_write

				// first word of valid address write
first_word:		movs	r4 , #0				//	default value is zero
				ldr.n	r3 , data_signature
				cmp		r2 , r3
				it		eq
				movseq	r4 , #2
				ldr.n	r3 , krnl_signature
				cmp		r2 , r3
				it		eq
				movseq	r4 , #4
				ldr.n	r3 , apli_signature
				cmp		r2 , r3
				it		eq
				movseq	r4 , #6
				movs	r4 , r4				//	test for found signatures
				it		eq
				moveq	r0 , r4
				beq		cleanup				//	early exit if none found
				push	{r4}
				b		no_write

				// do final check for first word
not_last:		cmp		r11, #128
				beq		first_word

// at this point, the data (r2) is bound to flash.
// the flash page is erased if the dest. addr. is 1KB aligned.
// the data is split into 2 half-word writes to flash.
// the programmed address is read back & pumped into a CRC register.

//				r0  (= 0x40022000)
//				r1	dest. addr
//				r2	src. 16-bit data

//				if r1 & 0x3FF then erase sector
				push	{r0}
				ldr.n	r0 , fpec_base
				tst		r1 , 0x03FC
				it		eq
				bleq	erase

				// burn the data to flash
				bl		pgm
				adds	r1 , #2
				lsrs	r2 , #16
				bl		pgm
				subs	r1 , #2
				pop		{r0}

				// read the data back and pump it into the CRC register
				ldr.n	r3 , CRC_base
				ldmia	r1!, {r2}				//	read the data back
				str		r2 , [r3, #0]			//	send it to the CRC data register

				// sector loop
no_write:		subs	r11, #1
				bne		sector_lp

				// check for code 3
cleanup:		cmp		r0 , #3
				beq		no_wipe

				// delete shift register buffer on error or end of transfer
				movs	r3 , #0
				ldr.n	r2 , app_RAM_base
				adds	r4 , r2, #256
c_lp:			stmia	r2!, {r3}
				cmp		r2 , r4
				bne		c_lp

				// update the source pointer
no_wipe:		pop		{r2}
				str		r1 , [r2, #0]
				pop		{r4, r5, r6, r7, r8, r9, r10, r11, pc}

//-------------------------------------------------------------------------------
				.align
vec_reg:		.word	0xE000ED00
fpec_base:		.word	0x40022000
fpec_key1:		.word	0x45670123
fpec_key2:		.word	0xCDEF89AB
CRC_base:		.word	0x40023000
kernel_base:	.word	0x08001000
app_base:		.word	0x08008000
rval:			.word	0x05FA0004
app_RAM_base:	.word	0x20002800				//	@ 10KB offset in RAM
top_flash_addr:	.word	0x08010000				//	the packages are trusted to not turf over the 4KB save area @ 0x0800F000
init_signature:	.ascii	"FIRMprov"
data_signature:	.ascii	"BtTd"
apli_signature:	.ascii	"Apli"
krnl_signature:	.ascii	"Krnl"

//-------------------------------------------------------------------------------

				.org	0x09AC

				// functions
				.word	0x08000001 + unlock_fpec
				.word	0x08000001 + decrypt
				.word	0x08000001 + erase
				.word	0x08000001 + pgm
				.word	0x08000001 + flash					

				// firmware dir entry
				.ascii	"FIRMWAREBIN"			//	filename + extension
				.byte	6						//	hidden + system attributes
				.byte	0x18					//	bit 3 for lowercase filename; bit 4 for lowercase extension
				.byte	0xAA					//	creation time centi-seconds
				.hword	0x1F6D					//	creation time
				.hword	0x4E76					//	creation date
				.hword	0x4E76					//	last access date
				.hword	0						//	starting cluster hi
				.hword	0x1F6E					//	modification time
				.hword	0x4E76					//	modification date
				.hword	0x0800					//	starting cluster
				.word	32768					//	file size

				// security dir entry
				.ascii	"SECURITYBIN"			//	filename + extension
				.byte	7						//	hidden + system attributes
				.byte	0x18					//	bit 3 for lowercase filename; bit 4 for lowercase extension
				.byte	0xAA					//	creation time centi-seconds
				.hword	0x1F6D					//	creation time
				.hword	0x4E76					//	creation date
				.hword	0x4E76					//	last access date
				.hword	0						//	starting cluster hi
				.hword	0x1F6E					//	modification time
				.hword	0x4E76					//	modification date
				.hword	0x1000					//	starting cluster
				.word	768						//	file size

				// boot sector
				.byte	0xEB, 0x3C, 0x90		//	near jump
				.ascii	"MSDOS5.0"				//	OEM string
				.byte	0, 2					//	bytes per sector
				.byte	1						//	sectors per cluster
				.hword	2048					//	boot and reserved sectors
				.byte	1						//	number of FATs
				.byte	32, 0					//	number of root directory entries
				.byte	0, 0					//	reserved
				.byte	0xF8					//	media descriptor
				.hword	2048					//	sectors per FAT
				.hword	8						//	sectors per track
				.hword	8						//	number of heads
				.word	0						//	hidden sectors
				.word	0x10FF2					//	total sectors
				.byte	0x80					//	physical drive
				.byte	0						//	reserved
				.byte	0x29					//	boot record signature
				.ascii	"bTtD"					//	serial number
				.ascii	"BTTD Group "			//	volume label
				.ascii	"FAT16   "				//	FAT system in text
				.byte	0x0E, 0x1F
				.byte	0xBE, 0x5B, 0x7C, 0xAC, 0x22, 0xC0, 0x74, 0x0B,	0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 0x10
				.byte	0x5E, 0xEB, 0xF0, 0x32,	0xE4, 0xCD, 0x16, 0xCD, 0x19, 0xEB, 0xFE, 0x54, 0x68, 0x69, 0x73, 0x20
				.byte	0x69, 0x73, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F,	0x74, 0x61, 0x62, 0x6C
				.byte	0x65, 0x20, 0x64, 0x69, 0x73, 0x6B, 0x2E, 0x20,	0x20, 0x50, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x20
				.byte	0x69, 0x6E, 0x73, 0x65,	0x72, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x61, 0x62, 0x6C
				.byte	0x65, 0x20, 0x66, 0x6C, 0x6F, 0x70, 0x70, 0x79, 0x20, 0x61, 0x6E, 0x64,	0x0D, 0x0A, 0x70, 0x72
				.byte	0x65, 0x73, 0x73, 0x20, 0x61, 0x6E, 0x79, 0x20,	0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x74
				.byte	0x72, 0x79, 0x20, 0x61,	0x67, 0x61, 0x69, 0x6E, 0x20, 0x2E, 0x2E, 0x2E, 0x20, 0x0D, 0x0A, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00
				.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA

				// product ID
security:		.ascii	"BTTD Grp"				//	company name
				.ascii	"BLUE-PILL BOOTUP"		//	product
				.ascii	"0.02"					//	version

				// security block - set up by the provisioning tool
				.org	0x0F00					//	authentication block
key:			.org	0x1000					//	encryption key
