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

#ifndef DOG_7565R_H
#define DOG_7565R_H


#define DOGM128 1
#define DOGL128 2
#define DOGM132 3

#define VIEW_BOTTOM 0xC0
#define VIEW_TOP 	0xC8


class dog_7565R
{
  public:
    void initialize     (byte p_cs, byte p_si, byte p_clk, byte p_a0, byte p_res, byte type);
    void clear			(void);
    void contrast       (byte contr);
	void view			(byte direction);
	void string         (byte column, byte page, const byte *font_adress, const char *str);
	void rectangle		(byte start_column, byte start_page, byte end_column, byte end_page, byte pattern);
	void picture		(byte column, byte page, const byte *pic_adress);

  private:
    byte p_cs;
    byte p_si;
    byte p_clk;
    byte p_a0;
	byte type;
	boolean hardware;
    boolean top_view;
	
	void position   (byte column, byte page);
    void command	(byte dat);
    void data		(byte dat);
    
    void spi_initialize	(byte cs, byte si, byte clk);
    void spi_put_byte	(byte dat);
    void spi_put		(byte *dat, int len);
	void spi_out		(byte dat);
};

#endif
