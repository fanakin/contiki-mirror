OUTPUT_FORMAT("elf32-h8300")
/*OUTPUT_FORMAT("coff-h8300")*/
OUTPUT_ARCH(h8300hn)
ENTRY("_start")
/* The memory size is 256KB to coincide with the simulator.
   Don't change either without considering the other.  */
MEMORY
{
	/* 0xc4 is a magic entry.  We should have the linker just
		skip over it one day... */
	vectorsarea : o = 0x0000, l = 0x50 /* controllare */
	/* 32 kbyte di rom */
	flash  : o = 0x050,  l = (0x7fff - 0x50)
	/* ram*/
	ram    : o = 0xf780, l = 0x700
	/* The stack starts at the top of main ram.  */
	topram : o = 0xfe80, l = 0x100

}
SECTIONS 				
{ 					
.vectors : {
	/* Use something like this to place a specific function's address
	   into the vector table.
	LONG(ABSOLUTE(_foobar)) vedi il five h8sintv.h ;si usera' short per la modalita' normal */
	SHORT(ABSOLUTE(_start))						/* 0 */
	SHORT(ABSOLUTE(__RESERVED))					/* 1 */
	SHORT(ABSOLUTE(__RESERVED))					/* 2 */
	SHORT(ABSOLUTE(__RESERVED))					/* 3 */
	SHORT(ABSOLUTE(__RESERVED))					/* 4 */
	SHORT(ABSOLUTE(__RESERVED))					/* 5 */
	SHORT(ABSOLUTE(__RESERVED))					/* 6 */
	SHORT(ABSOLUTE(__NMI_HANDLER))				/* 7 */
	SHORT(ABSOLUTE(__TRAP0_HANDLER))			/* 8 */
	SHORT(ABSOLUTE(__TRAP1_HANDLER))			/* 9 */
	SHORT(ABSOLUTE(__TRAP2_HANDLER))			/* 10 */
	SHORT(ABSOLUTE(__TRAP3_HANDLER))			/* 11 */
	SHORT(ABSOLUTE(__ADDRESSBREAK_HANDLER))		/* 12 */
	SHORT(ABSOLUTE(__SLEEP_HANDLER))			/* 13 */
	SHORT(ABSOLUTE(__IRQ0_HANDLER))				/* 14 */
	SHORT(ABSOLUTE(__IRQ1_HANDLER))				/* 15 */
	SHORT(ABSOLUTE(__IRQ2_HANDLER))				/* 16 */
	SHORT(ABSOLUTE(__IRQ3_HANDLER))				/* 17 */
	SHORT(ABSOLUTE(__WKP_HANDLER))				/* 18 */
	SHORT(ABSOLUTE(__RESERVED))					/* 19 */
	SHORT(ABSOLUTE(__RESERVED))					/* 20 */
	SHORT(ABSOLUTE(__RESERVED))					/* 21 */
	SHORT(ABSOLUTE(__TVI_HANDLER))				/* 22 */
	SHORT(ABSOLUTE(__SCI3_HANDLER))				/* 23 */
	SHORT(ABSOLUTE(__IIC2_HANDLER))				/* 24 */
	SHORT(ABSOLUTE(__ADI_HANDLER))				/* 25 */
	SHORT(ABSOLUTE(__TZITMZ0_HANDLER))			/* 26 */
	SHORT(ABSOLUTE(__TZITMZ1_HANDLER))			/* 27 */
	SHORT(ABSOLUTE(__RESERVED))					/* 28 */
	SHORT(ABSOLUTE(__TB1TMIB1_HANDLER))			/* 29 */
	SHORT(ABSOLUTE(__RESERVED))					/* 30 */
	SHORT(ABSOLUTE(__RESERVED))					/* 31 */
	SHORT(ABSOLUTE(__SCI3_2_HANDLER))			/* 32 */
	}  > vectorsarea
.text :	{
	*(.rodata) 				
	*(.rodata.str1.1)
	*(.text) 				
	*(.strings)
	_text_end = . ; 
   	 _etext = . ;
	}  > flash
.data : AT(ADDR(.text) + SIZEOF(.text)){
	 _data_start = . ;
	*(.data)
	 _data_end = . ;
	}  > ram
.bss : {
	 _bss_start = . ;
	*(.bss)
	*(COMMON)
	_bss_end = . ;
	_end = .;
	}  >ram
.stack (NOLOAD) : {
	*(.stack)
	 _stack = . ;
	}  > topram
}
