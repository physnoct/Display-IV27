/*
 * Princeton  PT6311
 * Can drive 16 digits (20 segments)
 /SS: pin 7
 MOSI: pin 11
 MISO: pin 12
 SCK: pin 13
*/

/* IV-27
Segment 1-8: a,b,c,d,e,f,g,dp
address 0 is the leftmost character (-/o)
*/

#include <SPI.h>

const int chipSelectPin = 10;

#define NB_DIGITS 14

/* PT6311's display memory is 48 bytes, 00-2F
 *  3 bytes for each position
segments 1-8,segments 9-16, segments 17-20
*/
byte display[] = 
    {
      0xff,0,0,   // -/o
      0x3F,0,0,   // leftmost digit
      0x06,0,0,
      0x5B,0,0,
      0x4F,0,0,
      0x66,0,0,
      0x6D,0,0,
      0x7D,0,0,
      0x07,0,0,
      0x7F,0,0,
      0x6F,0,0,
      0x77,0,0,
      0x7C,0,0,
      0x39,0,0
    };

const byte tab7seg[] = 
    {
      0x3F,0x06,0x5B,0x4F,
      0x66,0x6D,0x7D,0x07,
      0x7F,0x6F,0x77,0x7C,
      0x39,0x5E,0x79,0x71
    };

void setup()
{
  CLKPR = 0x80; //8MHz
  CLKPR = 0x00;
  
  DDRB = 0xEF;  //MISO
  PORTB = 0xFF;
  
  DDRD = 0xFF;
  
  // initialize the chip select pin:
  pinMode(chipSelectPin, OUTPUT);

  // start the SPI library:
  SPI.begin();

  SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE3));

  //Set command 1
  writeCommand(0b00001101);   //display mode setting: 14 digits, 14 segments

  //Set command 3: cleardisplay RAM
  writeCommand(0b11000000);   //address 0

  //Set command 4: display on
  writeCommand(0b10001111);   //on, pulse width = 14/16

  writeLeds(0xFD);
  
  writeBuffer(display);
}

void loop()
{
  delay(200);
  clearDisplay();
  home();
  writeSpace();
  writeHexNibble(0x0e);
  delay(200);
  writeSpace();
  writeHexByte(0xC3);
  delay(200);
  writeSpace();
  writeHexWord(0xdead);
  writeHexWord(0xbeef);
}

//Sends a write command to PT6311
void writeCommand(byte command) {
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(command);  //Send value to record into register

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}

void home()
{
  writeCommand(0b11000000);   //address 0
}

void clearDisplay()
{
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(0b11000000);  //Set address 0
  SPI.transfer(0b01000000);

  for (int i = 0; i < NB_DIGITS * 3; i++)
  {
      SPI.transfer(0);
  }

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}

void writeSpace()
{
  writeByte(0);  
}

void writeHexWord(int data)
{
  writeHexByte(data>>8);
  writeHexByte(data & 0xff);
}

void writeHexByte(byte data)
{
  writeHexNibble(data>>4);    //display 7-4
  writeHexNibble(data);       //display 3-0
}

void writeHexNibble(byte data)
{
  writeByte(tab7seg[data & 0x0f]);
}

void writeByte(byte data)
{
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(0b01000000);
  SPI.transfer(data);
  SPI.transfer(0);
  SPI.transfer(0);

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}

void writeBuffer(byte data[])
{
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(0b01000000);

  for (int i = 0; i < NB_DIGITS*3;)
  {
      SPI.transfer(data[i++]);
      SPI.transfer(data[i++]);
      SPI.transfer(data[i++]);
  }

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}

void writeLeds(byte data)
{
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(0b01000001);
  SPI.transfer(data);

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
}
