/*
 *
 *  Class IMU
 *
 *         Author:  Matthew Larson & Karsten Yin
 *
 *        Purpose:  Provide an implementation of the IMU device, utilizing the
 *        CanDispatcher class to send and receive CAN commands
 *
 *  Inherits From:  None
 *
 *     Interfaces:  None
 *
 *+-----------------------------------------------------------------------
 *
 *      Constants:  
 *
 *+-----------------------------------------------------------------------
 *
 *   Constructors:  [List the names and arguments of all defined
 *                   constructors.]
 *
 *  Class Methods:  [List the names, arguments, and return types of all
 *                   public class methods.]
 *
 * Inst. Methods:  [List the names, arguments, and return types of all
 *                   public instance methods.]
 *
*/

#include "IMU.h"

namespace BajaWildcatRacing
{

    IMU::IMU(CANDispatcher& canDispatcher, byte deviceId) : CANDevice(canDispatcher, deviceId){
        rot.rotationx = 0;
        rot.rotationy = 0;
        rot.rotationz = 0;
        
        accel.accelerationx = 0;
        accel.accelerationy = 0;
        accel.accelerationz = 0;
    }

    RotationXYZ IMU::getLatestRotation(){

        sendCanRequest(0x00, &rot, sizeof(RotationXYZ));
        return rot;

    }

    AccelerationXYZ IMU::getLatestAcceleration(){

        sendCanRequest(0x01, &accel, sizeof(AccelerationXYZ));
        return accel;

    }

}
