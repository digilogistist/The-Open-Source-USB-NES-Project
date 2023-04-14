USB-NES Lite Project
Public Release

Brad Taylor
BTTD Group
usbnes.com
04/05/2023



This is the complete USB-NES Lite 0.08/0.79 production unit engineering archive, containing:

- Source code
- Firmware binaries (composite and discrete)
- PCB engineering files (USB-NES, NES-BACKUP and new NES-FASTGFX)
- Serial wire debug wiring diagram
- Technical Documentations
- Quick setup guide print-out
- DIY kit guide print-out (manufacturing instructions)
- Owner's manual PDF
- 3D case models
- Manufacturing BOMs
- Photo gallery



+-------------------+
| Firmware Binaries |
+-------------------+

Inside the USB-NES is an STM32F103C8 microcontroller running at 72 MHz, which usually includes a very cozy 64 K Bytes of on-board flash memory.  Sometimes as a bonus though, the "Blue Pill" comes with double that amount of storage (and possibly even all the time).  While these differences are normally considered a toss-up on a few innocent chance purchases, the 128 KB flash types (aka incognito 'F103CB ones) are more commonly seen with marketplace Blue Pill system boards at slightly higher price points.

Nowadays, the Blue Pills are not as cheap, available, nor as powerful as newer mini system board offerings on the market, but the first generation blue pills with the through-hole mounted micro-USB port and low-profile reset push button switches are the absolute best types you can still get your hands on, if you're lucky.

Moreover, the main functional USB-NES firmware uses only the first 64 KB of flash, where as units with 128 KB would include a DEMO.NES graphics demonstration ROM that would fit in the bonus 64 KB of flash as additional superfluous brand marketing.  The firmware checks for the classic iNES signature where the DEMO.NES file is allocated on the flash to determine if the file should be created in the USB-NES file system.  Therefore, the DEMO.NES binary allocated there can technically be any defacto .NES image that can fit within the high 64 KB flash of a lucky 128 KB Blue Pill sample.


Micro Type      Flash Location  Size    Composite Binary Source
----------      --------------  ----    -----------------------
STM32F103C8     0x 0800 0000     64 KB  /USB-NES_Lite_0.08-0.79_64K.bin
STM32F103CB*    0x 0800 0000    128 KB  /USB-NES_Lite_0.08-0.79_128K.bin

* may also be labelled as a "lucky" 'F103C8 model.


Flash section   Flash Location  Size    Discrete Binary Location
-------------   --------------  ----    ------------------------
Boot Block   #  0x 0800 0000     4 KB   /firmware/boot_block/boot_block.bin
Kernel      ~*  0x 0800 1000    28 KB   /firmware/kernel/Debug/kernel.bin
Application ^*  0x 0800 8000    26 KB   /firmware/application/Debug/application.bin
SETTINGS.TXT %  0x 0800 E800     1 KB
BCMTABLE.BIN    0x 0800 EC00     1 KB   /firmware/BCMTABLE.BIN
OVERRIDE.BIN    0x 0800 F000     4 KB   /firmware/OVERRIDE.BIN
DEMO.NES     &  0x 0801 0000    64 KB   /firmware/DEMO.BIN

~ Uses lower 10 KB of system RAM
^ Uses upper 10 KB of system RAM
* Compiled using Atollic TruStudio (see ./BUILD.BAT for console log)
% This data is auto-generated by the application firmware
& DEMO.NES is disabled in SETTINGS.TXT by default
# Assembled and output to binary using windy script /firmware/boot_block/BUILD.BAT


Flashing the discrete binaries to USB-NES
-----------------------------------------
1. Acquire an ST-LINK Serial Wire Debug programming tool; they are cheap + easy to get.
2. Install the ST-LINK programming utility software.
3. Refer to the /USB-NES-SWD_pinout.jpg for connecting the ST-LINK to USB-NES via SWD.
4. Be sure the USB-NES has its USB port powered when connecting via SWD.
5. Hold in the reset button on USB-NES for a couple of seconds or so, while using the ST-LINK utility to connect to the STM32F103 micro via SWD interface.
6. Deactivate the readout protection on the STM32F103 by unchecking it in the option switches.  Please note this will completely erase the existing contents of the USB-NES firmware.  If the readout protection is not active, you should instead erase the entire contents of the chip by using the utility's Erase command.
7. Load the binary files into the ST-LINK utility*, and program them to the aforementioned corresponding locations in flash memory.  The five critical binaries to flash to USB-NES are the Boot Block, Kernel, Application, BCMTABLE.BIN, and OVERRIDE.BIN.  SETTINGS.TXT is auto-generated by the application, and DEMO.NES is entirely optional (for 128 KB units only).

* alternately, the ST-LINK utility command line interface may also be used to flash all five critical binaries automatically; please see /FLASHBALL.BAT for a windy example.



+-------------+
| Source Code |
+-------------+

The Kernel and Application projects offered here are built with STM32cubeMX, using the USB FS and mass storage class middlewares provided by ST Microelectronics, which is copywritten code.  Therefore, their source code is provided here strictly under the "fair use" doctrine; no infringement is implied nor intended in its parallel inclusion with this public-domain USB-NES preservation project and related functional source code files.

This default USB layer uses almost all the flash space in the kernel, but probably can be optimized to use less.  Moreover, the STM32cubeMX does not allow the implementation of a USB composite device (such as MSC + HID) on this particular STM32 micro; this is why USB-NES Lite never got NES gamepad socket add-ons over its trial run.  Many on-and-off attempts were made to hack together the two separate STM32 USB functions with no success; it is not clear to me at the time of this writing if the STM32F103C8 has enough flash or RAM resources to employ a USB MSC + HID composite device within the confines of the kernel binary.

The main functional USB-NES source code provided here is located as follows:

Source type     Language    Size    Location
-----------     --------    ----    --------
Boot Block      ARM M3 ASM   13745  /firmware/boot_block/boot_block.s
Kernel          C            25177  /firmware/kernel/Src/usbd_storage_if.c
Application     C           120352  /firmware/application/Src/main.c



+-----------+
| PCB Files |
+-----------+

Both USB-NES-02 and NES-BACKUP-02 are electrically identical products, and differ only with the Blue Pill/USB port orientation (side vs. back) to their respective PCBs.

The NES-BACKUP-02 PCB predates USB-NES-02, and does not have a nice shell design like for USB-NES Lite.  Note the only feature of this PCB is the 6-pin header that connects to the NES cartridge CIC pins, for easy AVR CIclone boardset programming.  Only 11 NES-BACKUP-01 units are currently known to exist.  Note there is no corner mounting holes, nor is there a footprint for the necessary electrolytic capacitor in this design, which are bugs that need addressing.  The -02 revision fixes a footprint pinout bug present on the -01 models, involving the orientation of the power MOSFET component.

USB-NES-02 is a better follow-up PCB to the aforementioned design, that adds in an experimental mini phone plug for theoretical expansion audio access, and leaves room in the front for further possible expansions like NES gamepad ports and such.  At least 100 USB-NES-01 PCB assemblies are known to exist, and many of these have been assembled into complete USB-NES Lite consumer units.  Note there is no footprint for the necessary electrolytic capacitor in this design, but it is possible to piggy-back solder it onto existing points on the PCB assembly (see manufacturing instructions).  A 1.6 mm PCB thickness is strongly recommended for manufacturing USB-NES-02.  All USB-NES firmware binaries mentioned in this document are intended for USB-NES-02.

USB-NES-04 is an enhanced version of the previous design that adds four controller ports to the USB-NES.  There is no firmware that exists yet to take advantage of the enhancements in this hardware design.  Moreover, no PCBs nor related shell designs have yet been manufactured for this either.  More info here:
https://youtu.be/dRpi10Yrjks

NES-FASTGFX-01 is a completely original concept NES cart mapper PCB design made for new NES game development.  A complete manufacturing BOM + working (as of 04-13-2023) parts source list for NES-FASTGFX-01 can be found in "/PCB (NES-FASTGFX-01)/BOM-FASTGFX.ods"

NES-FASTGFX-01 Specs
--------------------
- 512 K byte PRG flash (SST39SF040)
- 32 K byte PRG-RAM
- 512 K byte CHR-RAM
- 32768 playfield tiles (@ PPU $0000-$0FFF)
- 4 PPU mirroring modes
- 16-bit VBLANK time stamp counter
- 128 sprite banks (@ PPU $1000-$1FFF)
- HI name table located in first 2 KB of CHR-RAM
- 4-CPU clock cycle CHR-RAM byte filling @ PPU $2000-$3EFF*
- ATMEL ATtiny13A CIClone twin footprint (through-hole & SMT)

* CPU writes to PPU data I/O register $2xx7/$3xx7, will write the encoded 8-bit xx value to the CHR-RAM in one single STA/STX/STY abs. 6502 instruction.  In this case, writes to PPU $2000-$2FFF memory will fill the HI name tables with the xx value (and internal 2 KB LO NT as per usual CPU data), while writes to PPU $3000-$3EFF will write xx to the selected CHR-RAM sprite bank (meanwhile disabling the LO NT).  The CHR-RAM may also be disabled during name table fills, when updating only the LO NT is desired.   Moreover, the CHR-RAM may also be bi-directionally accessed via the traditional CPU $2007 data I/O port @ PPU $1000-$1FFF memory, swapping in the selected 4 KB sprite bank there as well.  PPU bank $0000-$0FFF is only used for playfield rendering; it can just be ignored otherwise.

The most notable feature of this mapper is the expansion of the original 8-bit name table tile codes to 15-bits; this is made possible by using a private graphics data bus in the cartridge for handling the NT HI business during a NT fetch phase.  Moreover, the NT fetch phase is identified as a read cycle from PPU $2000-$3FFF that follows a read from PPU $0000-$1FFF; this corresponds to how the PPU fetches CHR memory during on-screen playfield rendering.  The NT HI byte is then fetched from the first 2KB of CHR-RAM (using the identical mirroring scheme the NES internal 2K VRAM is configured for), and is stored in a register.  The register is then used as the basis for the 4KB bank selection for pattern graphics access @ PPU $0000-$0FFF, which is where the PPU should be normally configured to allocate the 4KB playfield graphics bank.

The VBLANK time stamp counter is reset from any reads from the PPU, so once screen rendering is done, this counter will start to count up every CPU clock.  Consequently, the 16-bit value read here can indicate how far the PPU is in VBLANK or the frame render period, relative to the CPU NMI event (or thereabouts).  If your VBLANK code runs past the 20-scanline (NTSC) limit, the time stamp counter can be used as the basis for forming the correct PPU scroll programming values, based on how many scanlines the render has advanced into the visible frame.  Reading the LO byte of the time stamp count register latches the next LO and HI 8-bit time stamp count samples to be read back.  Therefore, it is possible to only just read the LO TSC register when measuring CPU clock counts of less than 256 clocks.

When using FASTGFX for NES game development, writing to PPU memory is up to 50% faster, and VBLANK code time may be extended dynamically past the on-screen cutoff point, all while having access to a serious amount of graphical tiles for use with both backgrounds and sprites.

Flash Cart Story
----------------
NES-FASTGFX-01 is the answer to a previous programmable NES cart production attempt I made back in 2018: "NES-RED-PILL-02". This older design was more complicated due to the presence of both CHR-RAM and CHR-flash, where the PPU architecture is split into separate sections for sprites (RAM) and playfield tiles (flash/RAM).  The idea was to have 512 KB of CHR-flash for background tiles, and 32 KB CHR-RAM for sprites + BG, but the hardware required to implement the 16-bit background tile index code extensions ended up making programming the CHR-flash chip a complex nightmare.

This was also my first attempt implementing 5-volt flash chips into a new NES cart design.  One critical detail about using flash chips for PRG memory versus SRAM is the requirement of an inverted read/write line for the /OE input on the chip, which would normally just be tied to ground for a typical SRAM implementation.

My long-term plan was to develop game software for the NES using these cart boards, while using NES-BACKUP/USB-NES as the main flashing/programming tool, providing an easy-to-use drag + drop user interface.  These boards have a lot of SMD parts on them, so I got ten of them manufactured and populated professionally, which also made this cart production really expensive.

Subsequently however, once I had built NES-BACKUP and started testing out the NES-RED-PILL boards, I found out I made some fatal design mistakes on the boards.  Most notably, I discovered that both CHR and PRG flash chips could not be programmed efficiently or even at all, without reworking the boards and adding another chip!  After eventually reworking one board, I did manage to get the PRG + CHR flashing to work with USB-NES, but when programming the CHR-flash there was always 4KB in the last bank on the chip that would not flash correctly.  I couldn't figure out what the source of the CHR-flash corruption was, and lost my nerve.  From there on (late 2019), I focused on developing the USB-NES firmware, since I was at a loss with my game boards.

Then three years later, I reworked a second NES-RED-PILL board to discover that the CHR-flash on the first board was actually fried.  Apparently, all the initial testing I was doing on these boards before reworking them was enough to cause the CHR-flash chip on one board to partially self-destruct, and cause it to always fail a full flash memory programming test.  But now, the second reworked board was flashing perfectly using USB-NES!  So now I know what I did wrong, and how to do it right this time.

I took everything I learned from that first costly and disappointing NES-RED-PILL production disaster, trimmed the fat and tweaked the ideas, and made NES-FASTGFX: a state-of-the-art, open-source, graphics expansion NES development cartridge boardset PCB design, that adds powerful extensions to the NES architecture with a simplified programming environment, and a handful of easy-to-source boardset parts.

USB-NES support for NES-FASTGFX boardset development with full drag + drop PRG-flashing is speculated down the road, using existing methods in the USB-NES source code that have been 100% tested and confirmed working on reworked NES-RED-PILL-02 flash boards.  More info on the NES-RED-PILL can be seen here:

https://youtu.be/C34mVn180Tk
https://youtu.be/JX10NZo6VX4



+-------------------+
| USB-NES Lite BOM  |
+-------------------+

/BOM-final_USB-NES_Lite_2021.ods

This bill of materials includes everything to make a production USB-NES Lite system, with a working (as of 04-13-2023) part source list.

The 3D models are typically printed via 1.75/0.4 mm filament/nozzle 3D printer with 5% infill, 0.2 mm layers, and 1.2 mm top/bottom thickness.

The PCB files were made with Kicad.

Do not assemble before reading the manufacturing instructions.



+----------------------------+
| Manufacturing Instructions |
+----------------------------+

The step-by-step assembly guide for building USB-NES Lite is contained in:
/USB-NES DIY KIT (p.1).pdf
/USB-NES DIY KIT (p.2).pdf
https://usbnes.com/usb-nes-lite-diy-kit/

The companion video can be found here (doesn't cover adding the capacitor):
https://youtu.be/VL_1Z1ONgns

The capacitor install guide here:
/cap_add/
https://usbnes.com/power-system-upgrade/



+------------+
| Thank-Yous |
+------------+

All the USB-NES Lite project supporters
The entire NES emulation dev scene
Generational Gamer
Paul Belanger
Thor Robinson
Troy Denton
Henry Hoy
Wolf
Disch
Kevtris
Bootgod
Bregalad
Memblers
jeffqchen
NEScartDB
NESDEV wiki
Eugene's blog
ST Microelectronics
Nintendo of America, Inc. (and NCL of Japan, too)
And of course, the reader



+---------+
| License |
+---------+

USB middleware and other source code portions, copyright ST Microelectronics.  This copywritten work is included here for "fair use" and educational purposes only.

Except for source code explicitly copyrighted by ST Microelectronics, the rest of the work in this archive is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.

http://creativecommons.org/licenses/by-sa/4.0/