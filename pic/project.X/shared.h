#ifndef MAIN_H
#define	MAIN_H

#define true 1
#define false 0
#define USE_AND_OR	// To enable AND_OR mask setting for I2C.

typedef struct data{
    unsigned char imu[1200];
    unsigned char lcd[1200];
}spiData;

extern spiData rpiData;

#endif	/* SHARED_H */

