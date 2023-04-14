  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb


.global	g_pfnVectors
.global	Default_Handler


 	.section	.isr_vector,"a",%progbits
	.type	g_pfnVectors, %object
	.size	g_pfnVectors, .-g_pfnVectors


g_pfnVectors:

_file_size:		.word	0						//	in 32-bit words
_CRC32:			.word	-1
_company_name:	.ascii	"BTTD Grp"				//	do not change length of string
_product_name:	.ascii	"USB-NES lite    "		//	do not change length of string
_version:		.ascii	"0.79"					//	do not change length of string
_init_hndlr:	.word	init
_systick_hndlr:	.word	systick
