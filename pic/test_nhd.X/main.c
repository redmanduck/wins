#include "p24Fxxxx.h"

#include "lcdPmp.h"

_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
_CONFIG2(FCKSM_CSECMD & OSCIOFNC_ON & POSCMOD_XT & FNOSC_PRI )
unsigned long _temp32;


int main (void)
{
	// Disable Watch Dog Timer
	RCONbits.SWDTEN = 0;

	// Continuous Pin Toggle Init
	ODCAbits.ODA6 	= 0;
	TRISAbits.TRISA6 	= 0;

	// LCD Initialization
	LCDInit();

	// demo function in while.
	while (1) {

		LCDProcessEvents();		// display something on LCD with PMP

		__builtin_btg((unsigned int *)&LATA, 6);

		for (_temp32 = 0; _temp32 < 30000; _temp32++);

		// please add your function
	}
}
