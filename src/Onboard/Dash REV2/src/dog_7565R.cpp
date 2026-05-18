/*
  * Copyright (c) 2014 by ELECTRONIC ASSEMBLY GmbH / DISPLAY VISIONS GmbH <technik@lcd-module.de>
  * EA DOG Graphic (ST7565R) software library for arduino.
  *
  * MIT License Conditions
  * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
  * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  */


#include <Arduino.h>
#include <SPI.h>

// ESP32/ESP8266 compatibility - use pgmspace.h which works on both platforms
#ifdef ESP32
  #include <pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#else
  #include <avr/pgmspace.h>
#endif

#include "dog_7565R.h"


#define INITLEN 14
byte init_DOGM128[INITLEN] = {0x40, 0xA1, 0xC0, 0xA6, 0xA2, 0x2F, 0xF8, 0x00, 0x27, 0x81, 0x16, 0xAC, 0x00, 0xAF};
byte init_DOGL128[INITLEN] = {0x40, 0xA1, 0xC0, 0xA6, 0xA2, 0x2F, 0xF8, 0x00, 0x27, 0x81, 0x10, 0xAC, 0x00, 0xAF};
byte init_DOGM132[INITLEN] = {0x40, 0xA1, 0xC0, 0xA6, 0xA2, 0x2F, 0xF8, 0x00, 0x23, 0x81, 0x1F, 0xAC, 0x00, 0xAF};

//----------------------------------------------------public Functions----------------------------------------------------
//Please use these functions in your sketch

/*----------------------------
Func: DOG-INIT
Desc: Initializes SPI Hardware/Software and DOG Displays
Vars: CS-Pin, MOSI-Pin, SCK-Pin (MOSI=SCK Hardware else Software), A0-Pin (high=data, low=command), p_res = Reset-Pin, type (1=EA DOGM128-6, 2=EA DOGL128-6)
------------------------------*/
void dog_7565R::initialize(byte p_cs, byte p_si, byte p_clk, byte p_a0, byte p_res, byte type){
	Serial.println("1");
	byte *ptr_init; //pointer to the correct init values
	top_view = false; //default = bottom view

	Serial.println("2");
	dog_7565R::p_a0 = p_a0;
	pinMode(p_a0, OUTPUT);
	

	Serial.println("3");
	// Set CS to deselct slaves
	pinMode(p_cs, OUTPUT);
	digitalWrite(p_cs, HIGH);

	Serial.println("6");
	//Set pin Configuration
	this->p_cs = p_cs;
	this->p_si = p_si;
	this->p_clk = p_clk;

	Serial.println("7");
	//perform a Reset
	pinMode(p_res, OUTPUT);

	Serial.println("8");
	digitalWrite(p_res, LOW);
	delayMicroseconds(10);
	digitalWrite(p_res, HIGH);
	delay(1);

	Serial.println("9");
	//Init DOGM displays, depending on users choice
	ptr_init = init_DOGM128; //default pointer for wrong parameters
	if(type == DOGM128) 		ptr_init = init_DOGM128;
	else if(type == DOGL128) 	ptr_init = init_DOGL128;
	else if(type == DOGM132) 	ptr_init = init_DOGM132;

	Serial.println("10");
	dog_7565R::type = type;

	Serial.println("11");
	SPI.begin();

	Serial.println("11.5");
	digitalWrite(p_a0, LOW); //init display

	Serial.println("12");
	spi_put(ptr_init, INITLEN);

	Serial.println("13");
	clear();
}

/*----------------------------
Func: clear_display
Desc: clears the entire DOG-Display
Vars: ---
------------------------------*/
void dog_7565R::clear(void)
{
	byte page, column;
	byte page_cnt = 8, column_cnt = 128;

	if(type == DOGM132)
	{
		page_cnt = 4;
		column_cnt = 132;
	}

	for(page = 0; page < page_cnt; page++) //Display has 8 pages
	{
		position(0,page);
		digitalWrite(p_cs, LOW);
		digitalWrite(p_a0, HIGH);
		SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

		for(column = 0; column < column_cnt; column++) //clear the whole page line
			SPI.transfer(0x00);

		digitalWrite(p_cs, HIGH);
		SPI.endTransaction();
	}
}

/*----------------------------
Func: contrast
Desc: sets contrast to the DOG-Display
Vars: byte contrast (0..63)
------------------------------*/
void dog_7565R::contrast(byte contr)
{
	command(0x81);   		//double byte command
	command(contr&0x3F);	//contrast has only 6 bits
}

/*----------------------------
Func: view
Desc: ssets the display viewing direction
Vars: direction (top view 0xC8, bottom view (default) = 0xC0)
------------------------------*/
void dog_7565R::view(byte direction)
{
	if(direction == VIEW_TOP)
	{
		top_view = true;
		command(0xA0);
	}
	else
	{
		top_view = false;
		command(0xA1);
	}

	command(direction);

	clear(); //Clear screen, as old content is not usable (mirrored)
}

/*----------------------------
Func: string
Desc: shows string with selected font on position
Vars: column (0..127/131), page(0..3/7),  font adress in programm memory, stringarray
------------------------------*/
void dog_7565R::string(byte column, byte page, const byte *font_adress, const char *str)
{
	unsigned int pos_array; 										//Postion of character data in memory array
	byte x, y, column_cnt, width_max;								//temporary column and page adress, couloumn_cnt tand width_max are used to stay inside display area
	byte start_code, last_code, width, page_height, bytes_p_char;	//font information, needed for calculation
	const char *string;



	start_code 	 = pgm_read_byte(&font_adress[2]);  //get first defined character
	last_code	 = pgm_read_byte(&font_adress[3]);  //get last defined character
	width		 = pgm_read_byte(&font_adress[4]);  //width in pixel of one char
	page_height  = pgm_read_byte(&font_adress[6]);  //page count per char
	bytes_p_char = pgm_read_byte(&font_adress[7]);  //bytes per char

  if(type != DOGM132 && page_height + page > 8) //stay inside display area
		page_height = 8 - page;
  else  if(type == DOGM132 && page_height + page > 4)
    page_height = 4 - page;


	//The string is displayed character after character. If the font has more then one page,
	//the top page is printed first, then the next page and so on
	for(y = 0; y < page_height; y++)
	{
		position(column, page+y); //set startpositon and page
		column_cnt = column; //store column for display last column check
		string = str;             //temporary pointer to the beginning of the string to print
		SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
		digitalWrite(p_a0, HIGH);
		digitalWrite(p_cs, LOW);
		while(*string != 0)
		{
			if((byte)*string < start_code || (byte)*string > last_code) //make sure data is valid
				string++;
			else
			{
				//calculate positon of ascii character in font array
				//bytes for header + (ascii - startcode) * bytes per char)
				pos_array = 8 + (unsigned int)(*string++ - start_code) * bytes_p_char;
				pos_array += y*width; //get the dot pattern for the part of the char to print

        if(type != DOGM132 && column_cnt + width > 128) //stay inside display area
					width_max = 128-column_cnt;
        else if(type == DOGM132 && column_cnt + width > 132)
           width_max = 132-column_cnt;
				else
					width_max = width;

					for(x=0; x < width_max; x++) //print the whole string
					{
						SPI.transfer(pgm_read_byte(&font_adress[pos_array+x]));
						//SPI.transfer(pgm_read_byte(&font_adress[pos_array+x])); //double width font (bold)
					}
			}
		}
		digitalWrite(p_cs, HIGH);
		SPI.endTransaction();
	}
}

// Prints text to display (lightly modified from original library)
void dog_7565R::print(byte page, const byte *font_address, const char *str, byte alignment){

    unsigned int pos_array; 										//Postion of character data in memory array
	byte x, y, column, column_cnt, width_max;								//temporary column and page adress, couloumn_cnt tand width_max are used to stay inside display area
	byte start_code, last_code, width, page_height, bytes_p_char;	//font information, needed for calculation
	const char *string;

    start_code 	 = pgm_read_byte(&font_address[2]);  //get first defined character
	last_code	 = pgm_read_byte(&font_address[3]);  //get last defined character
    width		 = pgm_read_byte(&font_address[4]);  //width in pixel of one char

    // Determine if calculations are needed for alignment
    if(alignment != ALIGN_LEFT){

        // Create copy of input string to modify
        byte textWidth;
        const char *temp = str;

        // Find number of chars in string to determine width of string
        while(*temp != 0){
            if((byte)*temp >= start_code && (byte)*temp <= last_code) {
                textWidth += width;
            }
            temp++;
        }

        // Determine start column based on parameter
        if(alignment == ALIGN_CENTER){
            column = (128 - textWidth) / 2;
        }
        else if(alignment == ALIGN_RIGHT){
            column = 128 - textWidth;
        }

    }
    else{
        column = 0;
    }

	page_height  = pgm_read_byte(&font_address[6]);  //page count per char
	bytes_p_char = pgm_read_byte(&font_address[7]);  //bytes per char

    if(page_height + page > 8) page_height = 8 - page; //stay inside display area
		
	//The string is displayed character after character. If the font has more then one page,
	//the top page is printed first, then the next page and so on
	for(y = 0; y < page_height; y++){

		position(column, page+y); //set startpositon and page
		column_cnt = column; //store column for display last column check
		string = str;             //temporary pointer to the beginning of the string to print
		
		SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
        digitalWrite(p_a0, HIGH);
		digitalWrite(p_cs, LOW);
		
        while(*string != 0){

			if((byte)*string < start_code || (byte)*string > last_code) string++;//make sure data is valid
			else{

				//calculate positon of ascii character in font array
				//bytes for header + (ascii - startcode) * bytes per char)
				pos_array = 8 + (unsigned int)(*string++ - start_code) * bytes_p_char;
				pos_array += y*width; //get the dot pattern for the part of the char to print

                if(column_cnt + width > 128) width_max = 128-column_cnt; //stay inside display area
                else width_max = width;

                for(x=0; x < width_max; x++){ //print the whole string
                    SPI.transfer(pgm_read_byte(&font_address[pos_array+x]));
                    

                }
			}
		}
		digitalWrite(p_cs, HIGH);
		SPI.endTransaction();
	}
}

/*----------------------------
Func: rectangle
Desc: shows a pattern filled rectangle on the display
Vars: start and end column (0..127/131) and page(0..3/7), bit pattern
------------------------------*/
void dog_7565R::rectangle(byte start_column, byte start_page, byte end_column, byte end_page, byte pattern)
{
	byte x, y;

  if(type != DOGM132 && end_column > 128) //stay inside display area
		end_column = 128;
  else if(type == DOGM132 && end_column > 132)
     end_column = 132;
	if(type != DOGM132 && end_page > 7)
		end_page = 7;
  else if (type == DOGM132 && end_page > 3)
	  end_page = 3;

  for(y=start_page; y<=end_page; y++)
	{
		position(start_column, y);
		digitalWrite(p_a0, HIGH);
		digitalWrite(p_cs, LOW);
		SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

		for(x=start_column; x<=end_column; x++)
			SPI.transfer(pattern);

		digitalWrite(p_cs, HIGH);
		SPI.endTransaction();
	}
}

/*----------------------------
Func: picture
Desc: shows a BLH-picture on the display (see BitMapEdit EA LCD-Tools (http://www.lcd-module.de/support.html))
Vars: column (0..127/131) and page(0..3/7), program memory adress of data
------------------------------*/
void dog_7565R::picture(byte column, byte page, const byte *pic_adress)
{
	byte c,p;
	unsigned int byte_cnt = 2;
	byte width, page_cnt;

	width = pgm_read_byte(&pic_adress[0]);
	page_cnt = (pgm_read_byte(&pic_adress[1]) + 7) / 8; //height in pages, add 7 and divide by 8 for getting the used pages (byte boundaries)

  if(width + column > 128 && type != DOGM132) //stay inside display area
		width = 128 - column;
  else if(width + column > 132 && type == DOGM132)
    width = 132 - column;

  if(type != DOGM132 && page_cnt + page > 8)
		page_cnt = 8 - page;
  else if(type == DOGM132 && page_cnt + page > 4)
    page_cnt = 4 - page;

	for(p=0; p<page_cnt; p++)
	{
		position(column, page + p);
		digitalWrite(p_a0, HIGH);
		digitalWrite(p_cs, LOW);
		SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

		for(c=0; c<width; c++)
			SPI.transfer(pgm_read_byte(&pic_adress[byte_cnt++]));

		digitalWrite(p_cs, HIGH);
		SPI.endTransaction();
	}
}

//----------------------------------------------------private Functions----------------------------------------------------
//normally you don't need those functions in your sketch

/*----------------------------
Func: position
Desc: sets write pointer in DOG-Display
Vars: column (0..127/131), page(0..3/7)
------------------------------*/
void dog_7565R::position(byte column, byte page)
{
	if(top_view && type != DOGM132)
		column += 4;

	command(0x10 + (column>>4)); 	//MSB adress column
	command(0x00 + (column&0x0F));	//LSB adress column
	command(0xB0 + (page&0x0F)); 	//adress page
}

/*----------------------------
Func: command
Desc: Sends a command to the DOG-Display
Vars: data
------------------------------*/
void dog_7565R::command(byte dat)
{
	digitalWrite(p_a0, LOW);
	spi_put_byte(dat);
}

/*----------------------------
Func: data
Desc: Sends data to the DOG-Display
Vars: data
------------------------------*/
void dog_7565R::data(byte dat)
{
	 digitalWrite(p_a0, HIGH);
	 spi_put_byte(dat);
}

/*----------------------------
Func: spi_put_byte
Desc: Sends one Byte using CS
Vars: data
------------------------------*/
void dog_7565R::spi_put_byte(byte dat)
{
	digitalWrite(p_cs, LOW);
	SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
	SPI.transfer(dat);
	SPI.endTransaction();
	digitalWrite(p_cs, HIGH);
}

/*----------------------------
Func: spi_put
Desc: Sends bytes using CS
Vars: ptr to data and len
------------------------------*/
void dog_7565R::spi_put(byte *dat, int len)
{
	digitalWrite(p_cs, LOW);
	SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

	do
	{
		SPI.transfer(*dat++);
	}while(--len);

	digitalWrite(p_cs, HIGH);
	SPI.endTransaction();
}
