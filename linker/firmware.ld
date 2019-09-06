ENTRY(Reset_Handler)

/* Memory */
MEMORY
{
  flash (rx) : ORIGIN = 0x00000000, LENGTH = 32k
  /* reserve 304 bytes from 0x50 to 0x180 for the USB on chip ROM driver API */
  /* sram (rwx) : ORIGIN = 0x10000180, LENGTH = 0x1E80 */
  /* sram_low (rwx) : ORIGIN = 0x10000000, LENGTH = 0x50 */
  sram (rwx) : ORIGIN = 0x10000000, LENGTH = 0x2000
}

_end_stack = 0x10001FFC;

SECTIONS
{
	. = ORIGIN(flash);

	vectors :
	{
		KEEP(*(.vectors))
	} >flash

	.text : ALIGN(4)
	{
		FILL(0xff)
		*(.text*)
		*(.rodata*)
	} >flash

	.data :
	{
		_start_data = .;
		*(.data*)
		_end_data = .;
	} >sram AT >flash

	_start_data_flash = LOADADDR(.data);

	. = ALIGN(4);

	.bss :
	{
		_start_bss = .;
		*(.bss*)
		_end_bss = .;
	} >sram

	. = ALIGN(4);

	_start_stack = .;
}

_end = .;
PROVIDE (end = .);
