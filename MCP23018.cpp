#include <Arduino.h>
#include <MCP23018.h>
#include <Wire.h>

MCP23018::MCP23018(int _ADR)
{
  // ADR = _ADR; //FIX ADR!
  Wire.begin();  
}


int MCP23018::begin(void)  //FIX! Combine interrupt lines be default!
{
  Wire.beginTransmission(ADR); //Test if device present 
  if(Wire.endTransmission() != 0) return -1;
  else return 1;
}


int MCP23018::PinMode(int Pin, uint8_t PinType, bool Port)
{
  if(Pin > 8 || Pin < 0)
  {
    return -1;  //Fail if pin out of range
  }

  if(PinType == INPUT)
  {
    PinModeConf[Port] = PinModeConf[Port] | (0x01 << Pin);
    PullUpConf[Port] = PullUpConf[Port] & ~(0x01 << Pin);  //Clear pullup bit
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(PinModeConf, HEX); //DEBUG!
    SetDirection(PinModeConf[Port], Port);
    SetPullup(PullUpConf[Port], Port);
    return 1;
  }
  else if(PinType == OUTPUT)
  {
    PinModeConf[Port] = PinModeConf[Port] & ~(0x01 << Pin);
    Serial.print("Reg 0x03 = ");  //DEBUG!
    Serial.println(PinModeConf[Port], HEX); //DEBUG!
    SetDirection(PinModeConf[Port], Port);
    return 0;
  }

  else if(PinType == INPUT_PULLUP)
  {
    PullUpConf[Port] = PullUpConf[Port] | (0x01 << Pin);
    // Serial.print("Reg 0x03 = ");  //DEBUG!
    // Serial.println(PinModeConf, HEX); //DEBUG!
    SetPullup(PullUpConf[Port], Port);
    return 0;
  }
  else 
    return -1; //Fail if pin type not defined 
}

int MCP23018::DigitalWrite(int Pin, bool State, bool Port)
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
    SetPort(PortState[Port], Port);
    return 1;
  }
  else if(State == LOW)
  {
    PortState[Port] = PortState[Port] & ~(0x01 << Pin);
    // Serial.print("Reg 0x01 = ");  //DEBUG!
    // Serial.println(Port, HEX); //DEBUG!
    SetPort(PortState[Port], Port);
    return 0;
  }
  else 
    return -1; //Fail if state is ill-defined
}

int MCP23018::SetInterrupt(int Pin, bool State, bool Port)
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
    SetPort(InterruptConf[Port], Port);
    return 1;
  }
  else if(State == OFF)
  {
    InterruptConf[Port] = InterruptConf[Port] & ~(0x01 << Pin);
    // Serial.print("Reg 0x01 = ");  //DEBUG!
    // Serial.println(Port, HEX); //DEBUG!
    SetPort(InterruptConf[Port], Port);
    return 0;
  }
  else 
    return -1; //Fail if state is ill-defined
}

int MCP23018::SetPort(int Config, bool Port) 
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(LATA + Port);   //Send to output set register
  Wire.write(Config);   
  Serial.println(LATA + Port, HEX); //DEBUG!
  Serial.println(Config, HEX); //DEBUG!
  Serial.print("\n\n"); //DEBUG!
  return Wire.endTransmission();
}

int MCP23018::SetDirection(int Config, bool Port) 
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(DIRA + Port);        //Send to port configuration register
  Wire.write(Config);              
  Serial.println(DIRA + Port, HEX); //DEBUG!
  Serial.println(Config, HEX); //DEBUG!
  Serial.print("\n\n"); //DEBUG!
  return Wire.endTransmission();    // stop transmitting
}

int MCP23018::SetPolarity(int Config, bool Port) 
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(POLA + Port);        //Send to port configuration register
  Wire.write(Config);              
  return Wire.endTransmission();    // stop transmitting
}

int MCP23018::SetPullup(int Config, bool Port) 
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(PULLUPA + Port);        //Send to port configuration register
  Wire.write(Config);         
  return Wire.endTransmission();    // stop transmitting
}

int MCP23018::SetInt(int Config, bool Port) 
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(PULLUPA + Port);        //Send to port configuration register
  Wire.write(Config);         
  return Wire.endTransmission();    // stop transmitting
}

//IN DEVELOPMENT
int MCP23018::ReadPort(int Config, bool Port)
{
  Wire.beginTransmission(ADR); // transmit to device with address ADR
  Wire.write(0x01);        //Send to port configuration register
  Wire.write(Config);              //Set port 1-4 as OUTPUT, all others as inputs
  return Wire.endTransmission();    // stop transmitting
}





