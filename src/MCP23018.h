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

#if  !defined(OUTPUT_OPEN_DRAIN)
#define OUTPUT_OPEN_DRAIN 5
#endif
#define ON 1
#define OFF 0

// #define A 0
// #define B 1
// #define BOTH 2



// #define OPEN_DRAIN 3  //4th potential state of operation for IO expander pins
#define MCP23018_BASE_ADR 0x20 //Base address for device, to be or-ed with configurable address


class MCP23018
{
    //     enum IO_Type
    // {
    //     INPUT,
    //     OUTPUT
    // };


  public:
    MCP23018(int _ADR = MCP23018_BASE_ADR); //Default to base address if none specified 
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
    enum Ports: int
    // namespace Ports
    {
        // constexpr uint8_t CURRENT = 0;
        // constexpr uint8_t STALE = 1;
        // constexpr uint8_t BOTH = 2;
        A = 0,
        B = 1,
        BOTH = 2,
    };

  private:

    
    // enum Regs: const int
    // // namespace Ports
    // {
    //     // constexpr uint8_t CURRENT = 0;
    //     // constexpr uint8_t STALE = 1;
    //     // constexpr uint8_t BOTH = 2;
    //     PORTA = 0x12,
    //     PORTB = 0x13
    // };
    const int DIRA = 0x00;
    const int DIRB = 0x01;
    const int POLA = 0x02; 
    const int POLB = 0x03;
    const int INTENA = 0x04;
    const int INTENB = 0x05;
    const int PULLUPA = 0x0C;
    const int PULLUPB = 0x0D;
    const int PORTA_REG = 0x12;
    const int PORTB_REG = 0x13;
    const int LATA = 0x14;
    const int LATB = 0x15;

    const int DEFVALA = 0x06;
    const int DEFVALB = 0x07;
    const int IOCON = 0x0A; //Also = 0x0B
    const int FLAGA = 0x0E;
    const int FLAGB = 0x0F;
    const int INTCAPA = 0x10;
    const int INTCAPB = 0x11;
    const int INTCONA = 0x08;
    const int INTCONB = 0x09;

    int ADR = MCP23018_BASE_ADR; //FIX! Replace with equation later
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