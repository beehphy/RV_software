// RV led controller 
// 2/23/15
//#define F_CPU 16000000UL

//#include <eeprom.h>
#include <Arduino.h>
#include <Encoder.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <string.h>

//#define LCD_RESOLUTION 12864

// #define HW 328 //WS_STD 0V2
#define HW 329 //pnp feeder HW
// #define HW 2560 //final 2560 HW

#if (HW == 328) //hardware definitions
	#define LEDS 2
	#define PIN_RED1 1
	#define PIN_GREEN1 2
	#define PIN_BLUE1 3
	#define PIN_RED2 4
	#define PIN_GREEN2 6
	#define PIN_BLUE2 7
	#define PUSH_BUTTON A3
	#define ENCODER_A A2
	#define ENCODER_B A1
	#define OLED_R A0
	#define SA0 A4
	#define LCD_RESOLUTION 9616

#elif (HW == 329) 
	#define LEDS 2
	#define PIN_RED1 1
	#define PIN_GREEN1 2
	#define PIN_BLUE1 3
	#define PIN_RED2 4
	#define PIN_GREEN2 6
	#define PIN_BLUE2 7
	#define PUSH_BUTTON A2
	#define ENCODER_A A0
	#define ENCODER_B A1
	#define OLED_R A3
	#define SA0 A4
	#define LCD_RESOLUTION 12864

#elif (HW == 2560)
	#define LEDS 4
	#define PIN_RED1 1
	#define PIN_GREEN1 2
	#define PIN_BLUE1 3
	#define PIN_RED2 4
	#define PIN_GREEN2 6
	#define PIN_BLUE2 7
	#define PIN_RED3 8
	#define PIN_GREEN3 9
	#define PIN_BLUE3 10
	#define PIN_RED4 11
	#define PIN_GREEN4 12
	#define PIN_BLUE4 13
	#define ENCODER_A A8
	#define ENCODER_B A9
	#define PUSH_BUTTON A10
	#define OLED_R A11
	#define SA0 A12 
	#define LCD_RESOLUTION 12832
#endif

Encoder knob(ENCODER_A, ENCODER_B);
#define MENU_TIMEOUT 1000 //10 per socond
#define DIAL_DETENT 2 //4 for blue type knob, 2 for green type knob

#if(1) // Oled generic config defs, if'd to hide in IDE
	#define BOOL uint8_t
	#define OLED_ADDRESS    0x78
	#define OLED_READ       OLED_ADDRESS+1
	#define OLED_RUN        0xa4
	#define OLED_OFF        0xa5
	#define OLED_INVERSE    0xa7
	#define OLED_NORMAL     0xa6
	#define OLED_SLEEP      0xae
	#define OLED_ACTIVE     0xaf
	#define OLED_SCROLL_OFF 0x2e
	#define OLED_DATA       0
	#define COMMAND_COMMAND 0x80
	#define DATA_COMMAND    0x40
	#define CURSOR_COMMAND  0x00 // To make a blinking cursor, like data, but doesn't move column
	#define HIGH_COL_START_ADDR 0x10
	#define LOW_COL_START_ADDR  0x00
	#define SEGMENT_REMAP      0xA1
	#define DISPLAY_START_LINE 0x40
	#define SMALL_FONT_CHARACTER_W 5
	#define SMALL_FONT_CHARACTER_H 8
	#define H_PADDING 1
	#define ASCII_OFFSET_LOW  0x20
	#define ASCII_OFFSET_HIGH 0x7e
#endif
#if (LCD_RESOLUTION == 12864)
	#define LCD_HEIGHT         64
	#define LCD_WIDTH          128
	#define MAX_ROWS           8
	#define MAX_COLS           21
	#define MULTIPLEX_RATIO    0x3F
	#define COM_PIN_RATIO      0x12
	#define PRE_CHARGE_PERIOD  0xF1
	#define CLOCK_DIVIDE_RATIO 0x80
	#define CONTRAST_LEVEL	   0x80
	#define VCOM_DETECT		   0x20
#elif (LCD_RESOLUTION == 12832)
	#define LCD_HEIGHT         32
	#define LCD_WIDTH          128
	#define MAX_ROWS           2
	#define MAX_COLS		   21
	#define MULTIPLEX_RATIO    0x1F
	#define COM_PIN_RATIO      0x12
	#define PRE_CHARGE_PERIOD  0xd2
	#define CLOCK_DIVIDE_RATIO 0xa0
	#define CONTRAST_LEVEL	   0x80
	#define VCOM_DETECT		   0x34
#elif (LCD_RESOLUTION == 9616)
	#define LCD_HEIGHT         16
	#define LCD_WIDTH          96
	#define MAX_ROWS           2
	#define MAX_COLS		   16
	#define MULTIPLEX_RATIO    0x0F
	#define COM_PIN_RATIO      0x02
	#define PRE_CHARGE_PERIOD  0x22
	#define CLOCK_DIVIDE_RATIO 0xF0
	#define CONTRAST_LEVEL	   0x80
	#define VCOM_DETECT		   0x49
#endif
static uint8_t PROGMEM font5x8[][5] = {// The 7-bit ASCII character set...
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },  // 20 Space
	{ 0x00, 0x00, 0x5f, 0x00, 0x00 },  // 21 !
	{ 0x00, 0x07, 0x00, 0x07, 0x00 },  // 22 "
	{ 0x14, 0x7f, 0x14, 0x7f, 0x14 },  // 23 #
	{ 0x24, 0x2a, 0x7f, 0x2a, 0x12 },  // 24 $
	{ 0x23, 0x13, 0x08, 0x64, 0x62 },  // 25 %
	{ 0x36, 0x49, 0x55, 0x22, 0x50 },  // 26 &
	{ 0x00, 0x05, 0x03, 0x00, 0x00 },  // 27 '
	{ 0x00, 0x1c, 0x22, 0x41, 0x00 },  // 28 (
	{ 0x00, 0x41, 0x22, 0x1c, 0x00 },  // 29 )
	{ 0x14, 0x08, 0x3e, 0x08, 0x14 },  // 2a *
	{ 0x08, 0x08, 0x3e, 0x08, 0x08 },  // 2b +
	{ 0x00, 0x50, 0x30, 0x00, 0x00 },  // 2c ,
	{ 0x08, 0x08, 0x08, 0x08, 0x08 },  // 2d -
	{ 0x00, 0x60, 0x60, 0x00, 0x00 },  // 2e .
	{ 0x20, 0x10, 0x08, 0x04, 0x02 },  // 2f /
	{ 0x3e, 0x51, 0x49, 0x45, 0x3e },  // 30 0
	{ 0x00, 0x42, 0x7f, 0x40, 0x00 },  // 31 1
	{ 0x42, 0x61, 0x51, 0x49, 0x46 },  // 32 2
	{ 0x21, 0x41, 0x45, 0x4b, 0x31 },  // 33 3
	{ 0x18, 0x14, 0x12, 0x7f, 0x10 },  // 34 4
	{ 0x27, 0x45, 0x45, 0x45, 0x39 },  // 35 5
	{ 0x3c, 0x4a, 0x49, 0x49, 0x30 },  // 36 6
	{ 0x01, 0x71, 0x09, 0x05, 0x03 },  // 37 7
	{ 0x36, 0x49, 0x49, 0x49, 0x36 },  // 38 8
	{ 0x06, 0x49, 0x49, 0x29, 0x1e },  // 39 9
	{ 0x00, 0x36, 0x36, 0x00, 0x00 },  // 3a :
	{ 0x00, 0x56, 0x36, 0x00, 0x00 },  // 3b ;
	{ 0x08, 0x14, 0x22, 0x41, 0x00 },  // 3c <
	{ 0x14, 0x14, 0x14, 0x14, 0x14 },  // 3d =
	{ 0x00, 0x41, 0x22, 0x14, 0x08 },  // 3e >
	{ 0x02, 0x01, 0x51, 0x09, 0x06 },  // 3f ?
	{ 0x32, 0x49, 0x79, 0x41, 0x3e },  // 40 @
	{ 0x7e, 0x11, 0x11, 0x11, 0x7e },  // 41 A
	{ 0x7f, 0x49, 0x49, 0x49, 0x36 },  // 42 B
	{ 0x3e, 0x41, 0x41, 0x41, 0x22 },  // 43 C
	{ 0x7f, 0x41, 0x41, 0x22, 0x1c },  // 44 D
	{ 0x7f, 0x49, 0x49, 0x49, 0x41 },  // 45 E
	{ 0x7f, 0x09, 0x09, 0x09, 0x01 },  // 46 F
	{ 0x3e, 0x41, 0x49, 0x49, 0x7a },  // 47 G
	{ 0x7f, 0x08, 0x08, 0x08, 0x7f },  // 48 H
	{ 0x00, 0x41, 0x7f, 0x41, 0x00 },  // 49 I
	{ 0x20, 0x40, 0x41, 0x3f, 0x01 },  // 4a J
	{ 0x7f, 0x08, 0x14, 0x22, 0x41 },  // 4b K
	{ 0x7f, 0x40, 0x40, 0x40, 0x40 },  // 4c L
	{ 0x7f, 0x02, 0x0c, 0x02, 0x7f },  // 4d M
	{ 0x7f, 0x04, 0x08, 0x10, 0x7f },  // 4e N
	{ 0x3e, 0x41, 0x41, 0x41, 0x3e },  // 4f O
	{ 0x7f, 0x09, 0x09, 0x09, 0x06 },  // 50 P
	{ 0x3e, 0x41, 0x51, 0x21, 0x5e },  // 51 Q
	{ 0x7f, 0x09, 0x19, 0x29, 0x46 },  // 52 R
	{ 0x46, 0x49, 0x49, 0x49, 0x31 },  // 53 S
	{ 0x01, 0x01, 0x7f, 0x01, 0x01 },  // 54 T
	{ 0x3f, 0x40, 0x40, 0x40, 0x3f },  // 55 U
	{ 0x1f, 0x20, 0x40, 0x20, 0x1f },  // 56 V
	{ 0x3f, 0x40, 0x38, 0x40, 0x3f },  // 57 W
	{ 0x63, 0x14, 0x08, 0x14, 0x63 },  // 58 X
	{ 0x07, 0x08, 0x70, 0x08, 0x07 },  // 59 Y
	{ 0x61, 0x51, 0x49, 0x45, 0x43 },  // 5a Z
	{ 0x00, 0x7f, 0x41, 0x41, 0x00 },  // 5b [
	{ 0x02, 0x04, 0x08, 0x10, 0x20 },  // 5c backslash
	{ 0x00, 0x41, 0x41, 0x7f, 0x00 },  // 5d ]
	{ 0x04, 0x02, 0x01, 0x02, 0x04 },  // 5e ^
	{ 0x40, 0x40, 0x40, 0x40, 0x40 },  // 5f _
	{ 0x00, 0x01, 0x02, 0x04, 0x00 },  // 60 `
	{ 0x20, 0x54, 0x54, 0x54, 0x78 },  // 61 a
	{ 0x7f, 0x48, 0x44, 0x44, 0x38 },  // 62 b
	{ 0x38, 0x44, 0x44, 0x44, 0x20 },  // 63 c
	{ 0x38, 0x44, 0x44, 0x48, 0x7f },  // 64 d
	{ 0x38, 0x54, 0x54, 0x54, 0x18 },  // 65 e
	{ 0x08, 0x7e, 0x09, 0x01, 0x02 },  // 66 f
	{ 0x0c, 0x52, 0x52, 0x52, 0x3e },  // 67 g
	{ 0x7f, 0x08, 0x04, 0x04, 0x78 },  // 68 h
	{ 0x00, 0x44, 0x7d, 0x40, 0x00 },  // 69 i
	{ 0x20, 0x40, 0x44, 0x3d, 0x00 },  // 6a j
	{ 0x7f, 0x10, 0x28, 0x44, 0x00 },  // 6b k
	{ 0x00, 0x41, 0x7f, 0x40, 0x00 },  // 6c l
	{ 0x7c, 0x04, 0x18, 0x04, 0x78 },  // 6d m
	{ 0x7c, 0x08, 0x04, 0x04, 0x78 },  // 6e n
	{ 0x38, 0x44, 0x44, 0x44, 0x38 },  // 6f o
	{ 0x7c, 0x14, 0x14, 0x14, 0x08 },  // 70 p
	{ 0x08, 0x14, 0x14, 0x18, 0x7c },  // 71 q
	{ 0x7c, 0x08, 0x04, 0x04, 0x08 },  // 72 r
	{ 0x48, 0x54, 0x54, 0x54, 0x20 },  // 73 s
	{ 0x04, 0x3f, 0x44, 0x40, 0x20 },  // 74 t
	{ 0x3c, 0x40, 0x40, 0x20, 0x7c },  // 75 u
	{ 0x1c, 0x20, 0x40, 0x20, 0x1c },  // 76 v
	{ 0x3c, 0x40, 0x30, 0x40, 0x3c },  // 77 w
	{ 0x44, 0x28, 0x10, 0x28, 0x44 },  // 78 x
	{ 0x0c, 0x50, 0x50, 0x50, 0x3c },  // 79 y
	{ 0x44, 0x64, 0x54, 0x4c, 0x44 },  // 7a z
	{ 0x00, 0x08, 0x36, 0x41, 0x00 },  // 7b open curl
	{ 0x00, 0x00, 0x7f, 0x00, 0x00 },  // 7c |
	{ 0x00, 0x41, 0x36, 0x08, 0x00 },  // 7d close curl
	{ 0x10, 0x08, 0x08, 0x10, 0x08 }  // 7e ~
};
static uint8_t PROGMEM unknown5x8[5] = { 0x7f, 0x41, 0x41, 0x41, 0x7f }; // unknown char
static uint8_t PROGMEM custom5x8[][5] = { // A place for custom characters
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },  // Space
	{ 0x7f, 0x41, 0x41, 0x41, 0x7f }   // ?
};

uint8_t   lcdRow      = 0;
uint8_t   lcdColumn   = 0;
void twiInit(void)
{
	TWSR = 0x00; //clears the prescaler (twps0 twps1) bits for F set
	TWBR = 0x10; //set SCL to 400kHz
	TWCR = ((1<<TWINT) | (1<<TWEN)); //enable TWI should configure pins, reset flags
}
void twiStart(void){	
	TWCR = ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN));	//send start condition 
	while ((TWCR & (1<<TWINT)) == 0); //wait for start condition to ack
}
void twiStop(void)
{
#define TWI_STOP_DELAY 40
	TWCR = ((1<<TWINT) | (1<<TWEN) | (1<<TWSTO)); //send stop condition
//	for(uint8_t ct = 0; ct < 40; ct++); //wait a moment in case a start immediately follows
	uint8_t cnt = 0;
	while (cnt < TWI_STOP_DELAY)
	{
		cnt++;
	}
}
void twiSend(uint8_t u8data)
{
	TWDR = u8data; //data to dat register
	TWCR = ((1<<TWINT)|(1<<TWEN)); //set TWI byte ready to go
	while ((TWCR & (1<<TWINT)) == 0); //wait for data to send
}
void twiSendCmd(uint8_t command)
{
	twiSend(COMMAND_COMMAND);   // command command
	twiSend(command);// command value
}

void lcdSendCommand(uint8_t command)
{
	twiStart();
	twiSend(OLED_ADDRESS);
	twiSendCmd(command);
	twiStop();
}
void lcdInit(void)
{ 	
	twiInit();
	
	//*********** 96x16 / 128x64 ****************
	twiStart();
	twiSend(OLED_ADDRESS);
	twiSendCmd(OLED_SLEEP); // 0xae display off
	twiSendCmd(0xd5);       // set display clock divide ratio
		twiSendCmd(CLOCK_DIVIDE_RATIO);   // set ratio 128x64:80 96x16:f0
	twiSendCmd(0xa8);       // set multiplex ratio (screen lines 1 to 64)
		twiSendCmd(MULTIPLEX_RATIO); // set 16:0x0f 64:0x3f
	twiSendCmd(0xd3);       // set display offset
		twiSendCmd(0);      // not offset
	twiSendCmd(DISPLAY_START_LINE); // set display start line
#if (LCD_RESOLUTION == 12832)
	twiSendCmd(0xad);       // set master configuration
		twiSendCmd(0x8e);   // 
	twiSendCmd(0xd8);       // Set Area Color Mode On/Off & Low Power Display Mode
		twiSendCmd(0x05);   // 
	twiSendCmd(0x91); //Set current drive pulse width of BANK0, Color A, Band C.
		twiSendCmd(0x3f);
		twiSendCmd(0x3f);
		twiSendCmd(0x3f);
		twiSendCmd(0x3f);
#elif (LCD_RESOLUTION == 12864 || LCD_RESOLUTION == 9616)
	twiSendCmd(0x8d);       // charge pump control
		twiSendCmd(0x14);   // 0x14:Run 0x10:off
#endif
	twiSendCmd(OLED_NORMAL);
	twiSendCmd(OLED_RUN);
	twiSendCmd(SEGMENT_REMAP); // --set segment re-map 96 to 1
	twiSendCmd(0xC8);       // --Set COM Output Scan Direction 16 to 1
	twiSendCmd(0xda);       // --set com pins hardware configuration
		twiSendCmd(COM_PIN_RATIO); // set ratio 128x64:0x12 96x16:0x02
	twiSendCmd(0x81);       // --set contrast control register
		twiSendCmd(CONTRAST_LEVEL);
	twiSendCmd(0xd9);       // --set pre-charge period
		twiSendCmd(PRE_CHARGE_PERIOD);   // set ratio 128x64:0xf1 96x16:0x22
	twiSendCmd(0xdb);       // --set vcomh
		twiSendCmd(VCOM_DETECT); // --0.77vref
	twiSendCmd(0x20);       // Set Memory Addressing Mode
		twiSendCmd(0x00);   // 00, Horizontal Addressing Mode; 01, Vertical Addressing Mode; 10, Page Addressing Mode (RESET); 11, Invalid
	twiSendCmd(OLED_ACTIVE); // --turn on oled panel
	
	twiStop();
}
#define MIN(x, y) x < y ? x : y;
#define MAX(x, y) x > y ? x : y;
void lcdSetPos(uint8_t row, uint8_t column)
{
	row = MIN(row, MAX_ROWS);
	column = MIN(column, LCD_WIDTH);
	
	twiStart();
	twiSend(OLED_ADDRESS);
	twiSendCmd(0xb0+row);
	twiSendCmd(((column&0xf0)>>4) | HIGH_COL_START_ADDR); // high column start address or'd 0x10
	twiSendCmd((column&0x0f) | LOW_COL_START_ADDR);       // low column start address
	twiStop();
	
	lcdRow = row;	//save cursor location
	lcdColumn = column;
}
void lcdFill(char fillData)
{
	uint8_t m,n;
	for(m = 0; m <= MAX_ROWS; m++) {
		//twiStart();
		//twiSend(OLED_ADDRESS);
		//twiSendCmd(0xb0 + m);            // page0-page1
		//twiSendCmd(HIGH_COL_START_ADDR); // high column start address
		//twiSendCmd(LOW_COL_START_ADDR);  // low column start address
		//twiStop();
		
		lcdSetPos(m, 0);
		
		twiStart();
		twiSend(OLED_ADDRESS);
		twiSend(DATA_COMMAND);
		for(n = 0; n < LCD_WIDTH; n++) {
			twiSend(fillData);
		}
		twiStop();
	}
}
void lcdClearLine(uint8_t line)
{
	if (line >= MAX_ROWS) return;
	lcdSetPos(line, 0);		
	twiStart();
	twiSend(OLED_ADDRESS);
	twiSend(DATA_COMMAND);
// 	for(int n = 0; n < LCD_WIDTH; n++) {
	uint8_t n = 0;
	while (n < LCD_WIDTH)
	{
		twiSend(0);
		n++;
	}
	twiStop();
	lcdSetPos(line, 0);
}
void lcdClearScreen()
{
	lcdFill(0);
	lcdSetPos(0, 0);
}
void lcdWriteChar(uint8_t data)
{	
// 	twiStart(); // maybe move these later to lcdprint, etc.
// 	twiSend(OLED_ADDRESS); // maybe move these later to lcdprint, etc.
// 	twiSend(DATA_COMMAND); // maybe move these later to lcdprint, etc.
	
	if (data >= ASCII_OFFSET_LOW && data <= ASCII_OFFSET_HIGH) { //is the data within the charecter set?
		data -= ASCII_OFFSET_LOW; // offsets byte by 0x20, to beginning of ascii codes in memory
		//for (uint8_t i = 0; i < SMALL_FONT_CHARACTER_W; i++) {
		uint8_t i = 0;
		while (i < SMALL_FONT_CHARACTER_W)
		{
			twiSend(pgm_read_byte(&font5x8[data][i])); // cycle through and send 5 bytes of char
			i++;
		}
		twiSend(0); // pad 6th byte because separation between char
		
		lcdColumn += SMALL_FONT_CHARACTER_W + H_PADDING;
	} 
	else if (data == '\n') {
		lcdRow++;
		lcdColumn = 0;
	}
	else { // send space for unknown char
		//for (uint8_t i = 0; i < SMALL_FONT_CHARACTER_W; i++) {
		uint8_t i = 0;
		while (i < SMALL_FONT_CHARACTER_W)
		{
				twiSend(pgm_read_byte(&unknown5x8[i])); // cycle through and send 5 bytes of the unknown char
				i++;
		}
		twiSend(0); // pad 6th byte because separation between char
	
		lcdColumn += SMALL_FONT_CHARACTER_W + H_PADDING;
	}
//	twiStop();
}
void lcdPrint(char *buffer)
{
	twiStart();
	twiSend(OLED_ADDRESS);
	twiSend(DATA_COMMAND);
	
	uint8_t i = 0;
	uint8_t len = strlen(buffer);
	
	while (i < len) { 
		lcdWriteChar(buffer[i]); 
		i++; 
	}
	twiStop();
	
	if (lcdColumn >= LCD_WIDTH) {
		lcdColumn = 0;
		lcdRow++;
		lcdSetPos(lcdRow, lcdColumn);
	}
	
	if (lcdRow >= MAX_ROWS) {
		lcdColumn = 0;
		lcdRow = 0;
		lcdSetPos(lcdRow, lcdColumn);
	}
}
void lcdPrintln(char *buffer)
{
	lcdPrint(buffer);
	lcdWriteChar('\n');
}

typedef struct {
	int red;
	int green;
	int blue;
	uint8_t	redPin;
	uint8_t greenPin;
	uint8_t bluePin;
}ledOutput;
typedef struct {
	uint8_t g;
	uint8_t r;
	uint8_t b;
}rgb;
typedef struct {
	uint8_t h;
	uint8_t s;
	uint8_t v;
}hsv;
typedef struct {
	uint8_t x;
	uint8_t y;
}coordinate;
typedef struct {
	int time;
	int midnight;
	int rate;
}clock;
typedef struct {
	int period;
	int rise;
	int width;
	int fall;
	int count;
} wave;
	
ledOutput leds[5]; //0 is temp
char mainMode;
char patternMode;
char colorMode;
char speedMode;
char brightness;

#define MENU_TITLE 0
#define MENU_START 1
#define MENU_SIZE 255

// Button routines
void buttonDebounce()
{
	#define DEBOUNCE_LOOPS 20000; //approximately the microsecond delay after button is released
	int x = DEBOUNCE_LOOPS;  //start countdown
	while(x > 0)
	{
		if (digitalRead(PUSH_BUTTON)) {x--;} //button press is detected as low, count down while high.
		else {x = DEBOUNCE_LOOPS;}	//restart the timer, a bounce was detected.
	}
}
// Display and Menu routines
void showMenu  (char* (*useMenu)(uint8_t), uint8_t pos)
{
	lcdClearLine(1);
	lcdSetPos(1, 0);
 	lcdPrint(useMenu(pos));
}
void showTitle (char* (*useMenu)(uint8_t), uint8_t pos)
{
	lcdClearLine(0);
	lcdSetPos(0, 0);
	lcdPrint(useMenu(pos));
}
int a2i(char *s)
{
	int num=0;
	while(*s)
	{
		num=((*s)-'0')+num*10;
		s++;
	}
	return num;
}
int a2iSign(char *s)
{
	int sign = 1;
	if(*s == '-') // is first char indicate negative
	{
		sign = -1;	//save negative
		s++; //next char
	}
	int num=0;
	while(*s)
	{
		num=((*s)-'0')+num*10;
		s++;
	}
	return num;//*sign;
}
uint8_t runMenu(char* (*useMenu)(uint8_t), uint8_t menuPos)
{
	buttonDebounce();
	char* menuSizePtr = useMenu(MENU_SIZE); 
	int menuSize = a2i(menuSizePtr); // turn text returned by menu into a number 
	showTitle(useMenu, MENU_TITLE); //load menu title
	showMenu(useMenu, menuPos); //load menu starting possition, initial display
	long oldKnob, newKnob; //vars for dealing with menu position
	oldKnob = newKnob = knob.read(); //prime them
	double menuTimeout = MENU_TIMEOUT; 
	while (menuTimeout != 0)
	{
  		newKnob = knob.read();
  		if(newKnob <= oldKnob-DIAL_DETENT)
  		{
	  		oldKnob -= DIAL_DETENT;
	  		menuTimeout = MENU_TIMEOUT;
	  		menuPos++;
	 		if (menuPos >= MENU_START + menuSize) {menuPos -= menuSize;}
			showMenu(useMenu, menuPos);
  		}
  		if(newKnob >= oldKnob+DIAL_DETENT)
  		{
	  		oldKnob += DIAL_DETENT;
	  		menuTimeout = MENU_TIMEOUT;
	  		menuPos--;
			if (menuPos == MENU_TITLE) {menuPos += menuSize;}
			showMenu(useMenu, menuPos);
  		}
  		if (digitalRead(PUSH_BUTTON)) {menuTimeout--;} // keep counting down
		else {buttonDebounce();	return(menuPos);}  //was pressed return current selection
  		delay(10);
	}
	return(MENU_TITLE);  //result = timeout
}
char* errorMessage () {return("*Error*");}  //reduces memory, removes error string info from every menu
char* exitMessage () {return("Exit and Save");}  //reduces memory, removes error string info from every menu

//pattern  menu	
enum patternModes {
	PATTERN_TITLE			= MENU_TITLE ,
	PATTERN_SOLID			= MENU_START ,
	PATTERN_FADES						 ,
	PATTERN_PULSE						 ,
	PATTERN_HEARTBEAT					 ,
	PATTERN_EXIT						 ,
	PATTERN_SIZE			= MENU_SIZE
	};
char* patternMenuContent(uint8_t pos)
{
	if 		(pos ==	PATTERN_TITLE			)		{return(	"Pattern Menu:"		);}
	else if (pos == PATTERN_SOLID			)		{return(	"Static"			);}
	else if (pos == PATTERN_FADES			)		{return(	"Fading"			);}
	else if (pos ==	PATTERN_PULSE			)		{return(	"Pulses"			);}
	else if (pos ==	PATTERN_HEARTBEAT		)		{return(	"Heartbeat"			);}
	else if (pos ==	PATTERN_EXIT			)		{return(	exitMessage()		);}
	else if (pos ==	PATTERN_SIZE			)		{return(	"5"					);}
	else											{return(	errorMessage()		);}
}
void patternMenu()
{
	uint8_t	result = patternMode;
	while (result != PATTERN_EXIT && result != MENU_TITLE)
	{
  		result = runMenu(patternMenuContent, result);
  		if (result >= MENU_START && result < PATTERN_EXIT) {patternMode = result;}
  		//else {}
	}
}

int valueMenu (int start, int end, int position)
{
	char tempChar;
	int posHold = position;
	int menuSize = end - start; // turn text returned by menu into a number
	long oldKnob, newKnob; //vars for dealing with menu position
	oldKnob = newKnob = knob.read(); //prime them
	long menuTimeout = MENU_TIMEOUT;
	while (menuTimeout != 0)
	{
		newKnob = knob.read();
		if(newKnob <= oldKnob-DIAL_DETENT)
		{
			oldKnob = oldKnob-DIAL_DETENT;
			menuTimeout = MENU_TIMEOUT;
			if (position < end) {position++;};
			lcdClearLine(1);
			lcdPrint(itoa(position, &tempChar, 10));
		}
		else if(newKnob >= oldKnob+DIAL_DETENT)
		{
			oldKnob = oldKnob+DIAL_DETENT;
			menuTimeout = MENU_TIMEOUT;
			if (position > start) {position--;};
			lcdClearLine(1);
			lcdPrint(itoa(position, &tempChar, 10));
		}

		if (digitalRead(PUSH_BUTTON)) {buttonDebounce(); return(position);}  //was pressed return current selection
	}
	return(posHold);  //result = timeout no change
}
//mainmenu
enum mainModes {
	MAIN_TITLE			= MENU_TITLE,
	MAIN_PATTERN		= MENU_START,
	MAIN_COLOR						,
	MAIN_BRIGHTNESS					,
	MAIN_EXIT						,
	MAIN_SIZE			= MENU_SIZE
};
char* mainMenuContent(uint8_t pos)
{
	if 		(pos ==	MAIN_TITLE			)		{return(	"Main Menu:"		);}
	else if (pos == MAIN_PATTERN		)		{return(	"Patterns"			);}
	else if (pos == MAIN_COLOR			)		{return(	"Colors"			);}
	else if (pos ==	MAIN_BRIGHTNESS		)		{return(	"Brightness"		);}
	else if (pos ==	MAIN_EXIT			)		{return(	exitMessage()		);}
	else if (pos ==	MAIN_SIZE			)		{return(	"4"					);}
	else										{return(	errorMessage()		);}
}
void mainMenu()
{
	uint8_t	result = mainMode;
	while (result != MAIN_EXIT && result != MENU_TITLE)
	{
		result = runMenu(mainMenuContent, result);
		if		(result == MAIN_PATTERN		) {patternMenu();		}
		else if (result == MAIN_PATTERN		) {patternMenu();		}
		else if (result == MAIN_COLOR		) {patternMenu();		}
		else if (result == MAIN_BRIGHTNESS	) {patternMenu();		}
		else if (result == MAIN_EXIT		) {lcdClearScreen();	} //clean up after menu
		//else {}
	}
}

void setup ()
{
	// init
	//leds[0] is used for a temp working location
	leds[1].redPin	=	PIN_RED1;
	leds[1].greenPin	= PIN_GREEN1;
	leds[1].bluePin	=  PIN_BLUE1;
	leds[2].redPin	=	PIN_RED2;
	leds[2].greenPin	= PIN_GREEN2;
	leds[2].bluePin	=  PIN_BLUE2;
	pinMode(leds[1].redPin,OUTPUT);
	pinMode(leds[1].greenPin,OUTPUT);
	pinMode(leds[1].bluePin,OUTPUT);
	pinMode(leds[2].redPin,OUTPUT);
	pinMode(leds[2].greenPin,OUTPUT);
	pinMode(leds[2].bluePin,OUTPUT);
	pinMode(PUSH_BUTTON, INPUT_PULLUP);
	pinMode(OLED_R, OUTPUT);
	digitalWrite(OLED_R, HIGH);
  
#if (LEDS == 4)
	leds[3].redPin	=	PIN_RED3;
	leds[3].greenPin	= PIN_GREEN3;
	leds[3].bluePin	=  PIN_BLUE3;
	leds[4].redPin	=	PIN_RED4;
	leds[4].greenPin	= PIN_GREEN4;
	leds[4].bluePin	=  PIN_BLUE4;
	pinMode(leds[3].redPin,OUTPUT);
	pinMode(leds[3].greenPin,OUTPUT);
	pinMode(leds[3].bluePin,OUTPUT);
	pinMode(leds[4].redPin,OUTPUT);
	pinMode(leds[4].greenPin,OUTPUT);
	pinMode(leds[4].bluePin,OUTPUT);
 #endif
 //EEPROM.write(address = bay_4_count, bay4Count = 0);
  mainMode = MENU_START;
  patternMode = MENU_START;
  colorMode = MENU_START;
  lcdInit();
  lcdClearScreen();
  lcdSetPos(0,0);
  lcdPrint("hello");
  //delay(2500);
  //mainMenu();
}
void loop ()
{
		if (digitalRead(PUSH_BUTTON) == 0) {mainMenu(); buttonDebounce();  lcdClearScreen(); } 
		
		
}