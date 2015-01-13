/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2015
 * (c) NXP Semiconductors 2013-2015
 * Linker script memory definitions
 * Created from LinkMemoryTemplate
 * By LPCXpresso v7.0.0 [Build 92] [2014-02-20]  on Tue Jan 13 10:58:52 KST 2015)
*/

MEMORY
{
  /* Define each memory region */
  MFlash96 (rx) : ORIGIN = 0x0, LENGTH = 0x18000 /* 96K bytes */
  RamLoc8 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x2000 /* 8K bytes */
  RamPeriph2 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x800 /* 2K bytes */
  SRAM1_2 (rwx) : ORIGIN = 0x20004000, LENGTH = 0x800 /* 2K bytes */


}
  /* Define a symbol for the top of each memory region */
  __top_MFlash96 = 0x0 + 0x18000;
  __top_RamLoc8 = 0x10000000 + 0x2000;
  __top_RamPeriph2 = 0x20000000 + 0x800;
  __top_SRAM1_2 = 0x20004000 + 0x800;



