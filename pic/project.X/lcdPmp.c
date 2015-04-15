#include "p24fxxxx.h"
#include "lcdPmp.h"
#include "shared.h"

void SPI2Init(void)
{
    //config SPI2
    SPI2STATbits.SPIEN 		= 0;	// disable SPI port
    SPI2STATbits.SPISIDL 	= 0; 	// Continue module operation in Idle mode

    SPI2BUF 				= 0;   	// clear SPI buffer

    IFS2bits.SPI2IF 		= 0;	// clear interrupt flag
    IEC2bits.SPI2IE 		= 0;	// disable interrupt

    SPI2CON1bits.DISSCK		= 0;	// Internal SPIx clock is enabled
    SPI2CON1bits.DISSDO		= 0;	// SDOx pin is controlled by the module
    SPI2CON1bits.MODE16 	= 0;	// set in 16-bit mode, clear in 8-bit mode
    SPI2CON1bits.SMP		= 0;	// Input data sampled at middle of data output time
    SPI2CON1bits.CKP 		= 0;	// CKP and CKE is subject to change ...
    SPI2CON1bits.CKE 		= 1;	// ... based on your communication mode.
    SPI2CON1bits.MSTEN 		= 1; 	// 1 =  Master mode; 0 =  Slave mode
    SPI2CON1bits.SPRE 		= 4; 	// Secondary Prescaler = 4:1
    SPI2CON1bits.PPRE 		= 2; 	// Primary Prescaler = 4:1

    SPI2CON2 				= 0;	// non-framed mode

    SPI_SS_PORT				= 1;	//
    SPI_SS_TRIS				= 0;	// set SS as output

    SPI2STATbits.SPIEN 		= 1; 	// enable SPI port, clear status
}

unsigned short writeSPI2( unsigned short data )
{
    SPI2BUF = data;					// write to buffer for TX
    while(!SPI2STATbits.SPIRBF);	// wait for transfer to complete
    return SPI2BUF;    				// read the received value
}//writeSPI2

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
void LCDInit() {
    LCD_comm(CMD_SET_BIAS_9);
    LCD_comm(CMD_SET_ADC_NORMAL);
    LCD_comm(CMD_SET_COM_NORMAL);
    LCD_comm(CMD_SET_DISP_START_LINE);
    LCD_comm(CMD_SET_POWER_CONTROL | 0x4);
    LCD_comm(CMD_SET_POWER_CONTROL | 0x6);
    LCD_comm(CMD_SET_POWER_CONTROL | 0x7);
    LCD_comm(CMD_SET_RESISTOR_RATIO | 0x6);
    LCD_comm(0x25);
    LCD_comm(0x81);
    LCD_comm(0x19);
    LCD_comm(0x2F);
    LCD_comm(0xAF);
    LCD_comm(0xA4);
}

void LCD_comm(unsigned char c) {
    unsigned int a = 0;
    PORTAbits.RA0 = 0;
    for (a = 0; a < 4; a++);
    writeSPI2(c);
    for (a = 0; a < 4; a++);
}

void LCD_data(unsigned char c){
    unsigned int a = 0;
    PORTAbits.RA0 = 1;
    for(a = 0; a < 4; a++);
    writeSPI2(c);
    for(a = 0; a < 4; a++);
}

void LCDProcessEvents()
{
    unsigned char p, c;
    for(p = 0; p < 8; p++){
	LCD_comm(CMD_SET_PAGE | p);
        LCD_comm(CMD_SET_COLUMN_LOWER | 0x00);
        LCD_comm(CMD_SET_COLUMN_UPPER | 0x00);
        for(c = 0; c < 128; c++){
            LCD_comm(CMD_RMW);
            LCD_data(rpiData.imu[p*128+c]);
        }
    }
}
