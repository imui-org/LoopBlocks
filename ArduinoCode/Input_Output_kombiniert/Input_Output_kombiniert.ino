// Input shift registers transfering data to Pure Data
// Written by Alexandros Drymonitis
// This code was written for the "Arduino for Pd'ers" tutorial
// and is in the public domain

#include <SPI.h>

const byte latchin = 9;

// set here the number of chips
const byte numberOfChipsin = 14;
// array to get bytes from SPI
byte switchData[numberOfChipsin] = { 0 };
// array and size of data to be transferred to Pd
const byte numOfData = (numberOfChipsin * 2) + 1;
byte transferData[numOfData];

// function that calls SPI.transfer to transfer data from chips
void refreshSwitches()
{
  digitalWrite(latchin, LOW);
  digitalWrite(latchin, HIGH);
  for(int i = 0; i < numberOfChipsin; i++)
    switchData[i] = SPI.transfer(0);
}

//output:
const byte latchout = 10;

// set here the number of chips
const byte numberOfChipsout = 2;
// declare and initialize LED array
byte LEDdata[numberOfChipsout] = { 0 };

// variable to hold incoming data temporarily
int temporary;
// variables for data diffusion
int chip;
byte pin, state;

// function that calls SPI.transfer to transfer data to chips
void refreshLEDs()
{
  digitalWrite(latchout, LOW);
  for(int i = numberOfChipsout - 1; i >= 0; i--)
    SPI.transfer(LEDdata[i]); 
  digitalWrite(latchout, HIGH);
}


void setup()
{
  pinMode(latchin, OUTPUT);
  digitalWrite(latchin, LOW);
  Serial.begin(115200);
  SPI.begin();
  refreshLEDs();
}

void loop()
{
  transferData[0] = 0xc0;
  int index = 1;
  refreshSwitches();
  for(int i = 0; i < numberOfChipsin; i++){
    transferData[index++] = switchData[i] & 0x7f;
    transferData[index++] = switchData[i] >> 7;
  }
  Serial.write(transferData, numOfData);

  while(Serial.available()){
    byte inByte = Serial.read();
    if((inByte >= '0') && (inByte <= '9'))
      temporary = 10 * temporary + inByte - '0';
    else{
      switch(inByte){
        // set byte to store to LEDdata array
        case 'l':
          // subtract one so that the Pd patch is more intuitive
          temporary -= 1;
          // divide by 8 to work out which chip
          chip = temporary / 8;
          // remainder is pin
          pin = temporary % 8;
          // reset temporary
          temporary = 0;
          break;
        // set state of LED and write bit to byte
        case 's':
          state = temporary;
          // write bit to array byte
          bitWrite(LEDdata[chip], pin, state);
          // reset temporary
          temporary = 0;  
          break;
         // C: clear all bits
        case 'C':
          for(int i = 0; i < numberOfChipsout; i++) 
            LEDdata[i] = 0;
          break;
        // S: set all bits
        case 'S':
          for(int i = 0; i < numberOfChipsout; i++) 
            LEDdata[i] = 0xFF;
           break;
        // I: invert all bits
        case 'I':
          for(int i = 0; i < numberOfChipsout; i++) 
            LEDdata[i] ^= 0xFF;
          break;
       }
}
 refreshLEDs();
 }
}
