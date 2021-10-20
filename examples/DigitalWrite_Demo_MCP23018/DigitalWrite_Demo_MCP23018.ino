/******************************************************************************
digitalWrite_Demo_MCP23018.ino
Demo to demonstrate the utilization of the interface for the MCP23018
Bobby Schulz @ Northern Widget LLC
10/14/2019
https://github.com/NorthernWidget-Skunkworks/MCP23018

Toggles the states of the pins B0, B1, A0 at variable rates to demonstrate basic use of the digital write functionality. 
Connect LEDs or other indicator to these pins to observe output

0.0.0

"All existing things are really one"
-Zhuangzi

Distributed as-is; no warranty is given.
******************************************************************************/

#include <MCP23018.h>

MCP23018 IO(0x20);
const unsigned long PeriodMain = 1000; //Primary period is 1000ms
const unsigned long PeriodSecondary = 250; //Secondary period is 250ms

void setup() {
	IO.begin();
	IO.pinMode(0, OUTPUT, B); //Set pin 0, port B as output
	IO.pinMode(1, OUTPUT, B); //Set pin 1, port B as output
	IO.pinMode(0, OUTPUT, A); //Set pin 0, port A as output


	for(int i = 0; i < 5; i++) {  //DEBUG!
		IO.digitalWrite(0,1,B);
		delay(100);
		IO.digitalWrite(0,0,B);
		delay(100);
	}

}

void loop() {

	static boolean StateMain = false; //Equivelent to off
	static boolean StateSecondary = false; //Equivelent to off

	while(1) {
    static unsigned long TripTimeMain = 0; //Last time the function was serviced 
    static unsigned long TripTimeSecondary = 0; //Last time the function was serviced 
		
		if((millis() - TripTimeMain) > PeriodMain) { //If main period rolls over
			TripTimeMain = millis(); //Reset trip time
			StateMain = !StateMain; //Toggle state 
			// IO.digitalWrite(0, int(StateMain), B); //Set state value
			// IO.digitalWrite(0, int(!StateMain), A);  //Set inverse state value
		    IO.digitalWrite(0, int(StateMain)); //Set state value
       		IO.digitalWrite(8, int(!StateMain));  //Set inverse state value
		}
		if((millis() - TripTimeSecondary) > PeriodSecondary) { //If secondary period rolls over
			TripTimeSecondary = millis(); //Reset trip time
			StateSecondary = !StateSecondary; //Toggle state 
			IO.digitalWrite(1, StateSecondary, B); //Set state value
		}
	}
}
