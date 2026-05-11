#include <Arduino.h>

// I2C
#define I2C_SDA 9
#define I2C_SCL 8

#define I2C_ADDR_UPPER 0x70
#define I2C_ADDR_LOWER 0x71

// SPI (LCD & CAN)
#define SPI_SCK 36
#define SPI_MISO 37
#define SPI_MOSI 35


#define SPI_CS_CAN 6
#define SPI_CS_LCD 4

// LCD/CAN Specifics
#define LCD_RST 5
#define LCD_A0 21

#define CAN_ADDR 9
#define SPI_INT_CAN 34

// Button
#define BUTTON_LOWER 3

// Servos
#define SERVO_TACH 1
#define SERVO_SPEED 2

#define SERVO_TACH_MIN 0
#define SERVO_TACH_MAX 160
#define SERVO_SPEED_MIN 160
#define SERVO_SPEED_MAX 0