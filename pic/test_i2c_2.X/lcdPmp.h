/*********************************************************************
 *
 * LCD Driver for PIC24.
 *
 *********************************************************************
 * FileName:        lcdPmp.h
 * Dependencies:    
 * Processor:       PIC24
 * Compiler:       	MPLAB C30
 * Linker:          MPLAB LINK30
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company's
 * customer, for use solely and exclusively with products manufactured
 * by the Company. 
 *
 * The software is owned by the Company and/or its supplier, and is 
 * protected under applicable copyright laws. All rights are reserved. 
 * Any use in violation of the foregoing restrictions may subject the 
 * user to criminal sanctions under applicable laws, as well as to 
 * civil liability for the breach of the terms and conditions of this 
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, 
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT, 
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR 
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * A simple LCD driver for LCDs interface through the PMP
 * 
 *
 *
 * Author           Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Albert Z.		02/05/08	draft
 ********************************************************************/

// Display line length.
// number of characters displayed in a line
#define LCD_DISPLAY_LEN 	16	
	
// number of lines displayed on the LCD
#define LCD_DISPLAY_LINE	2		

#define LCD_LINE1			0		// first line
#define LCD_LINE2			1		// second line

#define LCD_I				0		// instruction
#define LCD_D				1		// data

#define LCD_Busy			0x80	// LCD is busy
#define LCD_Free			0		// LCD is free

// clear display
#define LCD_ClearDisplay	0x01	
// move cursor back to 00h position
#define LCD_ReturnHome		0x02	
// assign cursor as increase mode but screen not shift
#define LCD_CursorIncNS		0x06	
// assign cursor as decrease mode but screen not shift
#define LCD_CursorDecNS		0x04	
// assign cursor as increase mode and screen shift
#define LCD_CursorIncS		0x07	
// assign cursor as decrease mode adn screen shift
#define LCD_CursorDecS		0x05	
// Display on, Cursor on, Blinking of Cursor off
#define LCD_DonConBoff		0x0e	
// Display on, Cursor on, Blinking of Cursor on
#define LCD_DonConBon		0x0f	
// Display on, Cursor off, Blinking of Cursor off
#define LCD_DonCoffBoff		0x0c	
// Display off, Cursor off, Blinking of Cursor off
#define LCD_DoffCoffBoff	0x08	
// Cursor shift enabled, shift right
#define LCD_CursorShiftL	0x1c	
// Cursor shift enabled, shift left
#define LCD_CursorShiftR	0x18	
// Cursor shift disabled
#define LCD_CursorNonShift	0x10	
// 4-bit data length
#define LCD_DataLength4		0x20	
// 8-bit data length
#define LCD_DataLength8		0x3C	
// set 4-bit CGRAM address
#define LCD_CGRAM(_cgramAddr)	(((_cgramAddr)&0x0f)|0x40)		
// set 7-bit DDRAM address, line 1
#define LCD_DDRAM1(_ddramAddr)	(((_ddramAddr)&0x0f)|0x80)		
// set 7-bit DDRAM address, line 2
#define LCD_DDRAM2(_ddramAddr)	(((_ddramAddr)&0x0f)|0xc0)		



/*********************************************************************
 * Function: LCDProcessEvents
 *
 * Preconditions: None.
 *
 * Overview: 
 * This is a state mashine to issue commands and data to LCD. Must be
 * called periodically to make LCD message processing.
 *
 * Input: None.
 *
 * Output: None.
 *
 ********************************************************************/
void LCDProcessEvents(void);

/*********************************************************************
 * Function: LCDInit
 *
 * Preconditions: None.
 *
 * Overview: This is a LCD intialising routine which should be called
 * before show the characters on LCD.
 *
 * Input: None.
 *
 * Output: None.
 *
 ********************************************************************/
void LCDInit(void);


void LCDclear(void);
unsigned char LCDbusy(void);
void LCDwrite(unsigned char , unsigned char );
void LCDwriteLine(unsigned char , unsigned char * );

