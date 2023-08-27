/*

  HelloWorld.pde
  
  "Hello World!" example code.
  
  >>> Before compiling: Please remove comment from the constructor of the 
  >>> connected graphics display (see below).
  
  Universal 8bit Graphics Library, https://github.com/olikraus/u8glib/
  
  Copyright (c) 2012, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
*/


#include "U8glib.h"

// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported 
// devices with all constructor calls is here: https://github.com/olikraus/u8glib/wiki/device


// +-------------------------------------------------------------------
//                                                         en=18, cs1=14, cs2=15, di=17, rw=16 
// Bellow mini or pro mini data config
// U8GLIB_KS0108_128 u8g( 0,  1,   2,   3,  4,  5,  6,  7, 11,    14,     15,     17,    16); 		
// #define RESET_PIN  12
// Bellow mega 2560 data config
 U8GLIB_KS0108_128 u8g(22, 23,  24,  25, 26, 27, 28, 29, 37,    33,     34,     36,    35); 		
 #define RESET_PIN  38
// | Mini ou pro mini| mega 2560
// | Data = PORTD    | Data = PORTA
// | CS1 =A0  14     | 33    
// | CS2 =A1  15     | 34    
// | RST =    12     | 38
// | R/W =A2  16     | 35   
// | D/I =A3  17     | 36   
// | en  =11         | 37
// +-------------------------------------------------------------------




void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( 0, 12, "Hello World!");
  u8g.drawStr( 0, 25, "Hello World!");
  u8g.drawStr( 0, 40, "Hello World!");
  u8g.drawStr( 0, 55, "Hello World!");
}

void setup(void) {
  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
  pinMode(10, OUTPUT);
  digitalWrite(10,HIGH);

  pinMode(RESET_PIN, OUTPUT);

  digitalWrite(RESET_PIN,LOW);
  delay(10);
  digitalWrite(RESET_PIN,HIGH);

}

void loop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  //delay(50);
}

