/*
*   This library is based on the DOG_7565R, but has been rewritten to better suit the dash
*
*/

#include <Arduino.h>
#include <SPI.h>

// Use the non-AVR programspace library (since we're not gonna use AVR)
#include <pgmspace.h>

#include "bajaLCD.h"

// Initialization command array
#define INITLEN 14
byte init_DOGM128[INITLEN] = {0x40, 0xA1, 0xC0, 0xA6, 0xA2, 0x2F, 0xF8, 0x00, 0x27, 0x81, 0x16, 0xAC, 0x00, 0xAF};

// ----- Function Defs -----

// Setup/Config funcs -----

// Initialization func, set pin_MOSI and pin_SCK to the same value to use hardware-set SPI pins.
void bajaLCD::init(byte pin_CS, byte pin_MOSI, byte pin_SCK, byte pin_modeSwitch, byte pin_RST) {
    
    // Configure vars -----
    byte *initPtr;
    initPtr = init_DOGM128;
    
    top_view = false;

    // Configure IO pins -----

    // Secondary pins
    this->pin_modeSwitch = pin_modeSwitch; 

    // SPI Setup -----
    this->pin_CS = pin_CS;

    // If pin_MOSI and pin_SCK are the same, use hardware SPI
    if(pin_MOSI == pin_SCK) {
        this->pin_MOSI = MOSI;
        this->pin_SCK = SCK;
    }
    else{
        this->pin_MOSI = pin_MOSI;
        this->pin_SCK = pin_SCK;
    }

    pinMode(this->pin_CS, OUTPUT);
    pinMode(this->pin_MOSI, OUTPUT);
    pinMode(this->pin_SCK, OUTPUT);

    pinMode(pin_RST, OUTPUT);
    pinMode(pin_modeSwitch, OUTPUT);

    // Pull CS to high to deselect the LCD
    digitalWrite(this->pin_CS, HIGH);

    // Set up CLK line
    digitalWrite(this->pin_SCK, HIGH);

    // Formally initialize SPI
    SPI.begin();
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

    // Start LCD -----

    // Reset LCD
    digitalWrite(pin_RST, LOW);
	delayMicroseconds(10);
	digitalWrite(pin_RST, HIGH);
	delay(1);

    // send out initialization command sequence
    digitalWrite(this->pin_modeSwitch, LOW);
    send(initPtr, INITLEN, true);

}

// Clear LCD, takes no arguments
void bajaLCD::clear(void){
    
    byte page, column;
    byte pageCount = 8, columnCount = 128;

    byte clearVal = 0x00;

    for(page = 0; page < pageCount; page++){

        // Set cursor to first column of page
		position(0,page);

        // Enable data transfer and set to data transfer mode
		digitalWrite(pin_CS, LOW);
		digitalWrite(pin_modeSwitch, HIGH);

        // Work through each column of the page and clear it
		for(column = 0; column < columnCount; column++) send(&clearVal, 1, false);

        // Deselect chip once done
		digitalWrite(pin_CS, HIGH);
	}
}

// Set Contrast, accepts values 0-63
void bajaLCD::setContrast(byte contrast){
    sendCommand(0x81); //Set contrast
    sendData(contrast);
}

// Utility/Helper funcs -----

// Wrapper function for SPI sending to LCD
void bajaLCD::send(byte *data, int len,bool useCS){

    // Enable/disable use of CS pin
    if(useCS) digitalWrite(pin_CS, LOW);



    if(len == 1) SPI.transfer(data[0]);
    else{
        do{
            SPI.transfer(*data++);
        }while(--len);
    
        if(useCS) digitalWrite(pin_CS, HIGH);
    }

}

// Wraps send function together with setting A0 (Mode switching pin) to LOW
void bajaLCD::sendCommand(byte data){
    digitalWrite(pin_modeSwitch, LOW);
    send(&data, 1, true);
}

// Wraps send function together with setting A0 (Mode switching pin) to HIGH
void bajaLCD::sendData(byte data){
    digitalWrite(pin_modeSwitch, HIGH);
    send(&data, 1, true);
}

// Position "cursor" of LCD
void bajaLCD::position(byte column, byte page){

    if(top_view) column += 4;

    sendCommand(0x10 + (column>>4)); 	//MSB adress column
	sendCommand(0x00 + (column&0x0F));	//LSB adress column
	sendCommand(0xB0 + (page&0x0F)); 	//address page	

}

// Set LCD orientation to either PINS_TOP or PINS_BOTTOM
void bajaLCD::setOrientation(byte direction){
    if(direction == PINS_TOP){
        top_view = true;
        sendCommand(0xA0);
    }
    else if(direction == PINS_BOTTOM){
        top_view = false;
        sendCommand(0xA1);
    }

    sendCommand(direction);

    // clear display after orientation change
    clear();
}


// Display funcs -----

// Prints text to display (lightly modified from original library)
void bajaLCD::print(byte page, const byte *font_address, const char *str, byte alignment){

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
		
        digitalWrite(pin_modeSwitch, HIGH);
		digitalWrite(pin_CS, LOW);
		
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
		digitalWrite(pin_CS, HIGH);
	}
}

void bajaLCD::box(byte start_column, byte start_page, byte end_column, byte end_page, byte pattern){
    byte x, y;

    if(end_column > 128) end_column = 128;//stay inside display area
        
    if(end_page > 7) end_page = 7;

    for(y=start_page; y<=end_page; y++)
    {
        position(start_column, y);
        digitalWrite(pin_modeSwitch, HIGH);
        digitalWrite(pin_CS, LOW);

        for(x=start_column; x<=end_column; x++)
            SPI.transfer(pattern);

        digitalWrite(pin_CS, HIGH);
    }
}

void bajaLCD::image(byte column, byte page, const byte *pic_adress){
    byte c,p;
    unsigned int byte_cnt = 2;
    byte width, page_cnt;

    width = pgm_read_byte(&pic_adress[0]);
    page_cnt = (pgm_read_byte(&pic_adress[1]) + 7) / 8; //height in pages, add 7 and divide by 8 for getting the used pages (byte boundaries)

    if(width + column > 128) width = 128 - column; //stay inside display area
        

    if(page_cnt + page > 8) page_cnt = 8 - page;

    for(p=0; p<page_cnt; p++){
        position(column, page + p);
        digitalWrite(pin_modeSwitch, HIGH);
        digitalWrite(pin_CS, LOW);

        for(c=0; c<width; c++) SPI.transfer(pgm_read_byte(&pic_adress[byte_cnt++]));

        digitalWrite(pin_CS, HIGH);
    }
}