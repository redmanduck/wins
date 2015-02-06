/*
 * File:   main.c
 *
 * Created on February 9, 2010, 10:53 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p24Fxxxx.h>

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_XT & FNOSC_PRI)

/*
 *
 */
void delay(void) {
    long i = 65535;
    int j = 0;
    for(j = 0; j < 10; j++){
        i = 65535;
        while(i--);
    }
}

void shortdelay(void){
    int j = 500;
    while(j--);
}

/* 1. short trigger pulse
 * 2. 8 cycle sonic burst
 * 3. sends data back

*/

int main(int argc, char** argv) {
    int counter;
    TRISA = 0x8000;
    TRISB = 0x01;

    // Setup PortA IOs as digital
    AD1PCFG = 0xffff;

    T1CONbits.TON = 1;      //timer1 on bit
    T1CONbits.TCS = 0;

    LATBbits.LATB3 = 1;
    LATBbits.LATB2 = 0;
    LATA = 0x00;
    while(1) {
        if(PORTAbits.RA15){
            LATA = 0xFF;
        }
        else if(PORTBbits.RB0){
            LATA = 0xAA;
        }
    }
    return (EXIT_SUCCESS);
}
