#ifndef RPI_H
#define	RPI_H
#include "p24fxxxx.h"

typedef int bool;
#define true 1
#define false 0
#define USE_AND_OR	// To enable AND_OR mask setting for I2C.
#define BUF_SIZE 1200
#define CHUNK_SIZE 1200
#define SPI_SS_TRIS      TRISBbits.TRISB2
#define SPI_SS_PORT      PORTBbits.RB2

typedef struct data{
    unsigned char imu[BUF_SIZE];
    unsigned char lcd[BUF_SIZE];
}spiData;

void SPI1Init(void);
void TimerInit(void);

void _ISR _SPI1Interrupt(void);

#endif	/* RPI_H */

