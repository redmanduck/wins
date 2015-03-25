#include "canvas.h"

#ifndef PIBRIDGE_H
#define	PIBRIDGE_H

enum opcode {
    IMU = 'M',
    KEYPAD = 'K',
    HALT = 'H',
    ULTRASONIC='U',
    POSITION='P'
} opcode;

struct IMUdata{
    unsigned int timestamp;
    uint8_t accel[6];
    uint8_t gyro[6];
} ;

class pibridge {
public:
    pibridge();
    bool updateLCD(canvas C);
    uint8_t * recv_data(int numbyte);
    IMUdata * recv_IMU();
private:
    uint8_t * transmitSPI(char * g);
};

#endif	/* PIBRIDGE_H */

