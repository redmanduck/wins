#include "p24fxxxx.h"
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
#define MPU6050_RA_WHO_AM_I 0x75

#define FUELGAUGE_ADDRESS 0b01101100
#define FUELGAUGE_SOC 0x04
#define FUELGAUGE_CONFIG 0x0C

#define true 1
#define false 0
#define USE_AND_OR	// To enable AND_OR mask setting for I2C.
#include <i2c.h>
#include "lcdPmp.h"
// JTAG/Code Protect/Write Protect/Clip-on Emulation mode
// Watchdog Timer/ICD pins select
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
// Disable CLK switch and CLK monitor, OSCO or Fosc/2, HS oscillator,
// Primary oscillator
_CONFIG2(FCKSM_CSECMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_PRIPLL)


// calculate baud rate of I2C
#define Fosc	(8000000) 	// crystal
#define Fcy		(Fosc*4/2)	// w.PLL (Instruction Per Second)
#define Fsck	400000		// 400kHz I2C
#define I2C_BRG	((Fcy/2/Fsck)-1)

#define SPI_SS_TRIS      TRISBbits.TRISB2
#define SPI_SS_PORT      PORTBbits.RB2

enum opcode{
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

typedef struct data{
    unsigned char imu[1200];
    unsigned char lcd[1200];
}spiData;

unsigned char lcd_data1[16];
unsigned char lcd_data2[16];
unsigned int accel_p = 0;           //index of buffer where the next accel data will be inserted
unsigned int gyro_p = 6;            //index of buffer where the next gyro data will be inserted
unsigned int data_p = 0;            //index of buffer that will be transmitted next to the RPi
unsigned char bat1 = 0;
unsigned char bat2 = 0;
static spiData rpiData;

void LDByteWriteI2C(unsigned char SlaveAddress, unsigned char data, unsigned  char reg);
void LDByteReadI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char *data, int num);
void LDWordWriteI2C(unsigned char SlaveAddress, unsigned char reg, unsigned  char data1, unsigned char data2);
void LDWordReadI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char *data1, unsigned char *data2);
void Setup_MPU6050(void);
void SPI1Init(void);
void TimerInit(void);
unsigned short writeSPI1( unsigned short data );
bool _05ms;
unsigned long _temp32;

int main (void)
{
        //CLKDIVbits.RCDIV = 0b000; FRC clock divider
        unsigned char accel_xh = 0x00;
        unsigned char accel_xl = 0x00;
        unsigned char accel_yh = 0x00;
        unsigned char accel_yl = 0x00;
        unsigned char accel_zh = 0x00;
        unsigned char accel_zl = 0x00;
        unsigned int _60s = 0;
        int g_x=0;
        int g_y=0;
        int g_z=0;
	// Disable Watch Dog Timer
	RCONbits.SWDTEN = 0;
	// for LED
	ODCAbits.ODA6 = 0;
	TRISAbits.TRISA6 = 0;
        TRISAbits.TRISA0 = 0;
        TRISAbits.TRISA1 = 0;
        TRISAbits.TRISA5 = 0;
        //TRISAbits.TRISA2 = 0; scl2
        TRISAbits.TRISA4 = 0;
        //TRISAbits.TRISA3 = 0; sda2

        // push button
        TRISAbits.TRISA7 = 1;
        TRISDbits.TRISD6 = 1;
        TRISDbits.TRISD7 = 1;
        TRISDbits.TRISD13 = 1;


        //Enable channel
        SPI1Init();
        LCDInit();
	OpenI2C1( I2C_ON, I2C_BRG );
	OpenI2C2( I2C_ON, I2C_BRG );
        unsigned char lcd_data1[16];
        unsigned char lcd_data2[16];
        Setup_MPU6050();
        _05ms = false;
        LDByteWriteI2C(MPU6050_ADDRESS,MPU6050_RA_PWR_MGMT_1 , 0x00);   //turn on IMU
        TimerInit();

        lcd_data1[12] = ' ';
        lcd_data1[13] = ' ';
        lcd_data1[14] = ' ';
        lcd_data1[15] = ' ';

        lcd_data2[6]='y';
        lcd_data2[7]=' ';
        lcd_data2[8]=' ';
        lcd_data2[9]='z';
        bool accel_queue = true;


        while (1) {
            //LATAbits.LATA0 = 0;
            //LATAbits.LATA6 = 0;
            //LATAbits.LATA5 = 0;
            //LATAbits.LATA1 = 0;
            if (_60s==20000){
                _60s = 0;
                LDWordReadI2C(FUELGAUGE_ADDRESS, FUELGAUGE_CONFIG, &bat1, &bat2);
                LDWordReadI2C(FUELGAUGE_ADDRESS, FUELGAUGE_SOC, &bat1, &bat2);
            }
            if(_05ms==true){
                //do something
                _60s+=1;
                if(accel_queue){
                    LDByteReadI2C(MPU6050_ADDRESS,MPU6050_RA_ACCEL_XOUT_H , &rpiData.imu[accel_p], 6);

                    if(rpiData.imu[accel_p]==0xff && rpiData.imu[accel_p+1]==0xff)
                        LDByteWriteI2C(MPU6050_ADDRESS,MPU6050_RA_PWR_MGMT_1 , 0x00);   //turn on IMU

                accel_xh=rpiData.imu[accel_p];
                accel_xl=rpiData.imu[accel_p+1];
                accel_yh=rpiData.imu[accel_p+2];
                accel_yl=rpiData.imu[accel_p+3];
                accel_zh=rpiData.imu[accel_p+4];
                accel_zl=rpiData.imu[accel_p+5];
                    accel_p+=12;
                    if (accel_p>=1200)
                        accel_p=0;
                    lcd_data1[7] = 'A';
                    lcd_data1[8] = 'C';
                    lcd_data1[9] = 'C';
                    lcd_data1[10] = 'E';
                    lcd_data1[11] = 'L';
                    accel_queue=false;
                }
                else{
                    LDByteReadI2C(MPU6050_ADDRESS,MPU6050_RA_GYRO_XOUT_H , &rpiData.imu[gyro_p], 6);

                accel_xh=rpiData.imu[gyro_p];
                accel_xl=rpiData.imu[gyro_p+1];
                accel_yh=rpiData.imu[gyro_p+2];
                accel_yl=rpiData.imu[gyro_p+3];
                accel_zh=rpiData.imu[gyro_p+4];
                accel_zl=rpiData.imu[gyro_p+5];
                    gyro_p+=12;
                    if (gyro_p>=1206)
                        gyro_p=6;

                    lcd_data1[7] = 'G';
                    lcd_data1[8] = 'Y';
                    lcd_data1[9] = 'R';
                    lcd_data1[10] = 'O';
                    lcd_data1[11] = ' ';
                    accel_queue=true;
                }

                LCDwriteLine(LCD_LINE1, lcd_data1);
                LCDwriteLine(LCD_LINE2, lcd_data2);
                _05ms=false;
                g_x=accel_xl|accel_xh<<8;
                lcd_data1[0]=g_x < 0? '-' : ' ';
                g_x=g_x > 0 ? g_x : -g_x;

                g_y=accel_yl|accel_yh<<8;
                lcd_data2[0]=g_y < 0? '-' : ' ';
                g_y=g_y > 0 ? g_y : -g_y;

                g_z=accel_zl|accel_zh<<8;
                lcd_data2[10]=g_z < 0? '-' : ' ';
                g_z=g_z > 0 ? g_z : -g_z;
                lcd_data1[5]=(g_x%10)+'0';
                lcd_data1[4]=(g_x/10)%10+'0';
                lcd_data1[3]=(g_x/100)%10+'0';
                lcd_data1[2]=(g_x/1000)%10+'0';
                lcd_data1[1]=(g_x/10000)%10+'0';

                lcd_data2[5]=(g_y%10)+'0';
                lcd_data2[4]=((g_y/10)%10)+'0';
                lcd_data2[3]=((g_y/100)%10)+'0';
                lcd_data2[2]=(g_y/1000)%10+'0';
                lcd_data2[1]=(g_y/10000)%10+'0';

                lcd_data2[15]=(g_z%10)+'0';
                lcd_data2[14]=(g_z/10)%10+'0';
                lcd_data2[13]=(g_z/100)%10+'0';
                lcd_data2[12]=(g_z/1000)%10+'0';
                lcd_data2[11]=(g_z/10000)%10+'0';

            }
      }
	return 0;
}

void TimerInit(void){
    TMR1 = 0x00;
    T1CON = 0x00;
    PR1 = 0x3E8;        //without PLL FA,or 250
    IPC0bits.T1IP = 2;  // interrupt priority
    IFS0bits.T1IF = 0;  // reset interrupt flag
    T1CONbits.TCKPS = 1; //
    IEC0bits.T1IE = 1;  // timer 1 interrupt on
    T1CONbits.TON = 1;  //timer on
}

void _ISR _T1Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void){
    //set some flag every 0.5ms
    LATAbits.LATA4 = ~LATAbits.LATA4;
    _05ms = true;
    IFS0bits.T1IF = 0;
}

void _ISR _SPI1Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _SPI1Interrupt(void){
    unsigned char ack = 0;
    accel_p = 0;
    gyro_p = 6;
    unsigned int i = 0;
        while(!SPI1STATbits.SPIRBF); //wait for ack from pi?
    ack = SPI1BUF;
    // need to send some data back for acknowledgment
    if(ack==ACCEL){
        // acknowledgment from pic to pi
        SPI1BUF=ACCEL;
        while(!SPI1STATbits.SPIRBF); //wait for ack from pi?
        // will be the acknowledge from the pi
        i=SPI1BUF;                  //read second byte
        ack=VALID;                    // set ack as D
            // send first byte of data
        /*
            SPI1BUF = rpiData.imu[data_p];
            while(!SPI1STATbits.SPIRBF);
            rpiData.lcd[data_p]= SPI1BUF;
         */
            // each time read a byte and compare it with the last sent byte
        for(i=0; i<12; i++){
            SPI1BUF = rpiData.imu[data_p+i];
            while(!SPI1STATbits.SPIRBF);
            rpiData.lcd[data_p+i]= SPI1BUF;
        }
        data_p+=12;
        if(data_p>=1200)
            data_p=0;
        // send done byte
        SPI1BUF=ack;
        while(!SPI1STATbits.SPIRBF);
        i=SPI1BUF;
    }
    else if (ack==BAT){
        SPI1BUF = BAT;
        while(!SPI1STATbits.SPIRBF);
        ack=SPI1BUF;
        SPI1BUF=bat1;
        while(!SPI1STATbits.SPIRBF);
        ack=SPI1BUF;
        SPI1BUF=bat2;
        while(!SPI1STATbits.SPIRBF);
        ack=SPI1BUF;
    }
    else{
        SPI1BUF=ERROR;
        //need to restart the PIC or SPI
        SPI1STATbits.SPIROV = 0;
    }
    SPI1BUF = 0x5A;
    IFS0bits.SPI1IF = 0;
}

void SPI1Init(void)
{
    //config SPI1
    SPI1STATbits.SPIEN 		= 0;	// disable SPI port
    SPI1STATbits.SPISIDL 	= 0; 	// Continue module operation in Idle mode

    SPI1BUF 			= 0;   	// clear SPI buffer

    IFS0bits.SPI1IF 		= 0;	// clear interrupt flag
    IEC0bits.SPI1IE 		= 1;	// disable interrupt
    IPC2bits.SPI1IP             = 3;    // priority

    SPI1CON1bits.DISSCK		= 0;	// Internal SPIx clock is enabled
    SPI1CON1bits.DISSDO		= 0;	// SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 	= 0;	// set in 16-bit mode, clear in 8-bit mode
    SPI1CON1bits.SMP		= 0;	// Input data sampled at middle of data output time
    SPI1CON1bits.CKP 		= 0;	// CKP and CKE is subject to change ...
    SPI1CON1bits.CKE 		= 0;	// ... based on your communication mode.
    SPI1CON1bits.MSTEN 		= 0; 	// 1 =  Master mode; 0 =  Slave mode
    SPI1CON1bits.SPRE 		= 4; 	// Secondary Prescaler = 4:1
    SPI1CON1bits.PPRE 		= 2; 	// Primary Prescaler = 4:1

    SPI1CON2 			= 0;	// non-framed mode

    SPI_SS_PORT 		= 1;	//
    SPI_SS_TRIS			= 0;	// set SS as output

    SPI1STATbits.SPIEN 		= 1; 	// enable SPI port, clear status
}

unsigned short writeSPI1( unsigned short data )
{
    SPI1BUF = data;		// write to buffer for TX
    // wait for transfer to complete
    while(!SPI1STATbits.SPIRBF){

        //check if overflow has ocurred
        if(SPI1STATbits.SPIROV){
            SPI1STAT = 0<<6;
        }

    }
    return SPI1BUF;    				// read the received value
}//writeSPI1

void LDByteWriteI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char data){
    StartI2C1();	//Send the Start Bit
    IdleI2C1();		//Wait to complete
    MasterWriteI2C1(SlaveAddress); //transmit write command
    IdleI2C1();		//Wait to complete
    MasterWriteI2C1(reg);
    IdleI2C1();
    MasterWriteI2C1(data);
    IdleI2C1();
    StopI2C1();
    IdleI2C1();
}

void LDByteReadI2C(unsigned char SlaveAddress, unsigned  char reg, unsigned char *data, int num){
                StartI2C1();	//Send the Start Bit
		IdleI2C1();		//Wait to complete
		MasterWriteI2C1(SlaveAddress); //transmit write command
		IdleI2C1();		//Wait to complete
                MasterWriteI2C1(reg);
                IdleI2C1();
                StopI2C1();
                IdleI2C1();
                StartI2C1();	//Send the Start Bit
		IdleI2C1();		//Wait to complete
		MasterWriteI2C1(SlaveAddress|0x01); //transmit read command
		IdleI2C1();		//Wait to complete
      		MastergetsI2C1(num, data, 30);
		StopI2C1();	//Send the Stop condition
		IdleI2C1();	//Wait to complete
}

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

void Setup_MPU6050()
{
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
}