#include "rpi.h"

void TimerInit(void) {
    TMR1 = 0x00;
    T1CON = 0x00;
    PR1 = 0x3E8; //without PLL FA,or 250
    IPC0bits.T1IP = 2; // interrupt priority
    IFS0bits.T1IF = 0; // reset interrupt flag
    T1CONbits.TCKPS = 1; //
    IEC0bits.T1IE = 1; // timer 1 interrupt on
    T1CONbits.TON = 1; //timer on
}

void SPI1Init(void) {
    //config SPI1
    SPI1STATbits.SPIEN = 0; // disable SPI port
    SPI1STATbits.SPISIDL = 0; // Continue module operation in Idle mode

    SPI1BUF = 0; // clear SPI buffer

    IFS0bits.SPI1IF = 0; // clear interrupt flag
    IEC0bits.SPI1IE = 1; // disable interrupt
    IPC2bits.SPI1IP = 3; // priority

    SPI1CON1bits.DISSCK = 0; // Internal SPIx clock is enabled
    SPI1CON1bits.DISSDO = 0; // SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 = 0; // set in 16-bit mode, clear in 8-bit mode
    SPI1CON1bits.SMP = 0; // Input data sampled at middle of data output time
    SPI1CON1bits.CKP = 0; // CKP and CKE is subject to change ...
    SPI1CON1bits.CKE = 0; // ... based on your communication mode.
    SPI1CON1bits.MSTEN = 0; // 1 =  Master mode; 0 =  Slave mode
    SPI1CON1bits.SPRE = 4; // Secondary Prescaler = 4:1
    SPI1CON1bits.PPRE = 2; // Primary Prescaler = 4:1

    SPI1CON2 = 0; // non-framed mode

    SPI_SS_PORT = 1; //
    SPI_SS_TRIS = 0; // set SS as output

    SPI1STATbits.SPIEN = 1; // enable SPI port, clear status
}
