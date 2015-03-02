/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Mega 2560 or Mega ADK, Platform=avr, Package=arduino
*/

#define __AVR_ATmega2560__
#define ARDUINO 105
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
extern "C" void __cxa_pure_virtual() {;}

void twiInit(void);
void twiStart(void);
void twiStop(void);
void twiSend(uint8_t u8data);
void twiSendCmd(uint8_t command);
void lcdSendCommand(uint8_t command);
void lcdInit(void);
void lcdSetPos(uint8_t row, uint8_t column);
void lcdFill(char fillData);
void lcdClearLine(uint8_t line);
void lcdClearScreen();
void lcdWriteChar(uint8_t data);
void lcdPrint(char *buffer);
void lcdPrintln(char *buffer);
void buttonDebounce();
int a2i(char *s);
int a2iSigned(char *s);
void hsvToRgb(HSV input, RGB *returnValue);
void clockUpdate();
void updateLedOutputs();
void updateLEDsColorSingle (struct HSV inputHSV);
uint8_t patternRender(RGB *input, int ledNumber);
HSV colorRandomHSV(int ledNumber);
HSV colorSequenceRender(int ledNumber);
void colorValueRender(uint8_t intensity,  int ledNumber, RGB *returnValue);
void updateLEDs ();
uint8_t valueMenu (uint8_t *startingValue, int increment, int minimum, int maximum, uint8_t rolloverBOOL);
char* errorMessage ();
char* exitMessage ();
char* patternMenuContent(uint8_t pos);
void patternMenu();
int customColorMenu (int startHue);
char* colorSelectContent(uint8_t pos);
char* colorSelectContentSmall(uint8_t pos);
void colorSelectMenu(uint8_t pos, RGB *returnValue);
char* colorBehaviorMenuContent(uint8_t pos);
void colorBehaviorMenu();
char* brightnessMenuContent(uint8_t pos);
void brightnessMenu();
char* speedMenuContent(uint8_t pos);
void speedMenu();
char* mainMenuContent(uint8_t pos);
void mainMenu();
//
//

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\mega\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\Jesse\Desktop\Jopel Designs\Projects\RVstripController\RV_software\RV_software.ino"
