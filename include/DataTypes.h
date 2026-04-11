
#ifndef DATATYPES_H
#define DATATYPES_H

// The hex code associated with the data type

// This code will be used by both the pit side and the car side
// It is also the code that will serve as the "smift amount" for the mask value,
// as described in the live data packet protocol 
enum DataTypes{
    KEEP_ALIVE = 0x00,
    WHEEL_RPM_FRONT_L = 0x01,
    WHEEL_RPM_FRONT_R = 0x02,
    WHEEL_RPM_BACK = 0x03,
    CAR_SPEED = 0x04,
    MOTOR_RPM = 0x05,
    IMU_ROTATION_X = 0x06,
    IMU_ROTATION_Y = 0x07,
    IMU_ROTATION_Z = 0x08,
    IMU_ACCELERATION_X = 0x09,
    IMU_ACCELERATION_Y = 0x0A,
    IMU_ACCELERATION_Z = 0x0B,
    BRAKE_PRESSURE_REAR = 0x0C,
    BRAKE_PRESSURE_FRONT = 0x0D,
    CVT_TEMPERATURE = 0x0E,
    DISTANCE = 0x0F,
    RESERVE_16,
    RESERVE_17,
    RESERVE_18,
    RESERVE_19,
    RESERVE_20,
    RESERVE_21,
    RESERVE_22,
    RESERVE_23,
    RESERVE_24,
    RESERVE_25,
    RESERVE_26,
    RESERVE_27,
    RESERVE_28,
    RESERVE_29,
    RESERVE_30,
    RESERVE_31,
    RESERVE_32
    // Datatypes at or below 255 (0xFF) are the only ones that can be sent via radio, others will be ignored
};

#endif