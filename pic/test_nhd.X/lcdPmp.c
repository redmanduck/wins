#include "p24fxxxx.h"
#include "lcdPmp.h"

void LCDInit()
{
	unsigned int Local_16;
	
	// PMCON
	// PMP enabled
	PMCONbits.PMPEN		= 1;	
	// 1 = Discontinues module operation when device enters Idle mode
	PMCONbits.PSIDL		= 1;
	// 0 = Address and data appear on separate pins
	PMCONbits.ADRMUX	= 0;	
	// Byte Enable Port disabled
	PMCONbits.PTBEEN	= 0;	
	// Write Enable Strobe Port enabled
	PMCONbits.PTWREN	= 1;	
	// Read/Write Strobe Port enabled
	PMCONbits.PTRDEN	= 1;	
	// PMCS1 and PMCS2 function as chip select
	PMCONbits.CSF		= 0;
	// Address Latch Polarity Active-high (PMALL and PMALH)
        // ???????????????????????????????????????????????????
	PMCONbits.ALP		= 1;	
	// Chip Select 2 Polarity Active-low
	PMCONbits.CS2P		= 0;
	// Chip Select 1 Polarity Active-low
	PMCONbits.CS1P		= 0;
	// Byte Enable Active-high
	PMCONbits.BEP		= 1;	
	// Master Mode, Write Strobe active-low
	PMCONbits.WRSP		= 0;
	// Master Mode, Read/write strobe active-low
	PMCONbits.RDSP		= 0;
	
	// PMMODE
	// No interrupt generated
	PMMODEbits.IRQM		= 0;	
	// No increment or decrement of address
	PMMODEbits.INCM		= 0;	
	// 8-bit data mode
	PMMODEbits.MODE16	= 0;	
	// Master mode 1(PMCSx, PMRD/PMWR, PMENB, PMBE, PMA<x:0> and PMD<7:0>)
	PMMODEbits.MODE		= 3;	
	// Data wait of 4Tcy; multiplexed address phase of 4 Tcy
	PMMODEbits.WAITB	= 3;	
	// Read to Byte Enable Strobe: Wait of additional 15 Tcy
	PMMODEbits.WAITM	= 0xf;	
	// Data Hold After Strobe: Wait of 4 Tcy
	PMMODEbits.WAITE	= 3;	
	
	// PMADDR
	// For LCD, there is no address, so zero is assigned.
	PMADDR 	= 0xC000;
	
	// PMAEN
	// PMA15:2 function as port I/O, PMALH/PMALL enabled
	PMAEN 	= 0x0001;			
	
	for (Local_16 = 0; Local_16 < 40000; Local_16++);
	// Set the default function, DL:8-bit
	LCDwrite(LCD_I, CMD_SET_BIAS_9);

	for (Local_16 = 0; Local_16 < 40; Local_16++);
	// Set the display control, turn on LCD
	LCDwrite(LCD_I, CMD_SET_ADC_NORMAL);

	for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
	LCDwrite(LCD_I, CMD_SET_COM_NORMAL);

	for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
	LCDwrite(LCD_I, CMD_SET_DISP_START_LINE);

	for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
        LCDwrite(LCD_I, CMD_SET_POWER_CONTROL | 0x4);

	for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
        LCDwrite(LCD_I, CMD_SET_POWER_CONTROL | 0x6);
        
        for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
        LCDwrite(LCD_I, CMD_SET_POWER_CONTROL | 0x7);

        for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
        LCDwrite(LCD_I, CMD_SET_RESISTOR_RATIO | 0x6);

        for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
	LCDwrite(LCD_I, 0x25);

	for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
	LCDwrite(LCD_I, 0x81);

	for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
	LCDwrite(LCD_I, 0x19);

	for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
	LCDwrite(LCD_I, 0x2F);

	for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
	LCDwrite(LCD_I, 0xAF);

        for (Local_16 = 0; Local_16 < 20; Local_16++);
	// Set the entry mode, set cursor in increase mode
	LCDwrite(LCD_I, 0xA4);
}


/*********************************************************************
 * Function: LCDProcessEvents
 *
 * Preconditions: None.
 *
 * Overview: This is a state mashine to issue commands and data to
 * LCD. Must be called periodically to make LCD message processing.
 *
 * Input: None.
 *
 * Output: None.
 *
 ********************************************************************/
void LCDProcessEvents()
{
    unsigned char p, c;
    for(p = 0; p < 8; p++){
	LCDwrite(LCD_I, CMD_SET_PAGE | p);
        LCDwrite(LCD_I, CMD_SET_COLUMN_LOWER | (c&0xf));
        LCDwrite(LCD_I, CMD_SET_COLUMN_UPPER | ((c>>4) & 0xf));
        for(c = 0; c < 128; c++){
            LCDwrite(LCD_I, CMD_RMW);
            LCDwrite(LCD_D, p);
        }
    }
}

unsigned char LCDbusy()
{
    return 0x00;
}

void LCDclear()
{
    unsigned char p;
    for(p = 0; p < 8; p++){
	LCDwrite(LCD_I, CMD_SET_PAGE | p);
    }
}

void LCDwrite(unsigned char inputType, unsigned char inputByte)
{
	PMADDR = inputType;
	PMDIN1 = inputByte;
}
