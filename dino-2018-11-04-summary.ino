/*	Arduino sketch DINO, to wag Dino's tail (on Flintstone car) when a visitor gets close enough
2018-11-05 cewood, posted to  https://github.com/clarkclark/Dino
        Arduino device to augment Jack Butterfield's "Flintstone Car" art car.
        The car features a doghouse behind the back window that holds the battery, gas tank, and air compressor.
        The doghouse also includes Dino's head at one end and feet and tail at the other.
        Goal is to detect a visitor approaching Dino's head and then play a recording of Dino barking 
            and trigger a windshield wiper motor to wag Dino's tail, for five seconds.
	Built from an Arduino Nano clone, SR-04 ultrasonic transceiver, relay module, DFPlayerMini
	thanks, R.Girish, www.homemade-circuits.com/category/microcontroller-projects

2018-08-14 modified to use DFrobot LCD shield
	with LCD shield, our available analog pins are range A1-A4 (change from Girish)
	(DFrobot LCD shield uses pins different from Garish's hand-wired LCD)
	first step is to read the ultrasonic and display results
	success reading distance and displaying it in inches & feet...
2018-08-16 second step is to DO something when distance is < CloseEnough
	for example, turn on TailWagPin and leave it on for WagTime seconds, then turn it off.
	success triggering the output pin!
	I will leave the support for LCD shield in for troubleshooting
2018-08-20 adding on-board LED to pulse at each scan and stay solid while CloseEnough
	to verify the sketch is running, even on a bare board.
	On the NANO, setting a digital pin LOW does NOT provide a competent ground to run the relay (Girish's design).
	Get voltage for that relay board from the SPI pins:
	vcc from pin 6 (closest to A pins)	-	ground from ICSP pin 4 (closest to D pins)
2018-08-23 corrected speed of sound from 13393 ips to 13504 ips
2018-09-03 briefly ground pin D10 to trigger recording of Dino's bark on DFPlayerMini module
2018-09-05 the relay boards activate with a low signal, so I'm reversing the levels on pin D3
	This means BOTH outputs (sound player & wag motor relay) are activated with a LOW signal
	constructing a single-board Dino machiine, I had to change the tailwag pin to 11
*/

  // LIBRARY... calling library
#include <LiquidCrystal.h>

  // VARIABLES... declaring global variables & initial values

  // physical (pin) inputs & outputs
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);	// select the pins used by the LCD PANEL
int vcc = A1;				// put 5VDC on this ANALOG pin (to match SR04 sensor)
int trigger = A2;			// here's how we initiate the ultrasonic pulse
int echo = A3;   			// this returns our ultrasonic reflection
int gnd = A4;				// and this ANALOG pin is nailed to 0 VDC for the SR04
int TailWagPin = 3;			// this DIGITAL pin goes LOW when a person is CloseEnough
int Barking = 11;			// to trigger Dino's bark on DFPlayerMini module
// int AlsoGnd = 2;			// NOT a competent ground to drive the relay board  (on Nano)

// declaring variables that AREN'T physical inputs, and assign initial values when it mattters
long Time;				// the number of microseconds the reflection takes
float distanceIN = 1000;		// floating point to allow decimal fractions
float distanceFT;			// floating point to allow decimal fractions
int CloseEnough = 36;			// initially 36 inches. Change the trigger distance here
int WagTime = 5000;			// how long to wag Dino's tail in MILISECONDS

// SETUP... setup code, to run once
void setup()
{
  lcd.begin(16,2);			// define writable LCD space
  pinMode(vcc,OUTPUT);			// pin A1
  pinMode(echo,INPUT);			// pin A2
  pinMode(trigger,OUTPUT);		// pin A3
  pinMode(gnd,OUTPUT);			// pin A4
  pinMode(LED_BUILTIN, OUTPUT);		// pin D13 (on-board LED)
  pinMode(TailWagPin,OUTPUT);		// pin D3
  pinMode(Barking,OUTPUT);		// pin D10
//  pinMode(AlsoGnd,OUTPUT);		// pin D2 to ground DOESN'T work on the Nano

  digitalWrite(vcc,HIGH);		// permanent assignment
  digitalWrite(trigger,LOW);		// just to begin
  digitalWrite(gnd,LOW);		// permanent assignment
  digitalWrite(TailWagPin,HIGH);	// just to begin, LOW level engages wagging motor relay
  digitalWrite(Barking,HIGH);		// just to begin, grounding triggers bark
//  digitalWrite(AlsoGnd,LOW);		// permanent assignment (fails on the Nano)
}

  // MAIN LOOP... main code, to loop:
void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);	// turn the on-board LED on during the scan
  digitalWrite(trigger,HIGH);		// triggers ultrasonic transmission
  delayMicroseconds(10);		// pulse the transmitter for 10 microseconds
  digitalWrite(trigger,LOW);		// stop transmitting ultrasonic
  Time=pulseIn(echo,HIGH);		// count how long it takes to receive the reflection
		// sound travels 13504 inches per second, or .0135 inches per microsecond
		// but Time counts the round trip, so divide by 2 for actual inch distance
  distanceIN=Time * 0.0135 * .5;	
  distanceFT=distanceIN/12;		// display actual distance in feet
  digitalWrite(LED_BUILTIN, LOW);	// turn the on-board LED off by making the voltage LOW

  lcd.setCursor(0,0);			// set the LCD cursor position, first column of upper row
  lcd.print("Distance:");
  lcd.print(distanceFT);
  lcd.print("ft ");			// trailing space prevents 'ftt' display as number changes
  lcd.setCursor(0,1);			// set the LCD cursor position, first column of lower row
  lcd.print("That is: ");
  lcd.print(distanceIN);
  lcd.print("in ");			// trailing space prevents 'inn' display

  if (distanceIN < CloseEnough) {
  	lcd.setCursor(0,1);
  	lcd.print("CLOSE ENOUGH   *");		// that's 16 characters
  	digitalWrite(TailWagPin, LOW);		// set the output pin LOW to trigger the relay
  	digitalWrite(LED_BUILTIN, HIGH);	// turn the on-board LED on for entire WagTime
  	digitalWrite(Barking, LOW);		// turn on the output pin to trigger barking on 
	delay(250);				// just a brief tap on the player button
  	digitalWrite(Barking, HIGH);		// the brief tap is over
  	delay(WagTime);				// how long to wag? change the value in 'variables'
  	digitalWrite(TailWagPin, HIGH);		// HIGH to turn off the output pin & stop the relay
  	digitalWrite(LED_BUILTIN, LOW);		// turn the on-board LED off
  	lcd.setCursor(0,1);
  	lcd.print("                ");		// that's 16 blank characters
	}

  delay(1000);					// wait a second, then start the loop over
}
//--------------------------- end-----------------------------------//
