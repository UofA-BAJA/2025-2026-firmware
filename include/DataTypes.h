
#ifndef DATATYPES_H
#define DATATYPES_H

// The hex code associated with the data type

// This code will be used by both the pit side and the car side
// It is also the code that will serve as the "smift amount" for the mask value,
// as described in the live data packet protocol 
enum DataType{
    //oops what happened to zero???
    WHEEL_RPM = 0x01,
    CAR_SPEED = 0x02,
    MOTOR_RPM = 0x03,
    IMU_ROTATION = 0x04, 
    IMU_ACCELERATION = 0x05,
    BRAKE_PRESSURE = 0x06,
    SHOCK_DISPLACEMENT = 0x07,
    CVT_TEMPERATURE = 0x08,
    DISTANCE = 0x09,
    GPS_POSITION = 0x10,

    //...
    ERRORS = 0xAA,
    // Datatypes at or below 255 (0xFF) are the only ones that can be sent via radio, others will be ignored
};

typedef struct RotationXYZ{
    float rotationx;
    float rotationy;
    float rotationz;
}RotationXYZ;

typedef struct AccelerationXYZ{
    float accelerationx;
    float accelerationy;
    float accelerationz;
}AccelerationXYZ;

typedef struct WheelRPM{
    float frontLeft;
    float frontRight;
    float rear;
}WheelSpeed;

typedef struct ShockDisplacement{
    float frontLeft;
    float frontRight;
    float rearLeft;
    float rearRight;
}ShockDisplacement;

typedef struct BrakePressure{
    float front;
    float rear;
}BrakePressure;

typedef struct GPSPosition{
    float longitude;
    float latitude;
    float atltitude;
}GPSPosition;

typedef struct EngineRPM{
    float rpm;
} EngineRPM;

typedef struct CVTTemp{
    float temp;
}

#endif