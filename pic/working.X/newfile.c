#include "p24fxxxx.h"

#define USE_AND_OR	// To enable AND_OR mask setting for I2C.
#include <i2c.h>
#include "imu.h"
#include "rpi.h"
#include "lcd.h"

void _ISR _T1Interrupt(void);

_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
_CONFIG2(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL)

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

static spiData rpiData;

unsigned int accel_p = 0; //index of buffer where the next accel data will be inserted
unsigned int gyro_p = 6; //index of buffer where the next gyro data will be inserted
unsigned int data_p = 0; //index of buffer that will be transmitted next to the RPi
unsigned char bat1 = 0;
unsigned char bat2 = 0;
bool _05ms;
bool updatescreen = false;
unsigned long _temp32;

int main(void) {
    // Disable Watch Dog Timer
    RCONbits.SWDTEN = 0;
    // for LED
    ODCAbits.ODA6 = 0;
    TRISAbits.TRISA6 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA0 = 0;


    //Enable channel
    OpenI2C1(I2C_ON, I2C_BRG);
    Setup_MPU6050();
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x00);
    SPI1Init();
    SPI2Init();
    LCDinit();
    LCDProcessEvents();

    //_05ms = false;
    //TimerInit();
    unsigned int _1s = 0;


    while (1) {
        /*
        if(_1s == 2000){
            _1s = 0;


            // Update LCD

        }
        if(_05ms){
            _1s += 1;
            _05ms = false;
        }*/
        if(updatescreen){
            LCDProcessEvents();
            // sampling IMU for data
            LDByteReadI2C(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &rpiData.imu[accel_p], 6);
            LDByteReadI2C(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_H, &rpiData.imu[gyro_p], 6);
            accel_p+=12;
            gyro_p+=12;
            if (accel_p>=BUF_SIZE)
                accel_p=0;
            if (gyro_p>=BUF_SIZE)
                gyro_p=6;
            // END SAMPLING
            updatescreen = false;
        }
        __builtin_btg((unsigned int *) &LATA, 6);
    }
    return 0;
}

void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void) {
    //set some flag every 0.5ms
    LATAbits.LATA4 = ~LATAbits.LATA4;
    _05ms = true;
    IFS0bits.T1IF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _SPI1Interrupt(void) {
    unsigned char ack = 0;
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
        // each time read a byte and compare it with the last sent byte
        for (i = 0; i < CHUNK_SIZE; i++) {
            SPI1BUF = rpiData.imu[data_p + i];
            while (!SPI1STATbits.SPIRBF);
            rpiData.lcd[data_p + i] = SPI1BUF;
        }
        data_p += CHUNK_SIZE;
        if (data_p >= BUF_SIZE){
            data_p = 0;
            gyro_p = 6;
            accel_p = 0;
        }
        else{
            accel_p = data_p;
            gyro_p = data_p+6;
        }
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
    updatescreen = true;
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
            LCD_data(rpiData.lcd[(128*p)+c]);
        }
    }
}