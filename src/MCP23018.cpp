/******************************************************************************
MCP23018.cpp
Interface for MCP23018 16 bit IO expander
Bobby Schulz @ Northern Widget LLC
07/18/2019
https://github.com/NorthernWidget-Skunkworks/MCP23018

Allows control of all aspects of the control of the IO expander 

1.0.0

"All existing things are really one"
-Zhuangzi

Distributed as-is; no warranty is given.
******************************************************************************/

// #include <Arduino.h>
#include <MCP23018.h>
// #include <Wire.h>

MCP23018::MCP23018(int _ADR)
{
  ADR = _ADR; //FIX ADR!
}


int MCP23018::begin(void)  //FIX! Combine interrupt lines be default!
{
  #if defined(ARDUINO) && ARDUINO >= 100 
    Wire.begin();
  #elif defined(PARTICLE)
    if(!Wire.isEnabled()) Wire.begin(); //Only initialize I2C if not done already //INCLUDE FOR USE WITH PARTICLE 
  #endif
  pinModeConf[0] = 0xFF; //Default to all inputs //FIX make cleaner
  pinModeConf[1] = 0xFF; 

  Wire.beginTransmission(ADR); //Test if device present 
  if(Wire.endTransmission() != 0) return -1;
  else return 1;
}


int MCP23018::pinMode(int Pin, uint8_t PinType, bool Port)
{
  if(Pin > 8 || Pin < 0)
  {
    return -1;  //Fail if pin out of range
  }

  if(PinType == INPUT)
  {
    pinModeConf[Port] = pinModeConf[Port] | (0x01 << Pin); //Set bit for input
    PullUpConf[Port] = PullUpConf[Port] & ~(0x01 << Pin);  //Clear bit for pullup 
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(pinModeConf, HEX); //DEBUG!
    setDirection(pinModeConf[Port], Port);
    setPullup(PullUpConf[Port], Port);
    return 1;
  }
  else if(PinType == OUTPUT)
  {
    pinModeConf[Port] = pinModeConf[Port] & ~(0x01 << Pin); //Clear bit for output
    PullUpConf[Port] = PullUpConf[Port] | (0x01 << Pin);  //Set pullup bit to allow for "push-pull" operation
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(pinModeConf[Port], HEX); //DEBUG!
    // Serial.println(PullUpConf[Port], HEX); //DEBUG!
    setDirection(pinModeConf[Port], Port);
    setPullup(PullUpConf[Port], Port);
    return 0;
  }

  else if(PinType == OPEN_DRAIN)
  {
    pinModeConf[Port] = pinModeConf[Port] & ~(0x01 << Pin); //Clear bit for output
    PullUpConf[Port] = PullUpConf[Port] & ~(0x01 << Pin);  //Clear pullup bit to allow for open drain operation
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(pinModeConf[Port], HEX); //DEBUG!
    setDirection(pinModeConf[Port], Port);
    setPullup(PullUpConf[Port], Port);
    return 0;
  }

  else if(PinType == INPUT_PULLUP)
  {
    pinModeConf[Port] = pinModeConf[Port] | (0x01 << Pin); //Set bit for input
    PullUpConf[Port] = PullUpConf[Port] | (0x01 << Pin);  //Set bit for pullup  
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(pinModeConf, HEX); //DEBUG!
    setDirection(pinModeConf[Port], Port); 
    setPullup(PullUpConf[Port], Port);
    return 0;
  }
  else 
    return -1; //Fail if pin type not defined 
}

int MCP23018::pinMode(int Pin, uint8_t PinType)
{
  if(Pin > 15 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  if(Pin >= 8) {
    return pinMode(Pin - 8, PinType, B); //Shift pin number, pass along to set port B
  }
  if(Pin <= 7) {
    return pinMode(Pin, PinType, A); //Pass along to set port A
  }
  return -1; //Fail is state is ill-defined
}

int MCP23018::digitalWrite(int Pin, bool State, bool Port)
{
  if(Pin > 8 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  if(State == HIGH)
  {
    PortState[Port] = PortState[Port] | (0x01 << Pin);
    // Serial.print("Reg 0x01 = ");  //DEBUG!
    // Serial.println(Port, HEX); //DEBUG!
    setPort(PortState[Port], Port);
    return 1;
  }
  else if(State == LOW)
  {
    PortState[Port] = PortState[Port] & ~(0x01 << Pin);
    // Serial.print("Reg 0x01 = ");  //DEBUG!
    // Serial.println(Port, HEX); //DEBUG!
    setPort(PortState[Port], Port);
    return 0;
  }
  else 
    return -1; //Fail if state is ill-defined
}

int MCP23018::digitalWrite(int Pin, bool State)
{
  if(Pin > 15 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  if(Pin >= 8) {
    return digitalWrite(Pin - 8, State, B); //Shift pin number, pass along to set port B
  }
  if(Pin <= 7) {
    return digitalWrite(Pin, State, A); //Pass along to set port A
  }
  return -1; //Fail is state is ill-defined
}

int MCP23018::setInterrupt(int Pin, bool State, bool Port)
{
  if(Pin > 8 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  if(State == ON)
  {
    InterruptConf[Port] = InterruptConf[Port] | (0x01 << Pin);
    // Serial.print("Reg 0x01 = ");  //DEBUG!
    // Serial.println(Port, HEX); //DEBUG!
    setPort(InterruptConf[Port], Port);
    return 1;
  }
  else if(State == OFF)
  {
    InterruptConf[Port] = InterruptConf[Port] & ~(0x01 << Pin);
    // Serial.print("Reg 0x01 = ");  //DEBUG!
    // Serial.println(Port, HEX); //DEBUG!
    setPort(InterruptConf[Port], Port);
    return 0;
  }
  else 
    return -1; //Fail if state is ill-defined
}

int MCP23018::setInterrupt(int Pin, bool State)
{
  if(Pin > 15 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  if(Pin >= 8) {
    return setInterrupt(Pin - 8, State, B); //Shift pin number, pass along to set port B
  }
  if(Pin <= 7) {
    return setInterrupt(Pin, State, A); //Pass along to set port A
  }
  return -1; //Fail is state is ill-defined
}

int MCP23018::setPort(int Config, bool Port) 
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(LATA + Port);   //Send to output set register
  Wire.write(Config);   
  // Serial.println(LATA + Port, HEX); //DEBUG!
  // Serial.println(Config, HEX); //DEBUG!
  // Serial.print("\n\n"); //DEBUG!
  return Wire.endTransmission();
}

int MCP23018::setDirection(int Config, bool Port) 
{
  // Serial.println(Config, HEX); //DEBUG!
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(DIRA + Port);        //Send to port configuration register
  Wire.write(Config);              
  // Serial.println(DIRA + Port, HEX); //DEBUG!
  // Serial.print("\n\n"); //DEBUG!
  return Wire.endTransmission();    // stop transmitting
}

int MCP23018::setPolarity(int Config, bool Port) 
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(POLA + Port);        //Send to port configuration register
  Wire.write(Config);              
  return Wire.endTransmission();    // stop transmitting
}

int MCP23018::setPullup(int Config, bool Port) 
{
  // Serial.println(Config, HEX); //DEBUG!
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(PULLUPA + Port);        //Send to port configuration register
  Wire.write(Config);         
  return Wire.endTransmission();    // stop transmitting
}

int MCP23018::setInt(int Config, bool Port) 
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(PULLUPA + Port);        //Send to port configuration register
  Wire.write(Config);         
  return Wire.endTransmission();    // stop transmitting
}

//IN DEVELOPMENT
int MCP23018::readPort(int Config, bool Port)
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(0x01);        //Send to port configuration register
  Wire.write(Config);              //Set port 1-4 as OUTPUT, all others as inputs
  return Wire.endTransmission();    // stop transmitting
}





