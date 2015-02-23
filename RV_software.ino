// RV led controller 
// 2/23/15
#define F_CPU 16000000UL

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

typedef struct ledOutput {
	int red;
	int green;
	int blue;
	uint8_t	redPin;
	uint8_t greenPin;
	uint8_t bluePin;
	};
	
ledOutput leds[5];

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
  leds[3].redPin	=	PIN_RED3;
  leds[3].greenPin	= PIN_GREEN3;
  leds[3].bluePin	=  PIN_BLUE3;
  leds[4].redPin	=	PIN_RED4;
  leds[4].greenPin	= PIN_GREEN4;
  leds[4].bluePin	=  PIN_BLUE4;
  
  pinMode(leds[1].redPin,OUTPUT);
  pinMode(leds[1].greenPin,OUTPUT);
  pinMode(leds[1].bluePin,OUTPUT);
  pinMode(leds[2].redPin,OUTPUT);
  pinMode(leds[2].greenPin,OUTPUT);
  pinMode(leds[2].bluePin,OUTPUT);
  pinMode(leds[3].redPin,OUTPUT);
  pinMode(leds[3].greenPin,OUTPUT);
  pinMode(leds[3].bluePin,OUTPUT);
  pinMode(leds[4].redPin,OUTPUT);
  pinMode(leds[4].greenPin,OUTPUT);
  pinMode(leds[4].bluePin,OUTPUT);
   
}
void loop ()
{
  //analogWrite(leds[0].redPin,0);
}
