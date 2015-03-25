#include "p24fxxxx.h"
#include <i2c.h>
#include "lcdPmp.h"

#ifdef __PIC24FJ128GA010__	//Defined by MPLAB when using 24FJ256GB110 device
	// JTAG/Code Protect/Write Protect/Clip-on Emulation mode
	// Watchdog Timer/ICD pins select
	_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2)
	// Disable CLK switch and CLK monitor, OSCO or Fosc/2, HS oscillator,
	// Primary oscillator
	_CONFIG2(FCKSM_CSECMD & OSCIOFNC_ON & POSCMOD_HS & FNOSC_PRI)
#endif
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

#define USE_AND_OR	// To enable AND_OR mask setting for I2C.
// RB2 as SS for SPI slave MCU
#define SPI_SS_TRIS      TRISBbits.TRISB2
#define SPI_SS_PORT      PORTBbits.RB2


// calculate baud rate of I2C
#define Fosc	(8000000) 	// crystal
#define Fcy		(Fosc*4/2)	// w.PLL (Instruction Per Second)
#define Fsck	400000		// 400kHz I2C
#define I2C_BRG	((Fcy/2/Fsck)-1)

// Function declarations
void LDByteWriteI2C(unsigned char SlaveAddress, unsigned char data, unsigned  char reg);
void LDByteReadI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char *data, int num);
void Setup_MPU6050();
void TimerInit(void);
void SPI1Init(void);
unsigned short writeSPI1( unsigned short data );

int main (void)
{
    unsigned char accel_xh = 0x00;
    unsigned char accel_xl = 0x00;
    unsigned char accel_yh = 0x00;
    unsigned char accel_yl = 0x00;
    unsigned char accel_zh = 0x00;
    unsigned char accel_zl = 0x00;
    unsigned char gyro_xh = 0x00;
    unsigned char gyro_xl = 0x00;
    unsigned char gyro_yh = 0x00;
    unsigned char gyro_yl = 0x00;
    unsigned char gyro_zh = 0x00;
    unsigned char gyro_zl = 0x00;
    unsigned char temp_h = 0x00;
    unsigned char temp_l = 0x00;
    unsigned int i = 0;
    int g_x = 0;
    int g_y = 0;
    int g_z = 0;

    unsigned char lcd_data1[16];
    unsigned char lcd_data2[16];
    unsigned short a;
    // Disable Watch Dog Timer
    RCONbits.SWDTEN = 0;
    // for LED
    ODCAbits.ODA6 = 0;
    TRISAbits.TRISA6 = 0;
    TRISAbits.TRISA0 = 0;

    // push button
    TRISAbits.TRISA7 = 1;
    TRISDbits.TRISD6 = 1;
    TRISDbits.TRISD7 = 1;
    TRISDbits.TRISD13 = 1;

    SPI1Init();
    LCDInit();
    for(i=0; i<16;i++){
        lcd_data2[i]=i%10+'0';
        lcd_data1[i]=i%10+'0';
    }
    LCDwriteLine(LCD_LINE2, lcd_data2);
    LCDwriteLine(LCD_LINE1, lcd_data2);
    while (1) {
        if(!PORTAbits.RA7){
            LATAbits.LATA0 = 0;
            LCDwriteLine(LCD_LINE2, lcd_data2);
            LCDwriteLine(LCD_LINE1, lcd_data1);
        }
        else{
            LATAbits.LATA0 = 1;
        }
        a = writeSPI1(0x11);
        writeSPI1(a);
        a = writeSPI1(0x22);
        writeSPI1(a);
        a = writeSPI1(0x33);
        writeSPI1(a);
 	__builtin_btg((unsigned int *)&LATA, 6);
	}
	return 0;
}

void SPI1Init(void)
{
    //config SPI1
    SPI1STATbits.SPIEN 		= 0;	// disable SPI port
    SPI1STATbits.SPISIDL 	= 0; 	// Continue module operation in Idle mode

    SPI1BUF			= 0;   	// clear SPI buffer

    IFS0bits.SPI1IF 		= 0;	// clear interrupt flag
    IEC0bits.SPI1IE 		= 0;	// disable interrupt

    SPI1CON1bits.DISSCK		= 0;	// Internal SPIx clock is enabled
    SPI1CON1bits.DISSDO		= 0;	// SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 	= 0;	// set in 16-bit mode, clear in 8-bit mode
    SPI1CON1bits.SMP		= 0;	// Input data sampled at middle of data output time
    SPI1CON1bits.CKP 		= 0;	// CKP and CKE is subject to change ...
    SPI1CON1bits.CKE 		= 1;	// ... based on your communication mode.
    SPI1CON1bits.MSTEN 		= 0; 	// 1 =  Master mode; 0 =  Slave mode
    SPI1CON1bits.SPRE 		= 4; 	// Secondary Prescaler = 4:1
    SPI1CON1bits.PPRE 		= 2; 	// Primary Prescaler = 4:1

    SPI1CON2 			= 0;	// non-framed mode

    SPI_SS_PORT			= 1;	//
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