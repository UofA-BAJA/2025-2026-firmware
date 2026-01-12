/*
*   This library is based on the DOG_7565R, but has been rewritten to better suit the dash
*
*/

#ifndef BajaLCD_H
#define BajaLCD_H

// Orientation with main strip of pins as reference (NOT backlight)
#define PINS_TOP 0xC8
#define PINS_BOTTOM 0xC0

#define ALIGN_LEFT   0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT  2

class bajaLCD{

    public:

    // Setup/Config funcs
    void init(byte pin_CS, byte pin_MOSI, byte pin_SCK, byte pin_modeSwitch, byte pin_RST);

    void clear(void);

    void setContrast(byte contr);

    // Utility/Helper funcs
    void send(byte *data, int len, bool useCS = true);
    void sendCommand(byte data);
    void sendData(byte data);

    void position(byte column, byte page);

    void setOrientation(byte direction);

    // Display funcs

    

    void print(byte page, const byte *font_address, const char *str, byte alignment = ALIGN_LEFT);
	void box(byte start_column, byte start_page, byte end_column, byte end_page, byte pattern);
	void image(byte column, byte page, const byte *pic_address);

    private:

    // Pin definitions
    byte pin_CS;
    byte pin_MOSI;
    byte pin_SCK;

    // Alias for A0, pulling to LOW sets display to COMMAND mode, pulling to HIGH sets display to DATA mode
    byte pin_modeSwitch; 

    // Misc variables----

    // defaults to false, setting "top" of the screen to be the side with the main strip of pins
    boolean top_view;

};

#endif