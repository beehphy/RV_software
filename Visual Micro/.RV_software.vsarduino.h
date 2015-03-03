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
void twiSend(byte u8data);
void twiSendCmd(byte command);
void lcdInit(void);
void lcdSetPos(byte row, byte column);
void lcdFill(char fillData);
void lcdClearLine(byte line);
void lcdClearScreen();
void lcdWriteChar(byte data);
void lcdPrint(char *buffer);
void lcdPrintln(char *buffer);
void displayBattery (byte line);
//
//

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\mega\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\Jesse\Desktop\Jopel Designs\Projects\RVstripController\RV_software\RV_software.ino"
