// Display line length.
// number of characters displayed in a line
#define LCD_DISPLAY_LEN 	16	
	
// number of lines displayed on the LCD
#define LCD_DISPLAY_LINE	2		

#define LCD_LINE1			0		// first line
#define LCD_LINE2			1		// second line

#define LCD_I				0		// instruction
#define LCD_D				1		// data

#define LCD_Busy			0x80	// LCD is busy
#define LCD_Free			0		// LCD is free

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 128
#define LCDHEIGHT 64

#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

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


/*********************************************************************
 * Function: LCDProcessEvents
 *
 * Preconditions: None.
 *
 * Overview: 
 * This is a state mashine to issue commands and data to LCD. Must be
 * called periodically to make LCD message processing.
 *
 * Input: None.
 *
 * Output: None.
 *
 ********************************************************************/
void LCDProcessEvents(void);

/*********************************************************************
 * Function: LCDInit
 *
 * Preconditions: None.
 *
 * Overview: This is a LCD intialising routine which should be called
 * before show the characters on LCD.
 *
 * Input: None.
 *
 * Output: None.
 *
 ********************************************************************/
void LCDInit(void);


void LCDclear(void);
unsigned char LCDbusy(void);
void LCDwrite(unsigned char , unsigned char );
void LCDwriteLine(unsigned char , unsigned char * );

