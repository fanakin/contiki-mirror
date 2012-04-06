
#include "36064s.h"
#include "defines-arch.h"
#include "inh8300h.h"



void lowlevelinit(void)
{
//BEGIN abilitazione whatch dog. Ultima cosa da fare prima di andare nel main loop
WDT.TCSRWD.BYTE = 0x10;
//END

// settaggio porte non usate come output e basse
IO.PCR1 = (BIT2 | BIT0); // P10 wismo ON. P12 wismo RST
IO.PDR1.BYTE &= ~0x05;

IO.PCR2 = (BIT4 | BIT3 | BIT0);
IO.PDR2.BYTE |= 0x19;

IO.PCR3 = (BIT2 | BIT1 | BIT0); // eeprom E0,E1,E2 signals
IO.PDR3.BYTE = 0x00;

//IO.PCR5 = (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
//IO.PDR5.BYTE &= ~0x3F;

// P60 Relay0
// P61 Relay1
// P62 RelayH
// P66 TXCT-
IO.PCR6 = (BIT0);
IO.PDR6.BYTE |= 0x01;

IO.PCR7 = (BIT6 | BIT5 | BIT4);
IO.PDR7.BYTE &= ~0x70;

// P86 usata come out per notWC signal della m24c01
IO.PCR8 = (BIT6);
IO.PDR8.BYTE = 0x00;

}



void
irq_init(void)
{
	reset_imask_ccr();
}

