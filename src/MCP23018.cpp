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
  // Wire.setClock(400000); //DEBUG!
  // PinModeConf[0] = 0xFF; //Default to all inputs //FIX make cleaner
  // PinModeConf[1] = 0xFF; 

  for(int i = 0; i < 2; i++) { //Load current state
    PinModeConf[i] = readByte(DIRA + i); //Read in direction regs
    PortState[i] = readByte(LATA + i); //Read in LAT regs
    PinPolarityConfig[i] = readByte(POLA + i); //Read in input polarity regs
    PullUpConf[i] = readByte(PULLUPA + i); //Read in pullup regs
    InterruptConf[i] = readByte(INTENA + i); //Read in interrupt enable regs
    DefaultValConf[i] = readByte(DEFVALA + i); //Read in default interrupt value registers
    InterruptTypeConf[i] = readByte(INTCONA + i); //Read in interrupt mode registers
  }

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
    PinModeConf[Port] = PinModeConf[Port] | (0x01 << Pin); //Set bit for input
    PullUpConf[Port] = PullUpConf[Port] & ~(0x01 << Pin);  //Clear bit for pullup 
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(PinModeConf, HEX); //DEBUG!
    setDirection(PinModeConf[Port], Port);
    setPullup(PullUpConf[Port], Port);
    return 1;
  }
  else if(PinType == OUTPUT)
  {
    PinModeConf[Port] = PinModeConf[Port] & ~(0x01 << Pin); //Clear bit for output
    PullUpConf[Port] = PullUpConf[Port] | (0x01 << Pin);  //Set pullup bit to allow for "push-pull" operation
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(PinModeConf[Port], HEX); //DEBUG!
    // Serial.println(PullUpConf[Port], HEX); //DEBUG!
    setDirection(PinModeConf[Port], Port);
    setPullup(PullUpConf[Port], Port);
    return 0;
  }

  else if(PinType == OUTPUT_OPEN_DRAIN)
  {
    PinModeConf[Port] = PinModeConf[Port] & ~(0x01 << Pin); //Clear bit for output
    PullUpConf[Port] = PullUpConf[Port] & ~(0x01 << Pin);  //Clear pullup bit to allow for open drain operation
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(PinModeConf[Port], HEX); //DEBUG!
    setDirection(PinModeConf[Port], Port);
    setPullup(PullUpConf[Port], Port);
    return 0;
  }

  else if(PinType == INPUT_PULLUP)
  {
    PinModeConf[Port] = PinModeConf[Port] | (0x01 << Pin); //Set bit for input
    PullUpConf[Port] = PullUpConf[Port] | (0x01 << Pin);  //Set bit for pullup  
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(PinModeConf, HEX); //DEBUG!
    setDirection(PinModeConf[Port], Port); 
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

int MCP23018::digitalRead(int Pin, bool Port)
{
  if(Pin > 8 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  return (readPort(Port) >> Pin) & 0x01; //Return selected bit of the port value
}

int MCP23018::digitalRead(int Pin)
{
  if(Pin > 15 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  if(Pin >= 8) {
    return digitalRead(Pin - 8, B); //Shift pin number, pass along to set port B
  }
  if(Pin <= 7) {
    return digitalRead(Pin, A); //Pass along to set port A
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
    writeByte(INTENA + Port, InterruptConf[Port]); //Write adjusted value back
    return 1;
  }
  else if(State == OFF)
  {
    InterruptConf[Port] = InterruptConf[Port] & ~(0x01 << Pin);
    // Serial.print("Reg 0x01 = ");  //DEBUG!
    // Serial.println(Port, HEX); //DEBUG!
    writeByte(INTENA + Port, InterruptConf[Port]); //Write adjusted value back
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

int MCP23018::getInterrupt(int Pin)
{
  if(Pin > 15 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  if(Pin >= 8) {
    return (readByte(FLAGB) >> (Pin - 8)) & 0x01; //Return flag output
  }
  if(Pin <= 7) {
    return (readByte(FLAGA) >> Pin) & 0x01; //Return flag output
  }
  return -1; //Fail is state is ill-defined
}

unsigned int MCP23018::clearInterrupt(int Port)
{
  switch(Port){ //Read port to clear interrupts and returns pin state 
    case A:
      return readByte(PORTA);
      break;
    case B:
      return readByte(PORTB) << 8;
      break;
    case BOTH:
      return (readByte(PORTB) << 8) | readByte(PORTA);
      break;
    default:
      return 0;
      break;
  }
}

//IN DEVLOPMENT
int MCP23018::setInputPolarity(int Pin, bool State, bool Port)
{
  return -1; 
}

//IN DEVLOPMENT
int MCP23018::setInputPolarity(int Pin, bool State) 
{
  return -1;
}

int MCP23018::readPort(bool Port)
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(PORTA + Port);        //Address appropriate port register
  Wire.endTransmission();    // stop transmitting

  Wire.requestFrom(ADR, 1);
  return Wire.read();
}

//Pin -> The pin (0~15) which should be configured, OnChange -> if true, trigger interrupt on change of value, if false, trigger when different than default value, DefVal -> (optional) the default value to compare against
int MCP23018::setIntPinConfig(int Pin, bool OnChange, bool DefVal)
{
  // uint8_t DefVals = 0; //Used to store the existing default value configuration of the port in question
  // uint8_t Control = 0; //Used to store the existing int control register configuration of the port in question

  if(Pin > 15 || Pin < 0)
  {
    return -1; //Fail if pin out of range
  }

  if(Pin >= 8) {
    // Control = readByte(INTCONB); //Read in existing control values
    // DefVals = readByte(DEFVALB); //Read in existing default values
    InterruptTypeConf[B] = clearBit(InterruptTypeConf[B], Pin - 8); //Clear existing bit
    InterruptTypeConf[B] = InterruptTypeConf[B] | ((!OnChange) << (Pin - 8)); //Apply new value
    DefaultValConf[B] = clearBit(DefaultValConf[B], Pin - 8); //Clear existing bit
    DefaultValConf[B] = DefaultValConf[B] | (DefVal << (Pin - 8)) ; //Apply new value
    writeByte(INTCONB, InterruptTypeConf[B]); //Write control value back
    return writeByte(DEFVALB, DefaultValConf[B]); //Write default values back
  }
  if(Pin <= 7) {
    // Control = readByte(INTCONA); //Read in existing control values
    // DefVals = readByte(DEFVALA); //Read in existing default values
    InterruptTypeConf[A] = clearBit(InterruptTypeConf[A], Pin); //Clear existing bit
    InterruptTypeConf[A] = InterruptTypeConf[A] | ((!OnChange) << Pin); //Apply new value
    DefaultValConf[A] = clearBit(DefaultValConf[A], Pin); //Clear existing bit
    DefaultValConf[A] = DefaultValConf[A] | (DefVal << Pin) ; //Apply new value
    writeByte(INTCONA, InterruptTypeConf[A]); //Write control value back
    return writeByte(DEFVALA, DefaultValConf[A]); //Write default values back
  }
  return -1; //Fail is state is ill-defined
}

int MCP23018::setIntConfig(bool Mirror, bool OpenDrain, bool Polarity, bool Clearing)
{
  uint8_t Config = 0x00 | (Mirror << 6) | (OpenDrain << 2) | (Polarity << 1) | Clearing; //Assembly config register 
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(IOCON);        //Send to port configuration register
  Wire.write(Config);         
  return Wire.endTransmission();    // stop transmitting
}

unsigned int MCP23018::readBus()
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(PORTA);        //Address appropriate port register
  Wire.endTransmission();    // stop transmitting

  Wire.requestFrom(ADR, 2);
  unsigned int LowByte = Wire.read();
  unsigned int HighByte = Wire.read();

  return (HighByte << 8) | LowByte;
}


int MCP23018::readByte(int Pos)
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(Pos);        //Address appropriate register
  Wire.endTransmission();    // stop transmitting

  Wire.requestFrom(ADR, 1);
  return Wire.read();
}

int MCP23018::writeByte(int Pos, uint8_t Val)
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(Pos);        //Address appropriate register
  Wire.write(Val);
  return Wire.endTransmission();    // stop transmitting
}

uint8_t MCP23018::clearBit(uint8_t Val, uint8_t Pos)
{
  return Val & ~(0x01 << Pos); //Return adjusted byte
}





