#include "p24fxxxx.h"
#include "lcdPmp.h"
#include "shared.h"
#include "imu.h"
#include "fuel_gauge.h"
#include <i2c.h>

_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
// Disable CLK switch and CLK monitor, OSCO or Fosc/2, HS oscillator,
// Primary oscillator
_CONFIG2(FCKSM_CSECMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_PRIPLL)

// calculate baud rate of I2C
#define Fosc	(8000000) 	// crystal
#define Fcy		(Fosc*4/2)	// w.PLL (Instruction Per Second)
#define Fsck	400000		// 400kHz I2C
#define I2C_BRG	((Fcy/2/Fsck)-1)

enum opcode {
    ACCEL = 'A',
    GYRO = 'G',
    IMU = 'M',
    KEYPAD = 'K',
    HALT = 'H',
    ULTRASONIC = 'U',
    POSITION = 'P',
    VALID = 'V',
    ERROR = 'E',
    BAT = 'B',
};

typedef int bool;

spiData rpiData;

unsigned int accel_p = 0;           //index of buffer where the next accel data will be inserted
unsigned int gyro_p = 6;            //index of buffer where the next gyro data will be inserted
unsigned int data_p = 0;            //index of buffer that will be transmitted next to the RPi
unsigned char bat1 = 0;
unsigned char bat2 = 0;
bool _05ms;
unsigned long _temp32;

void SPI1Init(void);
void TimerInit(void);
unsigned short writeSPI1(unsigned short data);

int main(void) {
    unsigned int _60s = 0;
    // Disable Watch Dog Timer
    RCONbits.SWDTEN = 0;

    //Enable channel
    SPI1Init();
    SPI2Init();
    LCDInit();
    OpenI2C1(I2C_ON, I2C_BRG);
    OpenI2C2(I2C_ON, I2C_BRG);
    Setup_MPU6050();
    _05ms = false;
    TimerInit();

    bool accel_queue = true;

    while (1) {
        if (_60s == 20000) {
            _60s = 0;
            LDWordReadI2C(FUELGAUGE_ADDRESS, FUELGAUGE_CONFIG, &bat1, &bat2);
            LDWordReadI2C(FUELGAUGE_ADDRESS, FUELGAUGE_SOC, &bat1, &bat2);
        }
        if (_05ms == true) {
            _60s += 1;
            if (accel_queue) {
                LDByteReadI2C(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &rpiData.imu[accel_p], 6);

                if (rpiData.imu[accel_p] == 0xff && rpiData.imu[accel_p + 1] == 0xff)
                    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x00); //turn on IMU
                accel_p += 12;
                if (accel_p >= 1200)
                    accel_p = 0;
                accel_queue = false;
            } else {
                LDByteReadI2C(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_H, &rpiData.imu[gyro_p], 6);
                gyro_p += 12;
                if (gyro_p >= 1206)
                    gyro_p = 6;
                accel_queue = true;
            }
            _05ms = false;
        }
    }
    return 0;
}

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

void _ISR _T1Interrupt(void);

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void) {
    //set some flag every 0.5ms
    LATAbits.LATA4 = ~LATAbits.LATA4;
    _05ms = true;
    IFS0bits.T1IF = 0;
}

void _ISR _SPI1Interrupt(void);

void __attribute__((__interrupt__, auto_psv)) _SPI1Interrupt(void) {
    unsigned char ack = 0;
    accel_p = 0;
    gyro_p = 6;
    unsigned int i = 0;
    while (!SPI1STATbits.SPIRBF); //wait for ack from pi?
    ack = SPI1BUF;
    // need to send some data back for acknowledgment
    if (ack == ACCEL) {
        // acknowledgment from pic to pi
        SPI1BUF = ACCEL;
        while (!SPI1STATbits.SPIRBF); //wait for ack from pi?
        // will be the acknowledge from the pi
        i = SPI1BUF; //read second byte
        ack = VALID; // set ack as D
        // send first byte of data
        /*
            SPI1BUF = rpiData.imu[data_p];
            while(!SPI1STATbits.SPIRBF);
            rpiData.lcd[data_p]= SPI1BUF;
         */
        // each time read a byte and compare it with the last sent byte
        for (i = 0; i < 12; i++) {
            SPI1BUF = rpiData.imu[data_p + i];
            while (!SPI1STATbits.SPIRBF);
            rpiData.lcd[data_p + i] = SPI1BUF;
        }
        data_p += 12;
        if (data_p >= 1200)
            data_p = 0;
        // send done byte
        SPI1BUF = ack;
        while (!SPI1STATbits.SPIRBF);
        i = SPI1BUF;
    } else if (ack == BAT) {
        SPI1BUF = BAT;
        while (!SPI1STATbits.SPIRBF);
        ack = SPI1BUF;
        SPI1BUF = bat1;
        while (!SPI1STATbits.SPIRBF);
        ack = SPI1BUF;
        SPI1BUF = bat2;
        while (!SPI1STATbits.SPIRBF);
        ack = SPI1BUF;
    } else {
        SPI1BUF = ERROR;
        //need to restart the PIC or SPI 
        SPI1STATbits.SPIROV = 0;
    }
    SPI1BUF = 0x5A;
    IFS0bits.SPI1IF = 0;
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

unsigned short writeSPI1(unsigned short data) {
    SPI1BUF = data; // write to buffer for TX
    // wait for transfer to complete
    while (!SPI1STATbits.SPIRBF) {

        //check if overflow has ocurred
        if (SPI1STATbits.SPIROV) {
            SPI1STAT = 0 << 6;
        }
    }
    return SPI1BUF; // read the received value
}//writeSPI1

