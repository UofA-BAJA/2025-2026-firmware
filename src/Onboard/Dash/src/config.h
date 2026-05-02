#include <Arduino.h>

// I2C
#define I2C_SDA 5
#define I2C_SCL 6

#define I2C_ADDR_UPPER 0x70
#define I2C_ADDR_LOWER 0x71

// SPI (LCD & CAN)
#define SPI_SCK 8
#define SPI_MISO 9
#define SPI_MOSI 10

#define SPI_INT_CAN 1
#define SPI_CS_CAN 43

// Button
#define BUTTON_LOWER 44

// Servos
#define SERVO_TACH 2
#define SERVO_SPEED 3

#define SERVO_TACH_MIN 0
#define SERVO_TACH_MAX 180
#define SERVO_SPEED_MIN 0
#define SERVO_SPEED_MAX 180


/*
-------A-------
|\     |     /|
| \    J    / |
|   H  |  K   |
F    \ | /    B
|     \|/     |
|--G1--|--G2--|
|     /|\     |
E    / | \    C
|   L  |   N  |
| /    M    \ |
|/     |     \|
-------D-------  DP
*/

#undef ALPHANUM_SEG_A
#define ALPHANUM_SEG_A 0b0000000000000100
#undef ALPHANUM_SEG_C
#define ALPHANUM_SEG_C 0b0000000000000001