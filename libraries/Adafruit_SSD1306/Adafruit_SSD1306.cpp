/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#include <Wire.h>

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "glcdfont.c"

// a 5x7 font table
extern uint8_t PROGMEM font[];

// the memory buffer for the LCD

static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = {
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0x87, 0x07, 0x0f, 0x0f, 0x1f, 0x3f, 0x3f, 0x7f, 0x7f,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xc7, 0x87, 0x8f, 0x1e, 0x1e, 0x3c, 0x3c, 0x78, 0x70,
 0xf0, 0xe0, 0xe1, 0xc3, 0x83, 0x07, 0x07, 0x0f, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0x00, 0xf0, 0xf8, 0x8c, 0x0c, 0x0c, 0x0c, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0,
 0x38, 0x1c, 0x38, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00,
 0x00, 0x00, 0xf0, 0x38, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0xe0, 0x38, 0x1c, 0x38, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0x8c, 0x0c,
 0x0c, 0x0c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0xfc, 0x0c, 0x0c, 0x0c, 0x0c,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0x00,
 0x00, 0x01, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0x00, 0x18, 0x30, 0x21, 0x61, 0x61, 0x61, 0x23, 0x1e, 0x1e, 0x00, 0x00, 0x70, 0x3c, 0x07, 0x04,
 0x04, 0x04, 0x04, 0x05, 0x07, 0x1c, 0x60, 0x00, 0x00, 0x00, 0x7f, 0x01, 0x01, 0x01, 0x01, 0x01,
 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x00, 0x00,
 0x00, 0x01, 0x0f, 0x18, 0x30, 0x60, 0x60, 0x60, 0x60, 0x20, 0x18, 0x00, 0x00, 0x00, 0x70, 0x3c,
 0x07, 0x04, 0x04, 0x04, 0x04, 0x05, 0x07, 0x1c, 0x60, 0x00, 0x00, 0x00, 0x18, 0x30, 0x21, 0x61,
 0x61, 0x61, 0x23, 0x1e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xf0, 0xf0, 0xf0, 0xf8, 0xff, 0xff, 0xff, 0xf0,
 0xe0, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0xe0, 0xe0, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (SSD1306_LCDHEIGHT == 64)
 0x00, 0x00, 0x00, 0xc0, 0xe0, 0xf0, 0xf8, 0x0c, 0x0c, 0x0e, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff,
 0x9f, 0x3f, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfc, 0x38, 0xf8, 0xf0, 0xe0, 0xc0, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0xf8, 0xf8, 0xf8,
 0xf8, 0xf0, 0xf0, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf0, 0xf0,
 0xf0, 0xf0, 0xf0, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf0, 0xf0,
 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf0, 0xe0, 0x80, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xe0, 0xf0, 0xfb, 0xff, 0xf7, 0xf3, 0xc1, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 0x1f, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x60, 0x03, 0xff, 0xff, 0xff, 0xfc, 0xe0,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x0f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xc0, 0x00, 0x03, 0x07, 0x0f, 0x07, 0x03, 0x00,
 0x00, 0x1e, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x1f, 0x3f, 0x7f, 0x7f, 0x07,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x3f, 0x7f, 0x7e, 0xfe, 0xfc, 0xf8, 0xf8, 0xf8,
 0xf0, 0xf0, 0xf0, 0xf0, 0xf9, 0x79, 0x79, 0x38, 0x3c, 0x1c, 0x0c, 0x06, 0x03, 0x01, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
 0x0f, 0x1f, 0x1f, 0x1f, 0x3f, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x1f, 0x3f,
 0x3f, 0x1f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0x1f, 0x1f, 0x1f,
 0x3f, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0x1f, 0x1f, 0x1f,
 0x3f, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x03, 0x0f, 0x1f, 0x1f, 0x1f, 0x3f, 0x1f, 0x1f, 0x0f, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
};



// the most basic function, set a single pixel
void Adafruit_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }  

  // x is which column
  if (color == WHITE) 
    buffer[x+ (y/8)*SSD1306_LCDWIDTH] |= _BV((y%8));  
  else
    buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~_BV((y%8)); 
}

Adafruit_SSD1306::Adafruit_SSD1306(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS) {
  cs = CS;
  rst = RST;
  dc = DC;
  sclk = SCLK;
  sid = SID;
}

// initializer for I2C - we only indicate the reset pin!
Adafruit_SSD1306::Adafruit_SSD1306(int8_t reset) {
  sclk = dc = cs = sid = -1;
  rst = reset;
}
  

void Adafruit_SSD1306::begin(uint8_t vccstate, uint8_t i2caddr) {
#ifdef SSD1306_128_64
  constructor(128, 64);
#endif
#ifdef SSD1306_128_32
  constructor(128, 32);
#endif

  _i2caddr = i2caddr;


  // set pin directions
  if (sid != -1)
  {
    // SPI
    pinMode(sid, OUTPUT);
    pinMode(sclk, OUTPUT);
    pinMode(dc, OUTPUT);
    pinMode(cs, OUTPUT);
    clkport     = portOutputRegister(digitalPinToPort(sclk));
    clkpinmask  = digitalPinToBitMask(sclk);
    mosiport    = portOutputRegister(digitalPinToPort(sid));
    mosipinmask = digitalPinToBitMask(sid);
    csport      = portOutputRegister(digitalPinToPort(cs));
    cspinmask   = digitalPinToBitMask(cs);
    dcport      = portOutputRegister(digitalPinToPort(dc));
    dcpinmask   = digitalPinToBitMask(dc);
  }
  else
  {
    // I2C Init
	Wire.begin(); // Is this the right place for this?
  }

  // Setup reset pin direction (used by both SPI and I2C)  
  pinMode(rst, OUTPUT);
  digitalWrite(rst, HIGH);
  // VDD (3.3V) goes high at start, lets just chill for a ms
  delay(1);
  // bring reset low
  digitalWrite(rst, LOW);
  // wait 10ms
  delay(10);
  // bring out of reset
  digitalWrite(rst, HIGH);
  // turn on VCC (9V?)

   #if defined SSD1306_128_32
    // Init sequence for 128x32 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x1F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x10); }
    else 
      { ssd1306_command(0x14); }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
	ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x22); }
    else 
      { ssd1306_command(0xF1); }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  #endif

  #if defined SSD1306_128_64
    // Init sequence for 128x64 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x10); }
    else 
      { ssd1306_command(0x14); }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x9F); }
    else 
      { ssd1306_command(0xCF); }
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x22); }
    else 
      { ssd1306_command(0xF1); }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  #endif
  
  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}


void Adafruit_SSD1306::invertDisplay(uint8_t i) {
  if (i) {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  } else {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

void Adafruit_SSD1306::ssd1306_command(uint8_t c) { 
  if (sid != -1)
  {
    // SPI
    //digitalWrite(cs, HIGH);
    *csport |= cspinmask;
    //digitalWrite(dc, LOW);
    *dcport &= ~dcpinmask;
    //digitalWrite(cs, LOW);
    *csport &= ~cspinmask;
    fastSPIwrite(c);
    //digitalWrite(cs, HIGH);
    *csport |= cspinmask;
  }
  else
  {
    // I2C
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
  }
}

void Adafruit_SSD1306::ssd1306_data(uint8_t c) {
  if (sid != -1)
  {
    // SPI
    //digitalWrite(cs, HIGH);
    *csport |= cspinmask;
    //digitalWrite(dc, HIGH);
    *dcport |= dcpinmask;
    //digitalWrite(cs, LOW);
    *csport &= ~cspinmask;
    fastSPIwrite(c);
    //digitalWrite(cs, HIGH);
    *csport |= cspinmask;
  }
  else
  {
    // I2C
    uint8_t control = 0x40;   // Co = 0, D/C = 1
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
  }
}

void Adafruit_SSD1306::display(void) {
  ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
  ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

  if (sid != -1)
  {
    // SPI
    *csport |= cspinmask;
    *dcport |= dcpinmask;
    *csport &= ~cspinmask;

    for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
      fastSPIwrite(buffer[i]);
      //ssd1306_data(buffer[i]);
    }
    // i wonder why we have to do this (check datasheet)
    if (SSD1306_LCDHEIGHT == 32) {
      for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
        //ssd1306_data(0);
        fastSPIwrite(0);
      }
    }
    *csport |= cspinmask;
  }
  else
  {
    // save I2C bitrate
    uint8_t twbrbackup = TWBR;
    TWBR = 12; // upgrade to 400KHz!

    //Serial.println(TWBR, DEC);
    //Serial.println(TWSR & 0x3, DEC);

    // I2C
    for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
      // send a bunch of data in one xmission
      Wire.beginTransmission(_i2caddr);
      Wire.write(0x40);
      for (uint8_t x=0; x<16; x++) {
	Wire.write(buffer[i]);
	i++;
      }
      i--;
      Wire.endTransmission();
    }
    // i wonder why we have to do this (check datasheet)
    if (SSD1306_LCDHEIGHT == 32) {
      for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
	// send a bunch of data in one xmission
	Wire.beginTransmission(_i2caddr);
	Wire.write(0x40);
	for (uint8_t x=0; x<16; x++) {
	  Wire.write((uint8_t)0x00);
	  i++;
	}
	i--;
	Wire.endTransmission();
      }
    }
    TWBR = twbrbackup;
  }
}

// clear everything
void Adafruit_SSD1306::clearDisplay(void) {
  memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}


inline void Adafruit_SSD1306::fastSPIwrite(uint8_t d) {
  
  for(uint8_t bit = 0x80; bit; bit >>= 1) {
    *clkport &= ~clkpinmask;
    if(d & bit) *mosiport |=  mosipinmask;
    else        *mosiport &= ~mosipinmask;
    *clkport |=  clkpinmask;
  }
  //*csport |= cspinmask;
}

inline void Adafruit_SSD1306::slowSPIwrite(uint8_t d) {
 for (int8_t i=7; i>=0; i--) {
   digitalWrite(sclk, LOW);
   if (d & _BV(i)) {
     digitalWrite(sid, HIGH);
   } else {
     digitalWrite(sid, LOW);
   }
   digitalWrite(sclk, HIGH);
 }
}
