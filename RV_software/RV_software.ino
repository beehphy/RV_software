// RV led controller - Jesse Banks
// 2/23/15

//#include <eeprom.h>
#include <Arduino.h>
#include <Encoder.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
//#include <string.h>

typedef struct {
	byte r;
	byte g;
	byte b;
	byte redPin;
	byte greenPin;
	byte bluePin;
} Output;
typedef struct {
	byte g;
	byte r;
	byte b;
} RGB;
typedef struct {
	byte h;
	byte s;
	byte v;
	int hsvMode;
} HSV;
typedef struct {
	int time;
	int period;
	int rate;
} Clock;
typedef struct {
	int period;
	int rise;
	int width;
	int fall;
	int count;
} Wave;

#define BOOL uint8_t

//Hardware defines ***********************************************************************
#define LEDS				4
#define FRAME_RATE			30
#define RENDER_RATE			1000 / FRAME_RATE
#define PIN_RED1			4//2	//LED OUTPUT
#define PIN_GREEN1			2//3	//LED OUTPUT
#define PIN_BLUE1			3//4	//LED OUTPUT
#define PIN_RED2			7//5	//LED OUTPUT
#define PIN_GREEN2			5//6	//LED OUTPUT
#define PIN_BLUE2			6//7	//LED OUTPUT
#define PIN_RED3			10//8	//LED OUTPUT
#define PIN_GREEN3			8//9	//LED OUTPUT
#define PIN_BLUE3			9//10	//LED OUTPUT
#define PIN_RED4			13//11	//LED OUTPUT
#define PIN_GREEN4			11//12	//LED OUTPUT
#define PIN_BLUE4			12//13	//LED OUTPUT
#define ENCODER_A			A8	
#define ENCODER_B			A9
#define PUSH_BUTTON			A10
#define OLED_R				A11
#define SA0					A12 
#define V12					A13 //battery sense
	
//oled stuff ***********************************************************************
#define OLED_ADDRESS		0x78
#define OLED_RUN			0xa4
#define OLED_OFF			0xa5
#define OLED_INVERSE		0xa7
#define OLED_NORMAL			0xa6
#define OLED_SLEEP			0xae
#define OLED_ACTIVE			0xaf
#define OLED_SCROLL_OFF		0x2e
#define COMMAND_COMMAND		0x80
#define DATA_COMMAND		0x40
#define CURSOR_COMMAND		0x00 // To make a blinking cursor, like data, but doesn't move column
#define HIGH_COL_START_ADDR 0x10
#define LOW_COL_START_ADDR  0x00
#define SEGMENT_REMAP		0xA1
#define DISPLAY_START_LINE	0x40
#define MULTIPLEX_RATIO		0x1F
#define COM_PIN_RATIO		0x12
#define PRE_CHARGE_PERIOD	0xd2
#define CLOCK_DIVIDE_RATIO	0xa0
#define CONTRAST_LEVEL		0x81
#define VCOM_DETECT			0x34

#define LCD_HEIGHT			32
#define LCD_WIDTH			128
#define MAX_ROWS			4
#define MAX_COLS			21
#define SMALL_FONT_CHARACTER_W 5
#define SMALL_FONT_CHARACTER_H 8
#define H_PADDING 1
#define COLUMN_OFFSET 4
#define ASCII_OFFSET_LOW  0x20
#define ASCII_OFFSET_HIGH 0x7e
static byte PROGMEM font5x8[][5] = {// The 7-bit ASCII character set...
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
static byte PROGMEM unknown5x8[5] = { 0x7f, 0x41, 0x41, 0x41, 0x7f }; // unknown char
static byte PROGMEM custom5x8[][5] = { // A place for custom characters
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },  // Space
	{ 0x7f, 0x41, 0x41, 0x41, 0x7f }   // ?
};

byte   lcdRow      = 0;
byte   lcdColumn   = 0;
#define MIN(x, y) x < y ? x : y;
//#define MAX(x, y) x > y ? x : y;

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
	#define TWI_STOP_DELAY 80
	TWCR = ((1<<TWINT) | (1<<TWEN) | (1<<TWSTO)); //send stop condition
//	for(uint8_t ct = 0; ct < 40; ct++); //wait a moment in case a start immediately follows
	byte cnt = 0;
	while (cnt < TWI_STOP_DELAY)
	{
		cnt++;
	}
}
void twiSend(byte u8data)
{
	TWDR = u8data; //data to dat register
	TWCR = ((1<<TWINT)|(1<<TWEN)); //set TWI byte ready to go
	while ((TWCR & (1<<TWINT)) == 0); //wait for data to send
}
void twiSendCmd(byte command)
{
	twiSend(COMMAND_COMMAND);   // command command
	twiSend(command);// command value
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
	twiSendCmd(0xad);       // set master configuration
		twiSendCmd(0x8e);   // 
	twiSendCmd(0xd8);       // Set Area Color Mode On/Off & Low Power Display Mode
		twiSendCmd(0x05);   // 
	twiSendCmd(SEGMENT_REMAP); // --set segment re-map 96 to 1
	twiSendCmd(0xC8);       // --Set COM Output Scan Direction 16 to 1
	twiSendCmd(0xda);       // --set com pins hardware configuration
		twiSendCmd(COM_PIN_RATIO); // set ratio 128x64:0x12 96x16:0x02
	twiSendCmd(0x91); //Set current drive pulse width of BANK0, Color A, Band C.
		twiSendCmd(0x3f);
		twiSendCmd(0x3f);
		twiSendCmd(0x3f);
		twiSendCmd(0x3f);
	twiSendCmd(0x81);       // --set contrast control register
		twiSendCmd(CONTRAST_LEVEL);
	twiSendCmd(0xd9);       // --set pre-charge period
		twiSendCmd(PRE_CHARGE_PERIOD);   // set ratio 128x64:0xf1 96x16:0x22
	twiSendCmd(DISPLAY_START_LINE); // set display start line
	twiSendCmd(0xdb);       // --set vcomh
		twiSendCmd(VCOM_DETECT); // --0.77vref
	twiSendCmd(0x20);       // Set Memory Addressing Mode
		twiSendCmd(0x00);   // 00, Horizontal Addressing Mode; 01, Vertical Addressing Mode; 10, Page Addressing Mode (RESET); 11, Invalid
	twiSendCmd(OLED_NORMAL);
	twiSendCmd(OLED_RUN);
	twiSendCmd(OLED_ACTIVE); // --turn on oled panel
	
	twiStop();
}
void lcdSetPos(byte row, byte column)
{
	row = MIN(row, MAX_ROWS);
	column = MIN(column, LCD_WIDTH + COLUMN_OFFSET);
	column += COLUMN_OFFSET;
	twiStart();
	twiSend(OLED_ADDRESS);
	twiSendCmd(0xb0+row);
	twiSendCmd((column>>4) | HIGH_COL_START_ADDR); // high column start address or'd 0x10
	twiSendCmd((column&0x0f) | LOW_COL_START_ADDR);       // low column start address
	twiStop();
	
	lcdRow = row;	//save cursor location
	lcdColumn = column;
}
void lcdFill(char fillData)
{
	byte m,n;
	for(m = 0; m <= MAX_ROWS; m++) {
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
void lcdClearLine(byte line)
{
	if (line >= MAX_ROWS) return;
	lcdSetPos(line, 0);		
	twiStart();
	twiSend(OLED_ADDRESS);
	twiSend(DATA_COMMAND);
// 	for(int n = 0; n < LCD_WIDTH; n++) {
	byte n = 0;
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
void lcdWriteChar(byte data)
{	
// 	twiStart(); // maybe move these later to lcdprint, etc.
// 	twiSend(OLED_ADDRESS); // maybe move these later to lcdprint, etc.
// 	twiSend(DATA_COMMAND); // maybe move these later to lcdprint, etc.
	
	if (data >= ASCII_OFFSET_LOW && data <= ASCII_OFFSET_HIGH) { //is the data within the charecter set?
		data -= ASCII_OFFSET_LOW; // offsets byte by 0x20, to beginning of ascii codes in memory
		//for (uint8_t i = 0; i < SMALL_FONT_CHARACTER_W; i++) {
		byte i = 0;
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
		byte i = 0;
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
	
	byte i = 0;
	byte len = strlen(buffer);
	
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
void displayBattery (byte line)
{	
	//measure and display voltage "Bat: xx.xxV"
	char temp[10];
	lcdClearLine(line);
	lcdSetPos(line, 0);
	lcdPrint("Bat: ");
	long vCapture = (analogRead(V12) * 79) / 20 + 63;
	lcdPrint(itoa(vCapture / 100,temp,10));
	lcdPrint(".");
	int vcapsv = vCapture % 100;
	if (vcapsv < 10) {lcdPrint("0");}
	lcdPrint(itoa(vcapsv,temp,10));
	lcdPrint("V");
	
	//Deep cycle Lead acid battery guage
	//vCapture 1280 (12.8v)or greater is full
	//vCapture 950 (9.5v) or lower is empty
	lcdSetPos(line, 72);
	vCapture = (vCapture - 950) * 19 / 120; // offset, and scale, result is columns of full battery bars 0-50;
	if (vCapture < 0) {vCapture = 0;} // limit to end of of battery
	if (vCapture > 51) {vCapture = 51;} // limit to end of of battery
	twiStart();
	twiSend(OLED_ADDRESS);
	twiSend(DATA_COMMAND); ///pixel data stream
	twiSend(0x7e); //bat contact
	twiSend(0x42); //contact sides
	twiSend(0x42); //contact sides
	twiSend(0xFF); //top edge
	byte i = 0; //start display sequence count
	while (vCapture < 51) {
		twiSend(0x81); //empty bars
		vCapture++; //count up to full battery
		i++; //keep track of display bars
	}
	while (i < 52) { // this should result in 1 bars at the end for end cap
		twiSend(0xFF); //full bars
		i++;
	}
	twiStop();
}	

//Menu stuff ***********************************************************************
Encoder knob(ENCODER_A, ENCODER_B);
#define MENU_DELAY 1
#define MENU_TIMEOUT 3000 
#define DIAL_DETENT 4 //for blue type knob, 2 for green type knob
#define MENU_TITLE -1
#define MENU_START 1
#define MENU_SIZE -2

enum menuBehavior
{
	MENU_BEHAVIOR_STOP,
	MENU_BEHAVIOR_LOOP
	};
enum mainModes {
	MAIN_TITLE			= MENU_TITLE,
	MAIN_PATTERN		= MENU_START,
	MAIN_COLOR						,
	MAIN_SPEED						,
	MAIN_BRIGHTNESS					,
	MAIN_EXIT						,
	MAIN_SIZE			= MENU_SIZE
};
enum patternModes {
	PATTERN_TITLE			= MENU_TITLE ,
	PATTERN_SOLID			= MENU_START ,
	PATTERN_SEQUENCE					 ,
	PATTERN_PULSE						 ,
	PATTERN_HEARTBEAT					 ,
	PATTERN_EXIT						 ,
	PATTERN_SIZE			= MENU_SIZE
	};
enum speedModes
{
	SPEED_TITLE				= MENU_TITLE,
	SPEED_1					= MENU_START,
	SPEED_2								,
	SPEED_3								,
	SPEED_4								,
	SPEED_5								,
	SPEED_EXIT							,
	SPEED_SIZE					= MENU_SIZE
};
enum colorModes
{
	COLOR_TITLE						= MENU_TITLE,
	COLOR_BEHAVIOR					= MENU_START,
	COLOR_GLOBAL1								,
	COLOR_GLOBAL2								,
	COLOR_GLOBAL3								,
	COLOR_GLOBAL4								,
	COLOR_EXIT									,
	COLOR_MENU_SIZE					= MENU_SIZE
};
enum colorSequenceLengthModes
{
	COLOR_SEQUENCE_LENGTH_TITLE			= MENU_TITLE,
	COLOR_SEQUENCE_LENGTH_1				= MENU_START,
	COLOR_SEQUENCE_LENGTH_2							,
	COLOR_SEQUENCE_LENGTH_3							,
	COLOR_SEQUENCE_LENGTH_4							,
	COLOR_SEQUENCE_LENGTH_EXIT						,
	COLOR_SEQUENCE_LENGTH_MENU_SIZE		= MENU_SIZE
};
enum colorBehaviorModes
{
	COLOR_BEHAVIOR_TITLE					= MENU_TITLE,
	COLOR_BEHAVIOR_SOLID					= MENU_START,
	COLOR_BEHAVIOR_SEQUENCE								,
	COLOR_BEHAVIOR_RAINBOW								,
	COLOR_BEHAVIOR_RANDOM								,
	COLOR_BEHAVIOR_EXIT									,
	COLOR_BEHAVIOR_MENU_SIZE				= MENU_SIZE
};
enum colorHSVvalues 
{
	COLOR_HSV_TITLE		= MENU_TITLE,
	COLOR_HSV_RED		= 0,
	COLOR_HSV_ORANGE	= 15,
	COLOR_HSV_YELLOW	= 30,
	COLOR_HSV_GREEN		= 86,
	COLOR_HSV_TEAL		= 105,
	COLOR_HSV_BLUE		= 150,
	COLOR_HSV_SKY		= 172,
	COLOR_HSV_VIOLET	= 190,
	COLOR_HSV_PINK		= 234,
	COLOR_HSV_END		= 255,
	COLOR_HSV_WHITE		,
	COLOR_HSV_CUSTOM	,
	COLOR_HSV_EXIT		,
	COLOR_HSV_SIZE = MENU_SIZE
	};
enum colorSelectModes {
	COLOR_SELECT_TITLE				= MENU_TITLE,
	COLOR_SELECT_RED				= MENU_START,
	COLOR_SELECT_ORANGE							,
	COLOR_SELECT_YELLOW							,
	COLOR_SELECT_GREEN							,
	COLOR_SELECT_TEAL							,
	COLOR_SELECT_BLUE							,
	COLOR_SELECT_SKY							,
	COLOR_SELECT_VIOLET							,
	COLOR_SELECT_PINK							,
	COLOR_SELECT_WHITE							,
	COLOR_SELECT_CUSTOM							,
	COLOR_SELECT_RAINBOW_ROLL					,
	COLOR_SELECT_EXIT							,
	COLOR_SELECT_MENU_SIZE			= MENU_SIZE
};
enum brightnessModes
{
	BRIGHT_TITLE				= MENU_TITLE,
	BRIGHT_MINIMUM				= 0			,//1%
	BRIGHT_MAXIMUM				= 100		,//100%
	BRIGHT_EXIT								,
	BRIGHT_SIZE					= MENU_SIZE
};

Output leds[LEDS]; 
byte globalIntensity = 0x40; // should be eeprom loads
byte mainMode = MENU_START;
byte patternMode  = MENU_START;
byte colorMode = MENU_START;
byte colorBehaviorMode = MENU_START;
//byte colorSeqeunceLength = MENU_START; in colorSequenceClock.period now
HSV globalColorHSV[LEDS]; 
//byte speedMode = MENU_START; in colorSequenceClock.rate now
byte brightnessMode = MENU_START;
char tempChar [MAX_COLS]; // char width of OLED display
Clock patternClock = {1,10,1}; //something
Clock patternRenderClock = {1, 100, SPEED_3};
Clock colorSequenceClock = {1,4,1};
Clock colorRenderClock = {1, 100, SPEED_3};
Clock rainbowClock = {5, 255, -1}; //time, per, rate

RGB& hsvToRgb(HSV& input)
{
	RGB tempRGB;
	unsigned char region, p, q, t;
	unsigned int h, s, v, remainder;

	if (input.s == 0)
	{
		tempRGB.r = input.v;
		tempRGB.g = input.v;
		tempRGB.b = input.v;
		return(tempRGB);
	}

	// converting to 16 bit to prevent overflow
	h = input.h;
	s = input.s;
	v = input.v;

	region = h / 43;
	remainder = (h - (region * 43)) * 6;

	p = (v * (255 - s)) >> 8;
	q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
		case 0:
		tempRGB.r = v;
		tempRGB.g = t;
		tempRGB.b = p;
		break;
		case 1:
		tempRGB.r = q;
		tempRGB.g = v;
		tempRGB.b = p;
		break;
		case 2:
		tempRGB.r = p;
		tempRGB.g = v;
		tempRGB.b = t;
		break;
		case 3:
		tempRGB.r = p;
		tempRGB.g = q;
		tempRGB.b = v;
		break;
		case 4:
		tempRGB.r = t;
		tempRGB.g = p;
		tempRGB.b = v;
		break;
		default:
		tempRGB.r = v;
		tempRGB.g = p;
		tempRGB.b = q;
		break;
	}
	return(tempRGB);
}
void buttonDebounce()
{
	#define DEBOUNCE_LOOPS 20000; //approximately 1 millisecond delay after button is released
	int x = DEBOUNCE_LOOPS;  //start countdown
	while(x > 0)
	{
		if (digitalRead(PUSH_BUTTON)) {x--;} //button press is detected as low, count down while high.
		else {x = DEBOUNCE_LOOPS;}	//restart the timer, a bounce was detected.
	}
}
void updateLedOutputs()
{
	int i = 0;
	while (i < LEDS)
	{
		analogWrite(leds[i].redPin,   leds[i].r);
		analogWrite(leds[i].greenPin, leds[i].g);
		analogWrite(leds[i].bluePin,  leds[i].b);
		i++;
	}
}
/*
void updateLEDsColorSingle (HSV& input)
{
	RGB tempRGB = hsvToRgb(input);
	int i = 0;
	while (i < LEDS)
	{
		leds[i].r = tempRGB.r;
		leds[i].g = tempRGB.g;
		leds[i].b = tempRGB.b;
		i++;
	}
	updateLedOutputs();
}
*/

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
void showMenuContent  (char* (*useMenu)(int), int pos)
{
	lcdClearLine(1);
	lcdSetPos(1, 6);
	//lcdPrint(">> ");
 	lcdPrint(useMenu(pos));
}
void showMenuTitle (char* (*useMenu)(int), int pos)
{
	lcdClearLine(0);
	lcdSetPos(0, 0);
	lcdPrint(useMenu(pos));
	displayBattery(3);
}
int runContentMenu(char* (*useMenu)(int), int menuPos, byte menuExit)
{
	buttonDebounce();
	char* menuSizePtr = useMenu(MENU_SIZE); 
	int menuSize = a2i(menuSizePtr); // turn text returned by menu into a number 
	showMenuTitle(useMenu, MENU_TITLE); //load menu title
	showMenuContent(useMenu, menuPos); //load menu starting possition, initial display
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
			showMenuContent(useMenu, menuPos);
  		}
  		if(newKnob >= oldKnob+DIAL_DETENT)
  		{
	  		oldKnob += DIAL_DETENT;
	  		menuTimeout = MENU_TIMEOUT;
	  		menuPos--;
			if (menuPos < MENU_START) {menuPos += menuSize;}
			showMenuContent(useMenu, menuPos);
  		}
  		if (digitalRead(PUSH_BUTTON)) {menuTimeout--;} // keep counting down
		else {buttonDebounce();	return(menuPos);}  //was pressed return current selection
  		delay(MENU_DELAY);
	}
	return(menuExit);  //result = timeout
}
int runValueMenu (byte startingValue, int increment, int minimum, int maximum, byte rolloverBOOL, byte menuExit)
{
	buttonDebounce();
	int tempINT = int(startingValue);
	long oldKnob, newKnob; //vars for dealing with encoder
	oldKnob = newKnob = knob.read(); //initial values
	long menuTimeout = MENU_TIMEOUT; //countdown timeout
	while (menuTimeout != 0)
	{
		newKnob = knob.read();
		if(newKnob <= oldKnob-DIAL_DETENT)
		{
			oldKnob = oldKnob-DIAL_DETENT;
			menuTimeout = MENU_TIMEOUT;
			tempINT += increment;
			if (tempINT > maximum) 
			{
				if (rolloverBOOL)
				{
					tempINT = minimum;
				}
				else
				{
					tempINT = maximum;
				}
			}
			return(byte(tempINT));
		}
		else if(newKnob >= oldKnob+DIAL_DETENT)
		{
			oldKnob = oldKnob+DIAL_DETENT;
			menuTimeout = MENU_TIMEOUT;
			tempINT -= increment;
			if (tempINT < minimum)
			{
				if (rolloverBOOL)
				{
					tempINT = maximum;
				}
				else
				{
					tempINT = minimum;
				}
			}
			return(byte(tempINT));	
		}
		if (digitalRead(PUSH_BUTTON) == 0) //was pressed return current selection
		{
			return(menuExit);  //result = timeout no change
		}
		delay(MENU_DELAY);
		menuTimeout--;
	}
	return(menuExit);  //result = timeout no change
};

char* errorMessage () {return("*Error*");}  //reduces memory, removes error string info from every menu
char* exitMessage () {return("Nothing and Save");}  //reduces memory, removes error string info from every menu


Clock& clockUpdate(Clock& clockInUse)
{
// 	if (clockInUse.direction) // if positive
// 	{
		clockInUse.time += clockInUse.rate; //rate can be negative number
		if (clockInUse.time > clockInUse.period) {clockInUse.time -= clockInUse.period + 1;}
// 	}
// 	else 
// 	{
// 		clockInUse.time -= clockInUse.rate; 
 		if (clockInUse.time < 0)				{clockInUse.time += clockInUse.period + 1;}
}
uint8_t patternRender(int ledNumber)
{
	return(0);
}
HSV& colorRandomHSV(int ledNumber)
{
	HSV tempHSV;
	tempHSV.s = 255;
	tempHSV.v = 127;
	tempHSV.h = rand();
	return(tempHSV);
}
HSV& colorSequenceRender(int ledNumber)
{
	HSV tempHSV;
	tempHSV.s = 255;
	tempHSV.v = 127;
	//	if (colorMode == COLOR_SELECT_CUSTOM) {tempHSV.h = leds[ledNumber].customColor;}
	return(tempHSV);
}
RGB& colorValueRender(uint8_t intensity,  int ledNumber)
{
	HSV tempHSV;
	tempHSV.s = 255;
	tempHSV.v = 127;
	if		(colorMode == COLOR_BEHAVIOR_SOLID		) {tempHSV.h = globalColorHSV[ledNumber].h;}
	else if (colorMode == COLOR_BEHAVIOR_SEQUENCE	) {tempHSV = colorSequenceRender(ledNumber);}
	else if (colorMode == COLOR_BEHAVIOR_RAINBOW	) {tempHSV.h = globalColorHSV[ledNumber].h + rainbowClock.time;}
	else if (colorMode == COLOR_BEHAVIOR_RANDOM		) {tempHSV = colorRandomHSV(ledNumber);}
	//else if (result == COLOR_SELECT_WHITE					) {tempHSV.v = 255; tempHSV.s = 0;}
	RGB tempRGB = hsvToRgb(tempHSV);
	tempRGB.r = (tempRGB.r * intensity) / 255;
	tempRGB.g = (tempRGB.g * intensity) / 255;
	tempRGB.b = (tempRGB.b * intensity) / 255;
	return(tempRGB);
}
void render ()
{
	//clockUpdate();
	RGB tempRGB;
	int i = 0;
	uint8_t bright;
	while (i <= LEDS)
	{
		//bright = patternRender(&tempRGB, i);
		//colorValueRender(bright, i, &tempRGB);
		leds[i].r = (tempRGB.r * globalIntensity) / 255;
		leds[i].g = (tempRGB.g * globalIntensity) / 255;
		leds[i].b = (tempRGB.b * globalIntensity) / 255;
		i++;
	}
	updateLedOutputs();
}

//pattern  menu	
char* patternMenuContent(int pos)
{
	if 		(pos ==	PATTERN_TITLE			)		{return(	"Pattern Menu:"		);}
	else if (pos == PATTERN_SOLID			)		{return(	"Static"			);}
	else if (pos == PATTERN_SEQUENCE		)		{return(	"Sequence"			);}
	else if (pos ==	PATTERN_PULSE			)		{return(	"Pulses"			);}
	else if (pos ==	PATTERN_HEARTBEAT		)		{return(	"Heartbeat"			);}
	else if (pos ==	PATTERN_SIZE			)		{return(	"4"					);}
	else											{return(	errorMessage()		);}
}
void patternMenu()
{
	int	result = patternMode;
	while (result != PATTERN_EXIT)
	{
  		result = runContentMenu(patternMenuContent, result, PATTERN_EXIT);
  		if (result >= MENU_START && result < PATTERN_EXIT) {patternMode = result;}
  		else {}
	}
}

char* customColorMenuContent(int pos)
{
	if 		(pos == COLOR_HSV_TITLE	)	{return("Dial that color in!"		);}
	else if (pos >= BRIGHT_MINIMUM && pos <= BRIGHT_MAXIMUM	)
	{
		//build a string based on global brightness
		char tempstring [MAX_COLS + 1] = "Hue: "; // room for value render
		char* buffer = itoa(globalIntensity, tempChar, 10);
		byte i = 5; //length of hue:
		byte len = strlen(buffer);
		byte j = 0;
		while (i < len) {
			tempstring[i] = buffer[j];
			j++;
			i++;
		}
		tempstring[i] = '\n';
		return(	tempstring	);
	}
	else if (pos == BRIGHT_SIZE		)	{return("100"					);} //100%
	else								{return(errorMessage()			);}
}
HSV& customColorMenu (HSV& underChange) //pick a color hue with the dial
{
	int	result = underChange.h;
	showMenuTitle(customColorMenuContent, COLOR_HSV_TITLE);
	while (result != BRIGHT_EXIT)
	{
		result = runValueMenu(result, 1, COLOR_HSV_RED, COLOR_HSV_END, MENU_BEHAVIOR_LOOP, COLOR_HSV_EXIT);
		if (result >= BRIGHT_MINIMUM && result <= BRIGHT_MAXIMUM	)
		{showMenuContent(customColorMenuContent, result); brightnessMode = result;}
	}
}

char* colorSelectContent(int pos)
{
	if 		(pos == COLOR_SELECT_TITLE				)	{return(	"Choose a Color:"		);}
	else if (pos == COLOR_SELECT_RED				)	{return(	"Red"					);}
	else if (pos == COLOR_SELECT_ORANGE				)	{return(	"Orange"				);}
	else if (pos == COLOR_SELECT_YELLOW				)	{return(	"Yellow"				);}
	else if (pos == COLOR_SELECT_GREEN				)	{return(	"Green"					);}
	else if (pos == COLOR_SELECT_TEAL				)	{return(	"Teal"					);}
	else if (pos == COLOR_SELECT_BLUE				)	{return(	"Blue"					);}
	else if (pos == COLOR_SELECT_SKY				)	{return(	"Sky"					);}
	else if (pos == COLOR_SELECT_VIOLET				)	{return(	"Violet"				);}
	else if (pos == COLOR_SELECT_PINK				)	{return(	"Pink"					);}
	else if (pos == COLOR_SELECT_WHITE				)	{return(	"White"					);}
	else if (pos == COLOR_SELECT_CUSTOM				)	{return(	"Custom Color"			);}
	else if (pos == COLOR_SELECT_RAINBOW_ROLL		)	{return(	"Rainbow"				);}
	else if (pos == COLOR_SELECT_MENU_SIZE			)	{return(	"12"					);}
	else												{return(	"Custom Color"			);}
}
HSV& colorSelectMenu(HSV& underChange) //uint8_t *colorSpace)
{
	int result = underChange.hsvMode;
	while (result != COLOR_SELECT_EXIT)
	{
		result = runContentMenu(colorSelectContent, result, COLOR_SELECT_EXIT);
		if		(result == COLOR_SELECT_CUSTOM								) {underChange.hsvMode = result; underChange = customColorMenu(underChange);}
		else if	(result >= COLOR_SELECT_RED	&& result <= COLOR_SELECT_EXIT	) {underChange.hsvMode = result;}
	}
	return(underChange);
}

char* colorSequenceLengthContent(int pos)
{
	if 		(pos ==	COLOR_SEQUENCE_LENGTH_TITLE		)	{return(	"How many colors in the loop?"		);}
	else if (pos == COLOR_SEQUENCE_LENGTH_1			)	{return(	"One"			);}
	else if (pos == COLOR_SEQUENCE_LENGTH_2			)	{return(	"Two"			);}
	else if (pos ==	COLOR_SEQUENCE_LENGTH_3			)	{return(	"Three"			);}
	else if (pos ==	COLOR_SEQUENCE_LENGTH_4			)	{return(	"Four"			);}
	else if (pos ==	COLOR_SEQUENCE_LENGTH_MENU_SIZE	)	{return(	"4"				);}
	else												{return(	errorMessage()		);}
}
void colorSequenceLengthMenu()
{
	int	result = colorSequenceClock.period;
	result = runContentMenu(colorSequenceLengthContent, result, COLOR_SEQUENCE_LENGTH_EXIT);
	if (result >= MENU_START && result < COLOR_SEQUENCE_LENGTH_EXIT) 
		{colorSequenceClock.period = result;}
}

char* colorBehaviorMenuContent(int pos)
{
	if 		(pos == COLOR_BEHAVIOR_TITLE			)	{return(	"Color Behavior:"		);}
	else if (pos == COLOR_BEHAVIOR_SOLID			)	{return(	"Static"				);}
	else if (pos == COLOR_BEHAVIOR_SEQUENCE			)	{return(	"Sequence"				);}
	else if (pos == COLOR_BEHAVIOR_RAINBOW			)	{return(	"Rainbow"				);}
	else if (pos == COLOR_BEHAVIOR_RANDOM			)	{return(	"Random"				);}
	else if (pos == COLOR_BEHAVIOR_MENU_SIZE		)	{return(	"4"					);}
	else												{return(	errorMessage()			);}
}
void colorBehaviorMenu()
{
	int	result = colorBehaviorMode;
	while (result != COLOR_BEHAVIOR_EXIT)
	{
		result = runContentMenu(colorBehaviorMenuContent, result, COLOR_BEHAVIOR_EXIT);
		if		(result >= COLOR_BEHAVIOR_SOLID		)	{colorBehaviorMode = 	result;}
		else if (result == COLOR_BEHAVIOR_SEQUENCE	)	{colorBehaviorMode = 	result; colorSequenceLengthMenu();}
		else if (result == COLOR_BEHAVIOR_RAINBOW	)	{colorBehaviorMode = 	result;} // rainbow driven by a counter signal
		else if (result == COLOR_BEHAVIOR_RANDOM	)	{colorBehaviorMode = 	result;} // random has no control
	}
}

char* colorMenuContent(int pos)
{
	if 		(pos ==	COLOR_TITLE		)		{return(	"We are changing:"	);}
	else if (pos ==	COLOR_BEHAVIOR	)		{return(	"Color Behavior"	);}
	else if (pos ==	COLOR_GLOBAL1	)		{return(	"Color 1"			);}
	else if (pos ==	COLOR_GLOBAL2	)		{return(	"Color 2"			);}
	else if (pos ==	COLOR_GLOBAL3	)		{return(	"Color 3"			);}
	else if (pos ==	COLOR_GLOBAL4	)		{return(	"Color 4"			);}
	else if (pos ==	COLOR_MENU_SIZE	)		{return(	"5"					);}
	else									{return(	errorMessage()		);}
}
void colorMenu()
{
	int	result = colorMode;
	while (result != COLOR_EXIT)
	{
		result = runContentMenu(colorMenuContent, result, COLOR_EXIT);
		if		(result == COLOR_BEHAVIOR						) {colorMode = result; colorBehaviorMenu();}
		else if (result == COLOR_GLOBAL1						) {colorMode = result; globalColorHSV[0] = colorSelectMenu(globalColorHSV[0]);}
		else if (result == COLOR_GLOBAL2						) {colorMode = result; globalColorHSV[1] = colorSelectMenu(globalColorHSV[1]);}
		else if (result == COLOR_GLOBAL3						) {colorMode = result; globalColorHSV[2] = colorSelectMenu(globalColorHSV[2]);}
		else if (result == COLOR_GLOBAL4						) {colorMode = result; globalColorHSV[3] = colorSelectMenu(globalColorHSV[3]);}
	}
}

char* brightnessMenuContent(int pos)
{
	if 		(pos == BRIGHT_TITLE	)	{return("Global Dimming:"		);}
	else if (pos >= BRIGHT_MINIMUM && pos <= BRIGHT_MAXIMUM	)	
	{	
		//build a string based on global brightness
		char* tempstring = "Level: "; // room for value render
		char* buffer = itoa(brightnessMode, tempChar, 10);
		byte i = 7; //length of brightness
		byte len = strlen(buffer);
		//lcdPrint(itoa(i, tempChar, 10));
		byte j = 0;
		while (j < len) {
			tempstring[i] = buffer[j];
			j++;
			i++;
		}
		tempstring[i] = '%';
		i++;
		tempstring[i] = NULL;
		return(	tempstring	);
	}
	else if (pos == BRIGHT_SIZE		)	{return("100"					);} //100%
	else								{return(errorMessage()			);}
}
void brightnessMenu()
{
	int	result = brightnessMode;
	showMenuTitle(brightnessMenuContent, BRIGHT_TITLE);
	showMenuContent(brightnessMenuContent, result);
	while (result != BRIGHT_EXIT)
	{
		result = runValueMenu(result, 5, BRIGHT_MINIMUM, BRIGHT_MAXIMUM, MENU_BEHAVIOR_STOP, BRIGHT_EXIT);
		if (result >= BRIGHT_MINIMUM && result <= BRIGHT_MAXIMUM)
			{showMenuContent(brightnessMenuContent, result); brightnessMode = result;}	
	}
}

char* speedMenuContent(int pos)
{
	if 		(pos == SPEED_TITLE	)	{return("How Active?"			);}
	else if (pos == SPEED_1		)	{return("Mellow"				);}
	else if (pos == SPEED_2		)	{return("Normal"				);}
	else if (pos == SPEED_3		)	{return("Busy"					);}
	else if (pos == SPEED_4		)	{return("Dance Party"			);}
	else if (pos == SPEED_5		)	{return("Seizure"				);}
	else if (pos == SPEED_SIZE	)	{return("5"						);}
	else							{return(errorMessage()			);}
}
void speedMenu()
{
	int	result = colorRenderClock.rate;
	while (result != SPEED_EXIT)
	{
		result = runContentMenu(speedMenuContent, result, SPEED_EXIT);
		if		(result >= SPEED_1 && result <= SPEED_5)  {colorRenderClock.rate = result;}
		else {}
	}
}

char* mainMenuContent(int pos)
{
	if 		(pos ==	MAIN_TITLE			)		{return(	"Lets change the"	);}
	else if (pos == MAIN_PATTERN		)		{return(	"Patterns"			);}
	else if (pos == MAIN_COLOR			)		{return(	"Colors"			);}
	else if (pos == MAIN_SPEED			)		{return(	"Speed"				);}
	else if (pos ==	MAIN_BRIGHTNESS		)		{return(	"Brightness"		);}
	else if (pos ==	MAIN_EXIT			)		{return(	exitMessage()		);}
	else if (pos ==	MAIN_SIZE			)		{return(	"4"					);}
	else										{return(	errorMessage()		);}
}
void mainMenu()
{
	byte	result = mainMode;
	while (result != MAIN_EXIT)
	{
		result = runContentMenu(mainMenuContent, result, MAIN_EXIT);
		if		(result == MAIN_PATTERN		) {patternMenu();}
		else if (result == MAIN_COLOR		) {colorMenu();}
		else if (result == MAIN_SPEED		) {speedMenu();		}
		else if (result == MAIN_BRIGHTNESS	) {brightnessMenu();		}
		else if (result == MAIN_EXIT		) {lcdClearScreen();	} //clean up after menu
		else {}
	}
}

void setup ()
{
				leds[1].redPin    = PIN_RED1;
	pinMode(	leds[1].redPin    , OUTPUT);
	analogWrite(leds[1].redPin    , 1);
				leds[1].greenPin  = PIN_GREEN1;
	pinMode(	leds[1].greenPin  , OUTPUT);
	analogWrite(leds[1].greenPin  , 0);
				leds[1].bluePin   = PIN_BLUE1;
	pinMode(	leds[1].bluePin   , OUTPUT);
	analogWrite(leds[1].bluePin   , 0);
		
				leds[2].redPin    = PIN_RED2;
	pinMode(	leds[2].redPin    , OUTPUT);
	analogWrite(leds[2].redPin    , 0);
				leds[2].greenPin  = PIN_GREEN2;
	pinMode(	leds[2].greenPin  , OUTPUT);
	analogWrite(leds[2].greenPin  , 1);
				leds[2].bluePin   = PIN_BLUE2;
	pinMode(	leds[2].bluePin   , OUTPUT);
	analogWrite(leds[2].bluePin   , 0);
		
				leds[3].redPin    = PIN_RED3;
	pinMode(	leds[3].redPin    , OUTPUT);
	analogWrite(leds[3].redPin    , 0);
				leds[3].greenPin  = PIN_GREEN3;
	pinMode(	leds[3].greenPin  , OUTPUT);
	analogWrite(leds[3].greenPin  , 0);
				leds[3].bluePin   = PIN_BLUE3;
	pinMode(	leds[3].bluePin   , OUTPUT);
	analogWrite(leds[3].bluePin   , 1);
			
				leds[4].redPin    = PIN_RED4;
	pinMode(	leds[4].redPin    , OUTPUT);
	analogWrite(leds[4].redPin    , 2);
				leds[4].greenPin  = PIN_GREEN4;
	pinMode(	leds[4].greenPin  , OUTPUT);
	analogWrite(leds[4].greenPin  , 2);
				leds[4].bluePin   = PIN_BLUE4;
	pinMode(	leds[4].bluePin   , OUTPUT);
	analogWrite(leds[4].bluePin   , 2);
		
 //EEPROM.write(address = bay_4_count, bay4Count = 0);
  
	pinMode(PUSH_BUTTON, INPUT_PULLUP);
	pinMode(OLED_R, OUTPUT);
	digitalWrite(OLED_R, HIGH);
	lcdInit();
    lcdClearScreen();
    lcdSetPos(0,0);
    lcdPrintln("Wes's RV Light Show");
    lcdSetPos(2,33);
    lcdPrint("By Jopel Designs");
	delay(500);
	lcdClearScreen();
}
void loop ()
{
	if (digitalRead(PUSH_BUTTON) == 0) {mainMenu(); buttonDebounce();  lcdClearScreen(); }
	delay(RENDER_RATE);	 
	//render();
}