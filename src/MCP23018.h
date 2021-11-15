/******************************************************************************
MCP23018.h
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

#ifndef MCP23018_h
#define MCP23018_h

#include <Arduino.h>
#include <Wire.h>

#define ON 1
#define OFF 0

#define A 0
#define B 1
#define BOTH 2

#define OPEN_DRAIN 3  //4th potential state of operation for IO expander pins

#define BASE_ADR 0x20 //Base address for device, to be or-ed with configurable address

#define DIRA 0x00
#define DIRB 0x01
#define POLA 0x02 
#define POLB 0x03
#define INTENA 0x04
#define INTENB 0x05
#define PULLUPA 0x0C
#define PULLUPB 0x0D
#define PORTA 0x12
#define PORTB 0x13
#define LATA 0x14
#define LATB 0x15

#define DEFVALA 0x06
#define DEFVALB 0x07
#define IOCON 0x0A //Also 0x0B
#define FLAGA 0x0E
#define FLAGB 0x0F
#define INTCAPA 0x10
#define INTCAPB 0x11
#define INTCONA 0x08
#define INTCONB 0x09

class MCP23018
{
    //     enum IO_Type
    // {
    //     INPUT,
    //     OUTPUT
    // };

  public:
    MCP23018(int _ADR = BASE_ADR); //Default to base address if none specified 
    int begin(void);
    int pinMode(int Pin, uint8_t State, bool Port);
    int pinMode(int Pin, uint8_t State);
    int digitalWrite(int Pin, bool State, bool Port);
    int digitalWrite(int Pin, bool State);
    int digitalRead(int Pin, bool Port);
    int digitalRead(int Pin);
    int setInterrupt(int Pin, bool State, bool Port);
    int setInterrupt(int Pin, bool State);
    int getInterrupt(int Pin);
    unsigned int clearInterrupt(int Port);
    int setInputPolarity(int Pin, bool State, bool Port);
    int setInputPolarity(int Pin, bool State);
    int setIntPinConfig(int Pin, bool OnChange, bool DefVal = 0);
    int setIntConfig(bool Mirror, bool OpenDrain = 0, bool Polarity = 0, bool Clearing = 0);
    unsigned int readBus(); 


  private:
    int ADR = BASE_ADR; //FIX! Replace with equation later
  	uint8_t PinModeConf[2] = {0xFF, 0xFF}; //All pins natively inputs (IODIRx)
    uint8_t PortState[2] = {0}; //All pins natively off (LATx)
    uint8_t PinPolarityConfig[2] = {0x00}; //All pins natively non-inverted (IPOLx)
    uint8_t PullUpConf[2] = {0x00}; //Natively disabled (GPPUx)
    uint8_t InterruptConf[2] = {0x00}; //Interrupts dissabled by default (GPINTENx)
    uint8_t DefaultValConf[2] = {0x00}; //Default interrupt value (DEFVALx)
    uint8_t InterruptTypeConf[2] = {0x00}; //Interrupt type values (INTCONx)

    int setPort(int Config, bool Port);
    int setDirection(int Config, bool Port);
    int setPolarity(int Config, bool Port);
    int setPullup(int Config, bool Port);
    int setInt(int Config, bool Port);
    int readPort(bool Port); 
    int readByte(int Pos);
    int writeByte(int Pos, uint8_t Val);
    uint8_t clearBit(uint8_t Val, uint8_t Pos);
    
};

#endif