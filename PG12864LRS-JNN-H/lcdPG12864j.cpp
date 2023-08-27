// Bios for Atmel AVR ATmega128 
// 128x64 LCD (port PB 0-7),(port PC 2,3,4,5,6,7)  
// 4x4 keyboard (port PC 0-7)
// sound (port PD 5) 
// 100hz timer tick (TIMER0)
// by: Nikolaos Koskinas (koskinas@autofax.com.br) - Autofax Comercial Ltda.

// Display module pinout

// 1-  VSS       (GND)
// 2-  VDD       (+5VCC)
// 3-  VO        (contrast +5 to -5)
// 4-  data 0    (PA 0) (PD 0)
// 5-  data 1    (PA 1) (PD 1)
// 6-  data 2    (PA 2) (PD 2)
// 7-  data 3    (PA 3) (PD 3)
// 8-  data 4    (PA 4) (PD 4)
// 9-  data 5    (PA 5) (PD 5)
// 10- data 6    (PA 6) (PD 6)
// 11- data 7    (PA 7) (PD 7)
// 12- /CS1      (PC 2) A0
// 13- /CS2      (PC 3) A1
// 14- /RST      (PC 4) 12
// 15- R /W      (PC 5) A2
// 16- D /I      (PC 6) A3 
// 17- E         (PC 7) 11
// 18- VSS        ----
// 19- A          ----
// 20- K          ----

#include <Arduino.h>
//#include <macros.h>
#include <stdlib.h>
#include "arial14.h"
//#include "font850n.h"
//#include "bios.h"

//Control
#define LCD_E     37            //(1<<7)
#define LCD_DI    36            //(1<<6)
#define LCD_RW    35            //(1<<5)
#define LCD_RST   38            //(1<<4)
#define LCD_CS2   34            //(1<<3)
#define LCD_CS1   33            //(1<<2)

//Data
#define CONTROL_PORT  DDRA
#define READ_PORT     PINA
#define WRITE_PORT    PORTA



#define lcd_set_e     digitalWrite(LCD_E, HIGH)     //(PORTC|=LCD_E)
#define lcd_set_di    digitalWrite(LCD_DI, HIGH)     //(PORTC|=LCD_DI)
#define lcd_set_rw    digitalWrite(LCD_RW, HIGH)     //(PORTC|=LCD_RW)
#define lcd_set_rst   digitalWrite(LCD_RST, HIGH)     //(PORTC|=LCD_RST)
//#define lcd_set_cs2   digitalWrite(LCD_CS2, LOW)     //(PORTC|=LCD_CS2)
//#define lcd_set_cs1   digitalWrite(LCD_CS1, LOW)     //(PORTC|=LCD_CS1)
#define lcd_set_cs2   digitalWrite(LCD_CS2, HIGH)     //(PORTC|=LCD_CS2)
#define lcd_set_cs1   digitalWrite(LCD_CS1, HIGH)     //(PORTC|=LCD_CS1)

#define lcd_clear_e      digitalWrite(LCD_E, LOW)     //(PORTC&=~LCD_E)
#define lcd_clear_di     digitalWrite(LCD_DI, LOW)    //(PORTC&=~LCD_DI)
#define lcd_clear_rw     digitalWrite(LCD_RW, LOW)    //(PORTC&=~LCD_RW)
#define lcd_clear_rst    digitalWrite(LCD_RST, LOW)   //(PORTC&=~LCD_RST)
#define lcd_clear_cs2    digitalWrite(LCD_CS2, LOW)   //(PORTC&=~LCD_CS2)
#define lcd_clear_cs1    digitalWrite(LCD_CS1, LOW)   //(PORTC&=~LCD_CS1)
//#define lcd_clear_cs2    digitalWrite(LCD_CS2, HIGH)   //(PORTC&=~LCD_CS2)
//#define lcd_clear_cs1    digitalWrite(LCD_CS1, HIGH)   //(PORTC&=~LCD_CS1)
#define lcd_clear_cs2    digitalWrite(LCD_CS2, LOW)   //(PORTC&=~LCD_CS2)
#define lcd_clear_cs1    digitalWrite(LCD_CS1, LOW)   //(PORTC&=~LCD_CS1)

static char currow;
static char curcol;
static char starty;
static unsigned int tick;
static unsigned char curoff;
static unsigned char reverse;
static unsigned char underline;
static unsigned char lwmode=1;


void usdelay(unsigned int t)
{
  unsigned int t2=t;
  while(t--); // time = (t * 0,423)usec at 14.318 Mhz
  while(t2--); // time = (t * 0,423)usec at 14.318 Mhz
}
 
void delay(unsigned int ms)
{
   while(ms--) usdelay(2360);
}
 
unsigned char lcd_busy(void)
{
  unsigned char status;
  CONTROL_PORT=0x00;
  lcd_clear_di;
  lcd_set_rw;
  lcd_set_e;
  usdelay(1);
  status=READ_PORT;
  lcd_clear_e;
  lcd_clear_rw;
  CONTROL_PORT=0xff;
  return status&0x80;
}

void lcd_send_cmd(unsigned char cmd)
{
  lcd_clear_di;
  WRITE_PORT = cmd;
  lcd_set_e;
  usdelay(1);
  lcd_clear_e;
  usdelay(8);
}

void lcd_send_data(unsigned char data)
{
  lcd_set_di;
  usdelay(1);
  WRITE_PORT = data;
  lcd_set_e;
  usdelay(1);
  lcd_clear_e;
  usdelay(1); 
}

unsigned char lcd_read_data(void)
{
  unsigned char data;
  CONTROL_PORT=0x00;
  lcd_set_di;
  lcd_set_rw;
  lcd_set_e;
  usdelay(1);
  data=READ_PORT;
  lcd_clear_e;
  usdelay(8);
  lcd_set_e;
  usdelay(1);
  data=READ_PORT;
  lcd_clear_e;
  lcd_clear_rw;
  CONTROL_PORT=0xff;
  usdelay(8);
  return data;   
}

void lcd_send_reset(void)
{
  lcd_clear_rst;
  usdelay(50000);
  lcd_set_rst;
  usdelay(150000);
  lcd_clear_cs1;
  lcd_clear_cs2;
  lcd_send_cmd(0x3f);        // display on
  lcd_set_cs1;
  lcd_set_cs2;

  lcd_clear_cs1;
  lcd_clear_cs2;
  lcd_send_cmd(0xC0);        // display on
  lcd_set_cs1;
  lcd_set_cs2;

}

void put_pixel(unsigned char x, unsigned char y, unsigned char pixel)
{
  unsigned char t;
  if(x&0x40) lcd_clear_cs2; else lcd_clear_cs1;
  x&=0x3f;
  y=(y+starty)&0x3f;
  lcd_send_cmd(0x40|x);           // set x addr
  lcd_send_cmd(0xb8|(y>>3));      // set page addr
  // read data
  CONTROL_PORT=0x00;
  lcd_set_di;
  lcd_set_rw;
  lcd_set_e;
  usdelay(1);
  t=READ_PORT;
  lcd_clear_e;
  usdelay(8);
  lcd_set_e;
  usdelay(1);
  t=READ_PORT;
  lcd_clear_e;
  lcd_clear_rw;
  CONTROL_PORT=0xff;
  usdelay(8);
  lcd_send_cmd(0x40|x);    // set x addr
  switch(pixel)
  {
	case 0: t&=~(0x01<<(y%8)); break;
    case 1: t|=(0x01<<(y%8)); break;
	case 2: t^=(0x01<<(y%8));
  }
  lcd_send_data(t);
  lcd_set_cs1;
  lcd_set_cs2;
}

void draw_line(char x1, char y1, char x2, char y2)
{
  char x, y;
  int DeltaX, DeltaY;
  int XStep, YStep, direction;
  x=x1;
  y=y1;
  XStep=1;
  YStep=1;
  if(x1>x2) XStep=-1;
  if(y1>y2) YStep=-1;
  DeltaX=abs(x2-x1);
  DeltaY=abs(y2-y1);
  if(DeltaX==0) direction=-1; else direction=0;
  while(!((x==x2)&&(y==y2)))
  {     
    put_pixel(x,y,1);
    if(direction<0)
	{
      y+=YStep;
      direction+=DeltaX;
    }
    else
    {
      x+=XStep;
      direction-=DeltaY;
    }
  }    
  put_pixel(x,y,1);
}    

void draw_sqare(char x1, char y1, char x2, char y2, char fill)
{
  char i;
  if(!fill)
  {
    draw_line(x1, y1, x2, y1);
    draw_line(x2, y1, x2, y2);
    draw_line(x1, y2, x2, y2);
    draw_line(x1, y2, x1, y1);
  }
  else for(i=y1;i<y2;i++) draw_line(x1, i, x2, i);
}

void charat(char col, char row, unsigned char car)
{
  unsigned char c;
  unsigned char l;
  unsigned char frow;
  unsigned int faddr;
  col*=6;
  row*=10;
  faddr=car*10;
  for(l=0;l<10;l++)
  {
    frow=Arial_14[faddr+l]^reverse;
    for(c=0;c<6;c++) 
    {
      put_pixel(col+c,row+l,frow&0x01);
	  frow>>=1;
    }
  } 
}

/*****************************************************************************/

void xorcursor(void)
{
  unsigned char j;
  unsigned char x=curcol*6;
  unsigned char y=(currow*10)+9;
  for(j=0;j<6;j++) put_pixel(x+j,y,2);
}

void scrolldown(void)
{
  unsigned char j;
  unsigned char h; 
  for(j=0;j<10;j++)
  {
    lcd_clear_cs1;
    lcd_clear_cs2;
    starty=(starty+1)&0x3f;
	lcd_send_cmd(0xc0+starty);        // set z line 
    lcd_set_cs1;
    lcd_set_cs2;
    for(h=0;h<128;h++) put_pixel(h,63,0);
  }
}

void clrscr()
{
  unsigned char j;
  unsigned char h; 
  lcd_clear_cs1;
  lcd_clear_cs2;
//  usdelay(10);
  lcd_send_cmd(0xb8);        // set page addr to zero
  usdelay(8);
  lcd_send_cmd(0x40);        // set x addr to zero
  usdelay(8);
  lcd_send_cmd(0xc0);        // set z line to zero
  for(h=0;h<8;h++)
  {
  usdelay(8);
    lcd_send_cmd(0xb8 + h);  // change page (0..7)
    for(j=0;j<64;j++) lcd_send_data(0x00);
  }
  lcd_set_cs1;
  lcd_set_cs2;
//  usdelay(10);
  currow=curcol=starty=0;
  if(!curoff) xorcursor();
  reverse=underline=0;
}

void clearregion(char inicol,char inirow,char endcol,char endrow)
{
}

void clear_eol()
{
  unsigned char j;
  for(j=curcol;j<21;j++) charat(j,currow,0);
}

void creturn()
{
  xorcursor();
  curcol=0;
  xorcursor();
 }

void linefeed()
{
  xorcursor();
  currow++;
  if(currow>=6) {
    currow--;
    scrolldown();
  }
  xorcursor();
}

void newline()
{
  xorcursor();
  curcol=0;
  currow++;
  if(currow>=6) {
    currow--;
    scrolldown();
  }
  xorcursor();
}

void cursor_down(char n)
{
  xorcursor();
  currow+=n;
  if(currow>5) currow=5;
  xorcursor();
}

void cursor_up(char n)
{
  xorcursor();
  currow-=n;
  if(currow<0) currow=0;
  xorcursor();
}

void cursor_left(char n)
{
  xorcursor();
  curcol-=n;
  if(curcol<0) curcol=0;
  xorcursor();
}

void cursor_right(char n)
{
  xorcursor();
  curcol+=n;
  if(curcol>20) curcol=20;
  xorcursor();
}

void cursor_move(int row, int col)
{
  xorcursor();
  curcol=col;
  if(curcol>20) curcol=20;
  currow=row;
  if(currow>5) currow=5;
  xorcursor();
}

int putChar(int c)
{
  if(c<32)
  {
    if(c==0x0a) linefeed();
	if(c==0x0d) creturn();
  }
  else
  {
    c-=32;
    xorcursor();
    charat(curcol,currow,c);
    if(underline) xorcursor();
    curcol++;
    if (curcol>=21)
    {
      if(lwmode){ currow++; curcol=0; } 
  	else curcol=20;
    }
    if (currow>=6) {
      scrolldown();
      currow=5;
    }
    xorcursor();
  }
}

void screenat(char x,char y, char *s)
{
  while(*s) {
    charat(x,y,*s-32);
    s++;
    x++;
  }
}

void cputs(char *s)
{
  while(*s) {
    putchar(*s);
    s++;
  }
}

void puts(char *s)
{
  while(*s) {
    putchar(*s);
    s++;
  }
  newline();
}

void bkspace()
{
  unsigned char savereverse;
  unsigned char saveunderline;
  if(curcol>0) {
    xorcursor();
    curcol--;
    xorcursor();
    savereverse=reverse;
    saveunderline=underline;
    reverse=0;
    underline=0;
    putchar(32);
    reverse=savereverse;
    underline=saveunderline;
    xorcursor();
    curcol--;
    xorcursor();
  } 
}

void setunderline(int u)
{
  if(underline&&!u&&curcol==20&&curoff) xorcursor();
  underline=u;
}

int getunderline()
{
  return underline;
}

void setreverse(int r)
{
  if(r) reverse=0x3f; else reverse=0;
}

int getreverse()
{
  if(reverse==0x3f) return 1; else return 0;
}

void setcursor(int c)
{
  if(c!=curoff) return;
  if(c)
  {
    curoff=0;
    xorcursor();
  }
  else
  {
    xorcursor();
    curoff=1;
  }
}

int getcursor()
{
  return(!curoff);
}

#define Command_On				    0x3F
#define Command_Off				    0x3E
#define Command_Set_Address		0x40
#define Command_Set_Page	   	0xB8
#define Command_Display_Start	0xC0

void init_crt(void)
{
  CONTROL_PORT = 0xFF; // PORT D - data out to LCD
  pinMode(LCD_E  , OUTPUT);
  pinMode(LCD_DI , OUTPUT);
  pinMode(LCD_RW , OUTPUT);
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS2, OUTPUT);
  pinMode(LCD_CS1, OUTPUT);


  lcd_set_cs1;
  lcd_set_cs2;
  lcd_clear_e;
  lcd_clear_rw;
  lcd_clear_di;
  lcd_send_reset();



  usdelay(10000);

  //put_pixel(x,y,1);
  put_pixel( 50, 1,1);
  put_pixel( 50, 2,1);
  put_pixel( 50, 3,1);
  put_pixel( 50, 4,1);
  put_pixel( 50, 5,1);
  
  puts("Paulo");
}


  
