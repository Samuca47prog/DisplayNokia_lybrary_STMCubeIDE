/*Adaptado por Sauer para uso dos alunos da Fundacao Liberato*/
#include "stm32f4xx.h"
#include "LCD.h"
#include "font.h"
#include "grafic.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_gpio.h"

/*
// =============================================================================================================================
// ---- Configuração dos Pinos IO ----
#define LCD_CS 	12 		//CE
#define LCD_RST 10 		//RST
#define LCD_MO	15		//DIN
#define LCD_SCK	13      //CLK
#define LCD_DC	14 		//DO
#define PORT	GPIOB	//GPIO onde esta o display
*/
#define LCD_RST 10 		//RST
#define PORT	GPIOB	//GPIO onde esta o display

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define false 0
#define true 1

 int scrbuf[504];

 // manipulador do display
 LCD_HandleTypeDef *lcd;

//Define the LCD Operation function
void LCD5110_LCD_write_byte(unsigned char dat,unsigned char mode);
void LCD5110_LCD_delay_ms(unsigned int t);

//Define the hardware operation function
//void LCD5110_CS(unsigned char temp);
void LCD5110_RST(unsigned char temp);
//void LCD5110_DC(unsigned char temp);

// Variáveis para generalização dos pinos
//GPIO_TypeDef	*Port_DC;
//uint16_t		 Pin_DC;
//GPIO_TypeDef	*Port_CS;
//uint16_t		 Pin_CS;

// =============================================================================================================================
// ---- Configuração dos Pinos IO ----
void LCD5110_init(LCD_HandleTypeDef *hlcd5110)
{
	// phspi irá apontar pro endereço de dados da SPI escolhida
	lcd = hlcd5110;

	// agora o HAL vai controlar os pinos de CS e de DS
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, 1);
	// Antes:
	//LCD5110_DC(1);//LCD_DC = 1;
	//LCD5110_CS(1);//SPI_CS = 1;


	LCD5110_RST(0);//LCD_RST = 0;
	LCD5110_LCD_delay_ms(10);
	LCD5110_RST(1);//LCD_RST = 1;


	LCD5110_LCD_write_byte(0x21,0);
	LCD5110_LCD_write_byte(0xc6,0); //ajusta o contraste do display
	LCD5110_LCD_write_byte(0x06,0);
	LCD5110_LCD_write_byte(0x13,0);
	LCD5110_LCD_write_byte(0x20,0);
	LCD5110_clear();
	LCD5110_LCD_write_byte(0x0c,0);
}

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para escrever um byte no display ----
void LCD5110_LCD_write_byte(unsigned char dat,unsigned char mode)
{
	// define se é dado ou comando
	HAL_GPIO_WritePin(lcd->DC_Port, lcd->DC_Pin, mode);

	// habilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 0);

	// Transmissão dos dados feitos pela SPI do HAL
	HAL_SPI_Transmit(lcd->hspi, &dat, 1, 200);

	// desabilita o chip select
	HAL_GPIO_WritePin(lcd->CS_Port, lcd->CS_Pin, 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para escrever um caractere no display ----
void LCD5110_write_char(unsigned char c)
{
	unsigned char line;
	unsigned char ch = 0;

	// a função chama a write_byte para escrever o caractere no display

	c = c - 32;

	for(line=0;line<6;line++)
	{
		ch = font6_8[c][line];
		LCD5110_LCD_write_byte(ch,1);
		
	}
}
void LCD5110_write_char_reg(unsigned char c)
{
	unsigned char line;
	unsigned char ch = 0;

	c = c - 32;

	for(line=0;line<6;line++)
	{
		ch = ~font6_8[c][line];
		LCD5110_LCD_write_byte(ch,1);
		
	}
}

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para escrever uma string no display ----
void LCD5110_write_string(char *s)
{
	unsigned char ch;
  	while(*s!='\0')
	{
		ch = *s;
		LCD5110_write_char(ch);
		s++;
	}
}

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para limpar display ----
void LCD5110_clear()
{
	// limpa o display escrevendo 0 em todos os pixels
	unsigned char i,j;
	for(i=0;i<6;i++)
		for(j=0;j<84;j++)
			LCD5110_LCD_write_byte(0,1);	
}

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para setar a posição XY no display ----
void LCD5110_set_XY(unsigned char X,unsigned char Y)
{
	unsigned char x;
	x = 6*X;

	LCD5110_LCD_write_byte(0x40|Y,0);
	LCD5110_LCD_write_byte(0x80|x,0);
}

// -----------------------------------------------------------------------------------------------------------------------------
// ---- Função para escrever um número no display ----
void LCD5110_Write_Dec(unsigned int b)
{

	unsigned char datas[4];

	datas[0] = b/1000;
	b = b - datas[0]*1000;
	datas[1] = b/100;
	b = b - datas[1]*100;
	datas[2] = b/10;
	b = b - datas[2]*10;
	datas[3] = b;

	datas[0]+=48;
	datas[1]+=48;
	datas[2]+=48;
	datas[3]+=48;

	LCD5110_write_char(datas[0]);
	LCD5110_write_char(datas[1]);
	LCD5110_write_char(datas[2]);
	LCD5110_write_char(datas[3]);

	//a++;
}

void LCD5110_LCD_delay_ms(unsigned int nCount)
{
  unsigned long t;
	t = nCount * 40000;
	while(t--);
}


// =============================================================================================================================
// ---- Funções para configurar os pinos ----

void LCD5110_RST(unsigned char temp)
{
	if (temp) PORT->ODR|=1<<LCD_RST;
	else PORT->ODR&=~(1<<LCD_RST);

}

/*
void LCD5110_CS(unsigned char temp)
{
	if (temp) PORT->ODR|=1<<LCD_CS;
	else PORT->ODR&=~(1<<LCD_CS);


}


void LCD5110_DC(unsigned char temp)
{
	if (temp) PORT->ODR|=1<<LCD_DC;
	else PORT->ODR&=~(1<<LCD_DC);

}

void LCD5110_MO(unsigned char temp)
{
	if (temp) PORT->ODR|=1<<LCD_MO;
	else PORT->ODR&=~(1<<LCD_MO);

}

void LCD5110_SCK(unsigned char temp)
{
	if (temp) PORT->ODR|=1<<LCD_SCK;
	else PORT->ODR&=~(1<<LCD_SCK);
}
*/

/* Para usar a função printf */
int _write(int file, char *ptr, int len)
{
  int i=0;
  for(i=0 ; i<len ; i++)
	  LCD5110_write_char(*ptr++);
  return len;
}


/*
  LCD5110_Graph.cpp - Arduino/chipKit library support for Nokia 5110 compatible LCDs
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved

  Basic functionality of this library are based on the demo-code provided by
  ITead studio. You can find the latest version of the library at
  http://www.RinkyDinkElectronics.com/

  This library has been made to make it easy to use the Nokia 5110 LCD module
  as a graphics display on an Arduino or a chipKit.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the
  examples and tools supplied with the library.
*/

/*
void LCD5110_enableSleep()
{
   _sleep = true;
   _LCD_Write(PCD8544_SETYADDR, LCD_COMMAND);
   _LCD_Write(PCD8544_SETXADDR, LCD_COMMAND);
   for (int b=0; b<504; b++)
      _LCD_Write(0, LCD_DATA);
   _LCD_Write(PCD8544_FUNCTIONSET | PCD8544_POWERDOWN, LCD_COMMAND);
}

void LCD5110_disableSleep()
{
   _sleep = false;
   _LCD_Write(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION, LCD_COMMAND);
   _LCD_Write(PCD8544_SETVOP | _contrast, LCD_COMMAND);
   _LCD_Write(PCD8544_SETTEMP | LCD_TEMP, LCD_COMMAND);
   _LCD_Write(PCD8544_SETBIAS | LCD_BIAS, LCD_COMMAND);
   _LCD_Write(PCD8544_FUNCTIONSET, LCD_COMMAND);
   _LCD_Write(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL, LCD_COMMAND);
   update();
}
*/

// =============================================================================================================================
// ---- Função para escrever no buffer ----
void LCD5110_update()
{
//   if (_sleep==false)
//   {
//      _LCD_Write(PCD8544_SETYADDR, LCD_COMMAND);
//      _LCD_Write(PCD8544_SETXADDR, LCD_COMMAND);
	LCD5110_set_XY(0, 0);
      for (int b=0; b<504; b++){
    	  LCD5110_LCD_write_byte(scrbuf[b],1);
      }
         //_LCD_Write(scrbuf[b], LCD_DATA);
//   }
}

void LCD5110_clrScr()
{
   for (int c=0; c<504; c++)
      scrbuf[c]=0;
}

void LCD5110_fillScr()
{
   for (int c=0; c<504; c++)
      scrbuf[c]=255;
}

//void LCD5110_invert(int mode)
//{
//   if (mode==true)
//      _LCD_Write(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYINVERTED, LCD_COMMAND);
//   else
//      _LCD_Write(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL, LCD_COMMAND);
//}

void setPixel(uint16_t x, uint16_t y)
{
   int by, bi;

   if ((x>=0) && (x<84) && (y>=0) && (y<48))
   {
      by=((y/8)*84)+x;
      bi=y % 8;

      scrbuf[by]=scrbuf[by] | (1<<bi);
   }
}

void LCD5110_clrPixel(uint16_t x, uint16_t y)
{
   int by, bi;

   if ((x>=0) && (x<84) && (y>=0) && (y<48))
   {
      by=((y/8)*84)+x;
      bi=y % 8;

      scrbuf[by]=scrbuf[by] & ~(1<<bi);
   }
}

void LCD5110_invPixel(uint16_t x, uint16_t y)
{
   int by, bi;

   if ((x>=0) && (x<84) && (y>=0) && (y<48))
   {
      by=((y/8)*84)+x;
      bi=y % 8;

      if ((scrbuf[by] & (1<<bi))==0)
         scrbuf[by]=scrbuf[by] | (1<<bi);
      else
         scrbuf[by]=scrbuf[by] & ~(1<<bi);
   }
}
/*
void LCD5110_invertText(int mode)
{
   if (mode==true)
      cfont.inverted=1;
   else
      cfont.inverted=0;
}

void LCD5110_print(char *st, int x, int y)
{
   unsigned char ch;
   int stl;

   stl = strlen(st);
   if (x == RIGHT)
      x = 84-(stl*cfont.x_size);
   if (x == CENTER)
      x = (84-(stl*cfont.x_size))/2;

   for (int cnt=0; cnt<stl; cnt++)
         _print_char(*st++, x + (cnt*(cfont.x_size)), y);
}
*/
/*
void LCD5110_print(String st, int x, int y)
{
   char buf[st.length()+1];

   st.toCharArray(buf, st.length()+1);
   print(buf, x, y);
}
*/
void LCD5110_printNumI(long num, int x, int y, int length, char filler)
{
   char buf[25];
   char st[27];
   char neg=false;
   int c=0, f=0;

   if (num==0)
   {
      if (length!=0)
      {
         for (c=0; c<(length-1); c++)
            st[c]=filler;
         st[c]=48;
         st[c+1]=0;
      }
      else
      {
         st[0]=48;
         st[1]=0;
      }
   }
   else
   {
      if (num<0)
      {
         neg=true;
         num=-num;
      }

      while (num>0)
      {
         buf[c]=48+(num % 10);
         c++;
         num=(num-(num % 10))/10;
      }
      buf[c]=0;

      if (neg)
      {
         st[0]=45;
      }

      if (length>(c+neg))
      {
         for (int i=0; i<(length-c-neg); i++)
         {
            st[i+neg]=filler;
            f++;
         }
      }

      for (int i=0; i<c; i++)
      {
         st[i+neg+f]=buf[c-i-1];
      }
      st[c+neg+f]=0;

   }

   print(st,x,y);
}

void LCD5110_printNumF(double num, char dec, int x, int y, char divider, int length, char filler)
{
   char st[27];
   char neg=false;

   if (num<0)
      neg = true;

   _convert_float(st, num, length, dec);

   if (divider != '.')
   {
      for (int i=0; i<sizeof(st); i++)
         if (st[i]=='.')
            st[i]=divider;
   }

   if (filler != ' ')
   {
      if (neg)
      {
         st[0]='-';
         for (int i=1; i<sizeof(st); i++)
            if ((st[i]==' ') || (st[i]=='-'))
               st[i]=filler;
      }
      else
      {
         for (int i=0; i<sizeof(st); i++)
            if (st[i]==' ')
               st[i]=filler;
      }
   }

   print(st,x,y);
}
/*
void LCD5110_print_char(unsigned char c, int x, int y)
{
   if ((cfont.y_size % 8) == 0)
   {
      int font_idx = ((c - cfont.offset)*(cfont.x_size*(cfont.y_size/8)))+4;
      for (int rowcnt=0; rowcnt<(cfont.y_size/8); rowcnt++)
      {
         for(int cnt=0; cnt<cfont.x_size; cnt++)
         {
            for (int b=0; b<8; b++)
               if ((fontbyte(font_idx+cnt+(rowcnt*cfont.x_size)) & (1<<b))!=0)
                  if (cfont.inverted==0)
                     setPixel(x+cnt, y+(rowcnt*8)+b);
                  else
                     clrPixel(x+cnt, y+(rowcnt*8)+b);
               else
                  if (cfont.inverted==0)
                     clrPixel(x+cnt, y+(rowcnt*8)+b);
                  else
                     setPixel(x+cnt, y+(rowcnt*8)+b);
         }
      }
   }
   else
   {
      int font_idx = ((c - cfont.offset)*((cfont.x_size*cfont.y_size/8)))+4;
      int cbyte=fontbyte(font_idx);
      int cbit=7;
      for (int cx=0; cx<cfont.x_size; cx++)
      {
         for (int cy=0; cy<cfont.y_size; cy++)
         {
            if ((cbyte & (1<<cbit)) != 0)
               if (cfont.inverted==0)
                  setPixel(x+cx, y+cy);
               else
                  clrPixel(x+cx, y+cy);
            else
               if (cfont.inverted==0)
                  clrPixel(x+cx, y+cy);
               else
                  setPixel(x+cx, y+cy);
            cbit--;
            if (cbit<0)
            {
               cbit=7;
               font_idx++;
               cbyte=fontbyte(font_idx);
            }
         }
      }
   }
}

void LCD5110_setFont(uint8_t* font)
{
   cfont.font=font;
   cfont.x_size=fontbyte(0);
   cfont.y_size=fontbyte(1);
   cfont.offset=fontbyte(2);
   cfont.numchars=fontbyte(3);
   cfont.inverted=0;
}
*/
void LCD5110_drawHLine(int x, int y, int l)
{
   int by, bi;

   if ((x>=0) && (x<84) && (y>=0) && (y<48))
   {
      for (int cx=0; cx<l; cx++)
      {
         by=((y/8)*84)+x;
         bi=y % 8;

         scrbuf[by+cx] |= (1<<bi);
      }
   }
   LCD5110_update();
}

void LCD5110_clrHLine(int x, int y, int l)
{
   int by, bi;

   if ((x>=0) && (x<84) && (y>=0) && (y<48))
   {
      for (int cx=0; cx<l; cx++)
      {
         by=((y/8)*84)+x;
         bi=y % 8;

         scrbuf[by+cx] &= ~(1<<bi);
      }
   }
   LCD5110_update();
}

void LCD5110_drawVLine(int x, int y, int l)
{
   int by, bi;

   if ((x>=0) && (x<84) && (y>=0) && (y<48))
   {
      for (int cy=0; cy<l; cy++)
      {
         setPixel(x, y+cy);
      }
   }
   LCD5110_update();
}

void LCD5110_clrVLine(int x, int y, int l)
{
   int by, bi;

   if ((x>=0) && (x<84) && (y>=0) && (y<48))
   {
      for (int cy=0; cy<l; cy++)
      {
         clrPixel(x, y+cy);
      }
   }
   LCD5110_update();
}

void LCD5110_drawLine(int x1, int y1, int x2, int y2)
{
   int tmp;
   double delta, tx, ty;
   double m, b, dx, dy;

   if (((x2-x1)<0))
   {
      tmp=x1;
      x1=x2;
      x2=tmp;
      tmp=y1;
      y1=y2;
      y2=tmp;
   }
    if (((y2-y1)<0))
   {
      tmp=x1;
      x1=x2;
      x2=tmp;
      tmp=y1;
      y1=y2;
      y2=tmp;
   }

   if (y1==y2)
   {
      if (x1>x2)
      {
         tmp=x1;
         x1=x2;
         x2=tmp;
      }
      LCD5110_drawHLine(x1, y1, x2-x1);
   }
   else if (x1==x2)
   {
      if (y1>y2)
      {
         tmp=y1;
         y1=y2;
         y2=tmp;
      }
      LCD5110_drawVLine(x1, y1, y2-y1);
   }
   else if (abs(x2-x1)>abs(y2-y1))
   {
      delta=((double)(y2-y1)/(double)(x2-x1));
      ty=(double)(y1);
      if (x1>x2)
      {
         for (int i=x1; i>=x2; i--)
         {
            setPixel(i, (int)(ty+0.5));
              ty=ty-delta;
         }
      }
      else
      {
         for (int i=x1; i<=x2; i++)
         {
            setPixel(i, (int)(ty+0.5));
              ty=ty+delta;
         }
      }
   }
   else
   {
      delta=((float)(x2-x1)/(float)(y2-y1));
      tx=(float)(x1);
        if (y1>y2)
        {
         for (int i=y2+1; i>y1; i--)
         {
             setPixel((int)(tx+0.5), i);
              tx=tx+delta;
         }
        }
        else
        {
         for (int i=y1; i<y2+1; i++)
         {
             setPixel((int)(tx+0.5), i);
              tx=tx+delta;
         }
        }
   }
   LCD5110_update();
}

void LCD5110_clrLine(int x1, int y1, int x2, int y2)
{
   int tmp;
   double delta, tx, ty;
   double m, b, dx, dy;

   if (((x2-x1)<0))
   {
      tmp=x1;
      x1=x2;
      x2=tmp;
      tmp=y1;
      y1=y2;
      y2=tmp;
   }
    if (((y2-y1)<0))
   {
      tmp=x1;
      x1=x2;
      x2=tmp;
      tmp=y1;
      y1=y2;
      y2=tmp;
   }

   if (y1==y2)
   {
      if (x1>x2)
      {
         tmp=x1;
         x1=x2;
         x2=tmp;
      }
      clrHLine(x1, y1, x2-x1);
   }
   else if (x1==x2)
   {
      if (y1>y2)
      {
         tmp=y1;
         y1=y2;
         y2=tmp;
      }
      clrVLine(x1, y1, y2-y1);
   }
   else if (abs(x2-x1)>abs(y2-y1))
   {
      delta=((double)(y2-y1)/(double)(x2-x1));
      ty=(double)(y1);
      if (x1>x2)
      {
         for (int i=x1; i>=x2; i--)
         {
            clrPixel(i, (int)(ty+0.5));
              ty=ty-delta;
         }
      }
      else
      {
         for (int i=x1; i<=x2; i++)
         {
            clrPixel(i, (int)(ty+0.5));
              ty=ty+delta;
         }
      }
   }
   else
   {
      delta=((float)(x2-x1)/(float)(y2-y1));
      tx=(float)(x1);
        if (y1>y2)
        {
         for (int i=y2+1; i>y1; i--)
         {
             clrPixel((int)(tx+0.5), i);
              tx=tx+delta;
         }
        }
        else
        {
         for (int i=y1; i<y2+1; i++)
         {
             clrPixel((int)(tx+0.5), i);
              tx=tx+delta;
         }
        }
   }
   LCD5110_update();
}

void LCD5110_drawRect(int x1, int y1, int x2, int y2)
{
   int tmp;

   if (x1>x2)
   {
      tmp=x1;
      x1=x2;
      x2=tmp;
   }
   if (y1>y2)
   {
      tmp=y1;
      y1=y2;
      y2=tmp;
   }

   drawHLine(x1, y1, x2-x1);
   drawHLine(x1, y2, x2-x1);
   drawVLine(x1, y1, y2-y1);
   drawVLine(x2, y1, y2-y1+1);
   LCD5110_update();
}

void LCD5110_clrRect(int x1, int y1, int x2, int y2)
{
   int tmp;

   if (x1>x2)
   {
      tmp=x1;
      x1=x2;
      x2=tmp;
   }
   if (y1>y2)
   {
      tmp=y1;
      y1=y2;
      y2=tmp;
   }

   clrHLine(x1, y1, x2-x1);
   clrHLine(x1, y2, x2-x1);
   clrVLine(x1, y1, y2-y1);
   clrVLine(x2, y1, y2-y1+1);
}

void LCD5110_drawRoundRect(int x1, int y1, int x2, int y2)
{
   int tmp;

   if (x1>x2)
   {
      tmp=x1;
      x1=x2;
      x2=tmp;
   }
   if (y1>y2)
   {
      tmp=y1;
      y1=y2;
      y2=tmp;
   }
   if ((x2-x1)>4 && (y2-y1)>4)
   {
      setPixel(x1+1,y1+1);
      setPixel(x2-1,y1+1);
      setPixel(x1+1,y2-1);
      setPixel(x2-1,y2-1);
      LCD5110_drawHLine(x1+2, y1, x2-x1-3);
      LCD5110_drawHLine(x1+2, y2, x2-x1-3);
      LCD5110_drawVLine(x1, y1+2, y2-y1-3);
      LCD5110_drawVLine(x2, y1+2, y2-y1-3);
   }
   LCD5110_update();
}

void LCD5110_clrRoundRect(int x1, int y1, int x2, int y2)
{
   int tmp;

   if (x1>x2)
   {
      tmp=x1;
      x1=x2;
      x2=tmp;
   }
   if (y1>y2)
   {
      tmp=y1;
      y1=y2;
      y2=tmp;
   }
   if ((x2-x1)>4 && (y2-y1)>4)
   {
	   LCD5110_clrPixel(x1+1,y1+1);
	   LCD5110_clrPixel(x2-1,y1+1);
	   LCD5110_clrPixel(x1+1,y2-1);
	   LCD5110_clrPixel(x2-1,y2-1);
	   LCD5110_clrHLine(x1+2, y1, x2-x1-3);
	   LCD5110_clrHLine(x1+2, y2, x2-x1-3);
	   LCD5110_clrVLine(x1, y1+2, y2-y1-3);
	   LCD5110_clrVLine(x2, y1+2, y2-y1-3);
   }
}

void LCD5110_drawCircle(int x, int y, int radius)
{
   int f = 1 - radius;
   int ddF_x = 1;
   int ddF_y = -2 * radius;
   int x1 = 0;
   int y1 = radius;
   char ch, cl;

   setPixel(x, y + radius);
   setPixel(x, y - radius);
   setPixel(x + radius, y);
   setPixel(x - radius, y);

   while(x1 < y1)
   {
      if(f >= 0)
      {
         y1--;
         ddF_y += 2;
         f += ddF_y;
      }
      x1++;
      ddF_x += 2;
      f += ddF_x;
      setPixel(x + x1, y + y1);
      setPixel(x - x1, y + y1);
      setPixel(x + x1, y - y1);
      setPixel(x - x1, y - y1);
      setPixel(x + y1, y + x1);
      setPixel(x - y1, y + x1);
      setPixel(x + y1, y - x1);
      setPixel(x - y1, y - x1);
   }
   LCD5110_update();
}

void LCD5110_clrCircle(int x, int y, int radius)
{
   int f = 1 - radius;
   int ddF_x = 1;
   int ddF_y = -2 * radius;
   int x1 = 0;
   int y1 = radius;
   char ch, cl;

   LCD5110_clrPixel(x, y + radius);
   LCD5110_clrPixel(x, y - radius);
   LCD5110_clrPixel(x + radius, y);
   LCD5110_clrPixel(x - radius, y);

   while(x1 < y1)
   {
      if(f >= 0)
      {
         y1--;
         ddF_y += 2;
         f += ddF_y;
      }
      x1++;
      ddF_x += 2;
      f += ddF_x;
      LCD5110_clrPixel(x + x1, y + y1);
      LCD5110_clrPixel(x - x1, y + y1);
      LCD5110_clrPixel(x + x1, y - y1);
      LCD5110_clrPixel(x - x1, y - y1);
      LCD5110_clrPixel(x + y1, y + x1);
      LCD5110_clrPixel(x - y1, y + x1);
      LCD5110_clrPixel(x + y1, y - x1);
      LCD5110_clrPixel(x - y1, y - x1);
   }
   LCD5110_update();
}

void LCD5110_drawBitmap(int x, int y, uint8_t* bitmap, int sx, int sy)
{
   int bit;
   char data;

   for (int cy=0; cy<sy; cy++)
   {
      bit= cy % 8;
      for(int cx=0; cx<sx; cx++)
      {
         data=bitmap[cx+((cy/8)*sx)];
         if ((data & (1<<bit))>0)
        	 setPixel(x+cx, y+cy);
         else
        	 LCD5110_clrPixel(x+cx, y+cy);
      }
   }
}
