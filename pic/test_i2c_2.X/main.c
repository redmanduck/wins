/**********************************************************************
* ?2008 Microchip Technology Inc.
*
* FileName:        I2C Master Lite.c
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
* Albert Z.		04/03/09	First release of source file
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
* Code Tested on:
* Explorer 16 Demo board with PIC24FJ128GA010 controller
* The Processor starts with the External Crystal.
*
*
**********************************************************************/

#include "p24fxxxx.h"

#define USE_AND_OR	// To enable AND_OR mask setting for I2C.
#include <i2c.h>

// JTAG/Code Protect/Write Protect/Clip-on Emulation mode
// Watchdog Timer/ICD pins select
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
// Disable CLK switch and CLK monitor, OSCO or Fosc/2, HS oscillator,
// Primary oscillator
_CONFIG2(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL)


// calculate baud rate of I2C
#define Fosc	(8000000) 	// crystal
#define Fcy		(Fosc*4/2)	// w.PLL (Instruction Per Second)
#define Fsck	400000		// 400kHz I2C
#define I2C_BRG	((Fcy/2/Fsck)-1)


int main (void)
{
	unsigned char SlaveAddress;
	unsigned char *pWrite, *pRead;
	unsigned int i;

	// The data to be written should be end with '\0' if using
	// char MasterputsI2C1(unsigned char *) library function.
	unsigned char tx_data[] = {'P', 'I', 'C', '2', '4', 'F', '\0'};

	unsigned char rx_data[9];	// "MCHP I2C"
	char status;

	// Disable Watch Dog Timer
	RCONbits.SWDTEN = 0;
	// for LED
	ODCAbits.ODA6 = 0;
	TRISAbits.TRISA6 = 0;

	pWrite = tx_data;
	pRead = rx_data;

	//Enable channel
	OpenI2C1( I2C_ON, I2C_BRG );

	SlaveAddress = 0x69;	/* 0b1100001 Slave MCU address (customized)
							 * 4 msb is 1100, customized as slave MCU
							 * 3 lsb is 001, customized as the first slave MCU
							 */
        //AD0 = 1 -> i2c addr = 1101001

	while (1) {
            StartI2C1();	//Send the Start Bit
            IdleI2C1();		//Wait to complete
			MasterWriteI2C1((SlaveAddress<<1)|0); //transmit write command
			IdleI2C1();		//Wait to complete
			MasterWriteI2C1(0x75); //transmit write command
			IdleI2C1();		//Wait to complete
			MasterWriteI2C1(0xF0); //transmit write command
			IdleI2C1();		//Wait to complete
                        RestartI2C1();
            StopI2C1();	//Send the Stop condition
            IdleI2C1();	//Wait to complete
            __builtin_btg((unsigned int *)&LATA, 6);

            for (i=0; i<50000; i++);
	}

	return 0;
}