/**********************************************************************
* ?2008 Microchip Technology Inc.
*
* FileName:        SPI_Master.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       PIC24Fxxxx
* Compiler:        MPLAB?C30 v3.20 or higher
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all
* ownership and intellectual property rights in the code accompanying
* this message and in all derivatives hereto.  You may use this code,
* and any derivatives created by any person or entity by or on your
* behalf, exclusively with Microchip's proprietary products.  Your
* acceptance and/or use of this code constitutes agreement to the
* terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS". NO
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
* NOT LIMITED TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS
* CODE, ITS INTERACTION WITH MICROCHIP'S PRODUCTS, COMBINATION WITH
* ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE
* LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR
* BREACH OF STATUTORY DUTY), STRICT LIABILITY, INDEMNITY,
* CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR
* EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER
* CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE
* DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE BY LAW,
* MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS
* CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP
* SPECIFICALLY TO HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and
* determining its suitability.  Microchip has no obligation to modify,
* test, certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author        Date      	Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Albert Z.		3/27/09		first release
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
*
* Code Tested on:
* Explorer 16 Development board with PIC24FJ128GA010.
* The Processor starts with the 8MHz External Crystal (PLL to 16MIPS).
* SPI is worked on 16:1 prescaler = 1MHz.
*
**********************************************************************/

#include "p24fxxxx.h"

#ifdef __PIC24FJ128GA010__	//Defined by MPLAB when using 24FJ256GB110 device
	// JTAG/Code Protect/Write Protect/Clip-on Emulation mode
	// Watchdog Timer/ICD pins select
	_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
	// Disable CLK switch and CLK monitor, OSCO or Fosc/2, HS oscillator,
	// Primary oscillator
	_CONFIG2(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL)
#endif

// RB2 as SS for SPI slave MCU
#define SPI_SS_TRIS      TRISBbits.TRISB2
#define SPI_SS_PORT      PORTBbits.RB2

unsigned short spiBufT[]={0x0,0xDEAD,0xBEEF,0x0};	// 8 byte
unsigned short counter = 0;
unsigned short spiBufR[]={0,0,0,0};	// SPI buffer for Receiving

void SPI1Init(void)
{
    //config SPI1
    SPI1STATbits.SPIEN 		= 0;	// disable SPI port
    SPI1STATbits.SPISIDL 	= 0; 	// Continue module operation in Idle mode

    SPI1BUF 				= 0;   	// clear SPI buffer

    IFS0bits.SPI1IF 		= 0;	// clear interrupt flag
    IEC0bits.SPI1IE 		= 0;	// disable interrupt

    SPI1CON1bits.DISSCK		= 0;	// Internal SPIx clock is enabled
    SPI1CON1bits.DISSDO		= 0;	// SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 	= 1;	// set in 16-bit mode, clear in 8-bit mode
    SPI1CON1bits.SMP		= 0;	// Input data sampled at middle of data output time
    SPI1CON1bits.CKP 		= 0;	// CKP and CKE is subject to change ...
    SPI1CON1bits.CKE 		= 1;	// ... based on your communication mode.
    SPI1CON1bits.MSTEN 		= 0; 	// 1 =  Master mode; 0 =  Slave mode
    SPI1CON1bits.SPRE 		= 4; 	// Secondary Prescaler = 4:1
    SPI1CON1bits.PPRE 		= 2; 	// Primary Prescaler = 4:1

    SPI1CON2 				= 0;	// non-framed mode

	SPI_SS_PORT				= 1;	//
	SPI_SS_TRIS				= 0;	// set SS as output

    SPI1STATbits.SPIEN 		= 1; 	// enable SPI port, clear status
}

unsigned short writeSPI1( unsigned short data )
{
    SPI1BUF = data;					// write to buffer for TX
    while(!SPI1STATbits.SPIRBF);	// wait for transfer to complete
    return SPI1BUF;    				// read the received value
}//writeSPI1

int main (void)
{
	unsigned short i;

	// Disable Watch Dog Timer
	RCONbits.SWDTEN = 0;
	// for LED
	ODCAbits.ODA6 = 0;
	TRISAbits.TRISA6 = 0;
        TRISAbits.TRISA0 = 0;
	SPI1Init();
    
    while (1) {
	   // for (i=0; i<0xffff; i++);	// a simple delay

		//SPI_SS_PORT	= 0;
        /*
		spiBufR[0]	= writeSPI1(spiBufT[0]);
		spiBufR[1]zr	= writeSPI1(spiBufT[1]);
		spiBufR[2]	= writeSPI1(spiBufT[2]);
		spiBufR[3]	= writeSPI1(spiBufT[3]);*/
		//SPI_SS_PORT = 1;
                PORTAbits.RA0 = 1;
                writeSPI1(0x0123);
                writeSPI1(0x4567);
                writeSPI1(0xABCD);
                writeSPI1(0x1357);
                //writeSPI1(0x5A5A);

	    __builtin_btg((unsigned int *)&LATA, 6);
	}

	return 0;
}

