#include "p24fxxxx.h"
#include <i2c.h>

_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
_CONFIG2(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL)

// calculate baud rate of I2C
#define Fosc	(8000000) 	// crystal
#define Fcy		(Fosc*4/2)	// w.PLL (Instruction Per Second)
#define Fsck	400000		// 400kHz I2C
#define I2C_BRG	((Fcy/2/Fsck)-1)

typedef int bool;
#define true 1
#define false 0
#define USE_AND_OR	// To enable AND_OR mask setting for I2C.
#define BUF_SIZE 1200
typedef struct data{
    unsigned char imu[BUF_SIZE];
    unsigned char lcd[BUF_SIZE];
}spiData;

void SPI1Init(void);
void TimerInit(void);
unsigned short writeSPI1(unsigned short data);


// LCDPMP.h
#define CMD_SET_DISP_START_LINE  0x40
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x00

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
#define CMD_SET_BIAS_9 0xA2
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF  0xAC
#define CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define CMD_SET_BOOSTER_234  0
#define CMD_SET_BOOSTER_5  1
#define CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST  0xF0

#define SPI_SS_TRIS      TRISBbits.TRISB2
#define SPI_SS_PORT      PORTBbits.RB2

void SPI2Init(void);
unsigned short writeSPI2( unsigned short data );
void LCDProcessEvents(void);
void LCDinit(void);
void LCD_comm(unsigned char c);
void LCD_data(unsigned char c);

// IMU registers
#define MPU6050_ADDRESS 0b11010010 // Address with end write bit
#define MPU6050_RA_XG_OFFS_TC 0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_YG_OFFS_TC 0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_ZG_OFFS_TC 0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_X_FINE_GAIN 0x03 //[7:0] X_FINE_GAIN
#define MPU6050_RA_Y_FINE_GAIN 0x04 //[7:0] Y_FINE_GAIN
#define MPU6050_RA_Z_FINE_GAIN 0x05 //[7:0] Z_FINE_GAIN
#define MPU6050_RA_XA_OFFS_H 0x06 //[15:0] XA_OFFS
#define MPU6050_RA_XA_OFFS_L_TC 0x07
#define MPU6050_RA_YA_OFFS_H 0x08 //[15:0] YA_OFFS
#define MPU6050_RA_YA_OFFS_L_TC 0x09
#define MPU6050_RA_ZA_OFFS_H 0x0A //[15:0] ZA_OFFS
#define MPU6050_RA_ZA_OFFS_L_TC 0x0B
#define MPU6050_RA_XG_OFFS_USRH 0x13 //[15:0] XG_OFFS_USR
#define MPU6050_RA_XG_OFFS_USRL 0x14
#define MPU6050_RA_YG_OFFS_USRH 0x15 //[15:0] YG_OFFS_USR
#define MPU6050_RA_YG_OFFS_USRL 0x16
#define MPU6050_RA_ZG_OFFS_USRH 0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_RA_ZG_OFFS_USRL 0x18
#define MPU6050_RA_SMPLRT_DIV 0x19
#define MPU6050_RA_CONFIG 0x1A
#define MPU6050_RA_GYRO_CONFIG 0x1B
#define MPU6050_RA_ACCEL_CONFIG 0x1C
#define MPU6050_RA_FF_THR 0x1D
#define MPU6050_RA_FF_DUR 0x1E
#define MPU6050_RA_MOT_THR 0x1F
#define MPU6050_RA_MOT_DUR 0x20
#define MPU6050_RA_ZRMOT_THR 0x21
#define MPU6050_RA_ZRMOT_DUR 0x22
#define MPU6050_RA_FIFO_EN 0x23
#define MPU6050_RA_I2C_MST_CTRL 0x24
#define MPU6050_RA_I2C_SLV0_ADDR 0x25
#define MPU6050_RA_I2C_SLV0_REG 0x26
#define MPU6050_RA_I2C_SLV0_CTRL 0x27
#define MPU6050_RA_I2C_SLV1_ADDR 0x28
#define MPU6050_RA_I2C_SLV1_REG 0x29
#define MPU6050_RA_I2C_SLV1_CTRL 0x2A
#define MPU6050_RA_I2C_SLV2_ADDR 0x2B
#define MPU6050_RA_I2C_SLV2_REG 0x2C
#define MPU6050_RA_I2C_SLV2_CTRL 0x2D
#define MPU6050_RA_I2C_SLV3_ADDR 0x2E
#define MPU6050_RA_I2C_SLV3_REG 0x2F
#define MPU6050_RA_I2C_SLV3_CTRL 0x30
#define MPU6050_RA_I2C_SLV4_ADDR 0x31
#define MPU6050_RA_I2C_SLV4_REG 0x32
#define MPU6050_RA_I2C_SLV4_DO 0x33
#define MPU6050_RA_I2C_SLV4_CTRL 0x34
#define MPU6050_RA_I2C_SLV4_DI 0x35
#define MPU6050_RA_I2C_MST_STATUS 0x36
#define MPU6050_RA_INT_PIN_CFG 0x37
#define MPU6050_RA_INT_ENABLE 0x38
#define MPU6050_RA_DMP_INT_STATUS 0x39
#define MPU6050_RA_INT_STATUS 0x3A
#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_ACCEL_XOUT_L 0x3C
#define MPU6050_RA_ACCEL_YOUT_H 0x3D
#define MPU6050_RA_ACCEL_YOUT_L 0x3E
#define MPU6050_RA_ACCEL_ZOUT_H 0x3F
#define MPU6050_RA_ACCEL_ZOUT_L 0x40
#define MPU6050_RA_TEMP_OUT_H 0x41
#define MPU6050_RA_TEMP_OUT_L 0x42
#define MPU6050_RA_GYRO_XOUT_H 0x43
#define MPU6050_RA_GYRO_XOUT_L 0x44
#define MPU6050_RA_GYRO_YOUT_H 0x45
#define MPU6050_RA_GYRO_YOUT_L 0x46
#define MPU6050_RA_GYRO_ZOUT_H 0x47
#define MPU6050_RA_GYRO_ZOUT_L 0x48
#define MPU6050_RA_EXT_SENS_DATA_00 0x49
#define MPU6050_RA_EXT_SENS_DATA_01 0x4A
#define MPU6050_RA_EXT_SENS_DATA_02 0x4B
#define MPU6050_RA_EXT_SENS_DATA_03 0x4C
#define MPU6050_RA_EXT_SENS_DATA_04 0x4D
#define MPU6050_RA_EXT_SENS_DATA_05 0x4E
#define MPU6050_RA_EXT_SENS_DATA_06 0x4F
#define MPU6050_RA_EXT_SENS_DATA_07 0x50
#define MPU6050_RA_EXT_SENS_DATA_08 0x51
#define MPU6050_RA_EXT_SENS_DATA_09 0x52
#define MPU6050_RA_EXT_SENS_DATA_10 0x53
#define MPU6050_RA_EXT_SENS_DATA_11 0x54
#define MPU6050_RA_EXT_SENS_DATA_12 0x55
#define MPU6050_RA_EXT_SENS_DATA_13 0x56
#define MPU6050_RA_EXT_SENS_DATA_14 0x57
#define MPU6050_RA_EXT_SENS_DATA_15 0x58
#define MPU6050_RA_EXT_SENS_DATA_16 0x59
#define MPU6050_RA_EXT_SENS_DATA_17 0x5A
#define MPU6050_RA_EXT_SENS_DATA_18 0x5B
#define MPU6050_RA_EXT_SENS_DATA_19 0x5C
#define MPU6050_RA_EXT_SENS_DATA_20 0x5D
#define MPU6050_RA_EXT_SENS_DATA_21 0x5E
#define MPU6050_RA_EXT_SENS_DATA_22 0x5F
#define MPU6050_RA_EXT_SENS_DATA_23 0x60
#define MPU6050_RA_MOT_DETECT_STATUS 0x61
#define MPU6050_RA_I2C_SLV0_DO 0x63
#define MPU6050_RA_I2C_SLV1_DO 0x64
#define MPU6050_RA_I2C_SLV2_DO 0x65
#define MPU6050_RA_I2C_SLV3_DO 0x66
#define MPU6050_RA_I2C_MST_DELAY_CTRL 0x67
#define MPU6050_RA_SIGNAL_PATH_RESET 0x68
#define MPU6050_RA_MOT_DETECT_CTRL 0x69
#define MPU6050_RA_USER_CTRL 0x6A
#define MPU6050_RA_PWR_MGMT_1 0x6B
#define MPU6050_RA_PWR_MGMT_2 0x6C
#define MPU6050_RA_BANK_SEL 0x6D
#define MPU6050_RA_MEM_START_ADDR 0x6E
#define MPU6050_RA_MEM_R_W 0x6F
#define MPU6050_RA_DMP_CFG_1 0x70
#define MPU6050_RA_DMP_CFG_2 0x71
#define MPU6050_RA_FIFO_COUNTH 0x72
#define MPU6050_RA_FIFO_COUNTL 0x73
#define MPU6050_RA_FIFO_R_W 0x74

void LDByteWriteI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char data);
void LDByteReadI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char *data, int num);
void Setup_MPU6050();

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

// FUELGAUGE.h
#define FUELGAUGE_ADDRESS 0b01101100
#define FUELGAUGE_SOC 0x04
#define FUELGAUGE_CONFIG 0x0C

void LDWordWriteI2C(unsigned char SlaveAddress, unsigned char reg, unsigned  char data1, unsigned char data2);
void LDWordReadI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char *data1, unsigned char *data2);

static spiData rpiData;

unsigned int accel_p = 0;           //index of buffer where the next accel data will be inserted
unsigned int gyro_p = 6;            //index of buffer where the next gyro data will be inserted
unsigned int data_p = 0;            //index of buffer that will be transmitted next to the RPi
unsigned char bat1 = 0;
unsigned char bat2 = 0;
bool _05ms;

unsigned long _temp32;
/*
unsigned int duc2k[2] = {0xBEEF, 0xCAFE};
static unsigned char st7565_buffer[BUF_SIZE] = {
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0x7F,0x3F,0x1F,0x0F,0x0F,0x07,0x07,0x07,0x07,0x3F,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0x03,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x0F,0x0F,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x1F,0x3F,0x3F,
0x3F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x07,0xE3,0xF3,0xF3,0xF3,
0xF3,0xC7,0x00,0x00,0xFF,0xFF,0xFF,0x03,0x03,0xFF,0xFF,0xFF,0xFF,0x03,0x03,0xFF,
0xFF,0xFF,0x03,0x01,0x00,0x00,0xF8,0xFC,0xFC,0xF8,0xF1,0xF3,0xFF,0xFF,0xFF,0xFF,
0x00,0x00,0x00,0x0F,0x0F,0x07,0xC3,0xE3,0xF1,0xF8,0xF8,0xFC,0xFE,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

0xFC,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x30,0x30,0x70,
0x60,0xC0,0x00,0x00,0x00,0x20,0x3E,0x0F,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x01,
0x03,0x03,0x03,0x03,0x03,0x03,0x07,0x07,0x0F,0x1F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xF8,0xF1,0xF3,0xF7,0xF3,
0xF3,0xF0,0xF0,0xF0,0xFF,0xFF,0xFF,0xFE,0xF8,0xF1,0xF3,0xF3,0xF3,0xF0,0xF8,0xFF,
0xFF,0xFF,0xF0,0xE0,0xE0,0xE0,0xE7,0xEF,0xEF,0xE7,0xE3,0xF3,0xFF,0xFF,0xFF,0xFF,
0xE0,0xE0,0xE0,0xFC,0xFC,0xF8,0xF8,0xF1,0xE3,0xE7,0xE7,0xEF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

0xFF,0xFF,0xFF,0x7F,0x3F,0x0E,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0x70,
0x70,0x18,0x00,0x00,0x00,0x00,0xE0,0x80,0x02,0x06,0x06,0x06,0x06,0x02,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF0,0xFC,0xFF,0xFF,0xFF,0x9F,
0x8F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x03,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x3F,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x8F,0x9F,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,

0xFF,0x3F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,
0xC0,0xE0,0x00,0x00,0x00,0x00,0x03,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,
0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x01,
0x01,0xFF,0xFF,0xFF,0x01,0x01,0xF1,0xF9,0xF9,0xF9,0x01,0x03,0xFF,0xFF,0x0F,0x03,
0xF1,0xF9,0xF9,0xF9,0xF9,0xF3,0x00,0x00,0xFF,0xFF,0x01,0x01,0xFF,0xFF,0xFF,0xFF,
0x01,0x01,0xFF,0xFF,0xF7,0xC1,0xC1,0x89,0x99,0x19,0x3F,0xFF,0xF9,0x00,0x00,0xF9,
0xFD,0xFF,0x01,0x01,0xF9,0xF9,0xFD,0xFF,0xFF,0x01,0x01,0xFF,0xFF,0x0F,0x03,0xC1,
0xD9,0xD9,0xD9,0xD9,0xC3,0xC7,0xFF,0xFF,0xE3,0xC1,0xC9,0x99,0x19,0x3B,0x7F,0xFF,

0xFF,0xFE,0xFE,0xFE,0xFC,0xFC,0xFE,0xFE,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFC,0xF8,0xF0,0xE0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xE0,0xF8,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,
0xF8,0xFF,0xFF,0xFF,0xF8,0xF8,0xFF,0xFF,0xFF,0xFF,0xF8,0xF8,0xFF,0xFF,0xFF,0xFC,
0xF8,0xF9,0xF9,0xF9,0xF9,0xFC,0xF8,0xF8,0xFF,0xFF,0xFF,0xFC,0xF8,0xF9,0xF9,0xF9,
0xF8,0xFC,0xFF,0xFF,0xFC,0xF8,0xF9,0xF9,0xF9,0xF8,0xFE,0xFF,0xFF,0xF8,0xF8,0xFF,
0xFF,0xFF,0xF8,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0xF8,0xFF,0xFF,0xFE,0xFC,0xF8,
0xF9,0xF9,0xF9,0xF9,0xFC,0xFC,0xFF,0xFF,0xFC,0xF8,0xF9,0xF9,0xF9,0xFC,0xFE,0xFF,

0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};*/
int main(void) {
    
    unsigned int _60s = 0;
    // Disable Watch Dog Timer
    RCONbits.SWDTEN = 0;
	ODCAbits.ODA6 = 0;
	TRISAbits.TRISA6 = 0;
        TRISAbits.TRISA0 = 0;
        TRISAbits.TRISA4 = 0;

        unsigned char acel = 0;
    //Enable channel
    SPI1Init();
    SPI2Init();
    LCDinit();
    OpenI2C1(I2C_ON, I2C_BRG);
    OpenI2C2(I2C_ON, I2C_BRG);
    Setup_MPU6050();
    _05ms = false;
    TimerInit();
    bool accel_queue = true;

    while (1) {
        
        if (_60s == 20000) {
            _60s = 0;
            //LDWordReadI2C(FUELGAUGE_ADDRESS, FUELGAUGE_CONFIG, &bat1, &bat2);
            //LDWordReadI2C(FUELGAUGE_ADDRESS, FUELGAUGE_SOC, &bat1, &bat2);
            // update lcd screen
        }
        if(_60s % 500 == 0){
            LCDProcessEvents();
        }
        if (_05ms == true) {
            
            _60s += 1;
            if (accel_queue) {
                LDByteReadI2C(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &rpiData.lcd[accel_p], 6);

                if (rpiData.imu[accel_p] == 0xff && rpiData.imu[accel_p + 1] == 0xff)
                    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x00); //turn on IMU
                accel_p += 12;
                if (accel_p >= BUF_SIZE)
                    accel_p = 0;
                accel_queue = false;
            } else {
                LDByteReadI2C(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_H, &rpiData.lcd[gyro_p], 6);
                gyro_p += 12;
                if (gyro_p >= BUF_SIZE)
                    gyro_p = 6;
                accel_queue = true;
            }
            _05ms = false;
            
        }
        //LCDProcessEvents();
        __builtin_btg((unsigned int *)&LATA, 6);
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
    /* will try to make the main loop based on flags
     * if the SPI1 receives a byte, need to set a flag
     * and not reset the interrupt flag until the whole transfer finishes
     * !! don't think it is feasible !!
     */
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
        // each time read a byte and compare it with the last sent byte
        for (i = 0; i < 12; i++) {
            SPI1BUF = rpiData.imu[data_p + i];
            while (!SPI1STATbits.SPIRBF);
            rpiData.lcd[data_p + i] = SPI1BUF;
        }
        data_p += 12;
        if (data_p >= BUF_SIZE)
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
}

// FUELGAUGE.c
void LDWordWriteI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char data1, unsigned char data2){
    StartI2C2();	//Send the Start Bit
    IdleI2C2();		//Wait to complete
    MasterWriteI2C2(SlaveAddress); //transmit write command
    IdleI2C2();		//Wait to complete
    MasterWriteI2C2(reg);
    IdleI2C2();
    MasterWriteI2C2(data1);
    IdleI2C2();
    MasterWriteI2C2(data2);
    IdleI2C2();
    StopI2C2();
    IdleI2C2();
}

void LDWordReadI2C(unsigned char SlaveAddress, unsigned  char reg, unsigned char *data1, unsigned char *data2){
                StartI2C2();	//Send the Start Bit
		IdleI2C2();		//Wait to complete
		MasterWriteI2C2(SlaveAddress); //transmit write command
		IdleI2C2();		//Wait to complete
                MasterWriteI2C2(reg);
                IdleI2C2();
                StopI2C2();
                IdleI2C2();


                StartI2C2();	//Send the Start Bit
		IdleI2C2();		//Wait to complete
		MasterWriteI2C2(SlaveAddress|0x01); //transmit read command
		IdleI2C2();		//Wait to complete
                unsigned char data[2];
      		MastergetsI2C2(2, data, 30);		// "MCHP I2C"
                *data1=data[0];
                *data2=data[1];
//		if (status!=0)
//			while(1);

		StopI2C2();	//Send the Stop condition
		IdleI2C2();	//Wait to complete


}

// LCDPMP.c
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
    SPI2CON1bits.SPRE 		= 7; 	// Secondary Prescaler = 4:1
    SPI2CON1bits.PPRE 		= 3; 	// Primary Prescaler = 1:1

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

void LCDinit(void){
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

void LCD_comm(unsigned char c){
    PORTAbits.RA0 = 0;
    writeSPI2(c);
}

void LCD_data(unsigned char c){
    PORTAbits.RA0 = 1;
    writeSPI2(c);
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

 // IMU.c
void LDByteWriteI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char data) {
    StartI2C1(); //Send the Start Bit
    IdleI2C1(); //Wait to complete
    MasterWriteI2C1(SlaveAddress); //transmit write command
    IdleI2C1(); //Wait to complete
    MasterWriteI2C1(reg);
    IdleI2C1();
    MasterWriteI2C1(data);
    IdleI2C1();
    StopI2C1();
    IdleI2C1();
}

void LDByteReadI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char *data, int num) {
    StartI2C1(); //Send the Start Bit
    IdleI2C1(); //Wait to complete
    MasterWriteI2C1(SlaveAddress); //transmit write command
    IdleI2C1(); //Wait to complete
    MasterWriteI2C1(reg);
    IdleI2C1();
    //StopI2C1();
    //IdleI2C1();

    StartI2C1(); //Send the Start Bit
    IdleI2C1(); //Wait to complete
    MasterWriteI2C1(SlaveAddress | 0x01); //transmit read command
    IdleI2C1(); //Wait to complete
    MastergetsI2C1(num, data, 30);
    StopI2C1(); //Send the Stop condition
    IdleI2C1(); //Wait to complete
}

void Setup_MPU6050() {
    //Sets sample rate to 8000/1+7 = 1000Hz
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_SMPLRT_DIV, 0xFF);
    //Disable FSync, 256Hz DLPF
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_CONFIG, 0x00);
    //Disable gyro self tests, scale of 500 degrees/s
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_GYRO_CONFIG, 0b00001000);
    //Disable accel self tests, scale of +-2g, no DHPF
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_ACCEL_CONFIG, 0x00);
    //Freefall threshold of |0mg|
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FF_THR, 0x00);
    //Freefall duration limit of 0
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FF_DUR, 0x00);
    //Motion threshold of 0mg
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_THR, 0x00);
    //Motion duration of 0s
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_DUR, 0x00);
    //Zero motion threshold
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_ZRMOT_THR, 0x00);
    //Zero motion duration threshold
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_ZRMOT_DUR, 0x00);
    //Disable sensor output to FIFO buffer
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FIFO_EN, 0x00);

    //AUX I2C setup
    //Sets AUX I2C to single master control, plus other config
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_CTRL, 0x00);
    //Setup AUX I2C slaves
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_ADDR, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_REG, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_CTRL, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_ADDR, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_REG, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_CTRL, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_ADDR, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_REG, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_CTRL, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_ADDR, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_REG, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_CTRL, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_ADDR, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_REG, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_DO, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_CTRL, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_DI, 0x00);

    //MPU6050_RA_I2C_MST_STATUS //Read-only
    //Setup INT pin and AUX I2C pass through
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x00);
    //Enable data ready interrupt
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_INT_ENABLE, 0x00);


    //Slave out, dont care
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_DO, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_DO, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_DO, 0x00);
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_DO, 0x00);
    //More slave config
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_DELAY_CTRL, 0x00);
    //Reset sensor signal paths
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_SIGNAL_PATH_RESET, 0x00);
    //Motion detection control
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_MOT_DETECT_CTRL, 0x00);
    //Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_USER_CTRL, 0x00);
    //Sets clock source to gyro reference w/ PLL
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0b00000010);
    //Controls frequency of wakeups in accel low power mode plus the sensor standby modes
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_2, 0x00);
    //Data transfer to and from the FIFO buffer
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_FIFO_R_W, 0x00);
    //MPU6050_RA_WHO_AM_I             //Read-only, I2C address
    LDByteWriteI2C(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x00); //turn on IMU
}

unsigned int duck[2] = {0xDEAD, 0xBEEF};