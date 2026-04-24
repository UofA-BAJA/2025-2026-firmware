#include "Procedure.h"
#include "TestSubsystem.h"
#include "Coms.h"


#include <iomanip>
#include <cstdlib> //random

namespace BajaWildcatRacing
{

class TestProcedure : public Procedure{
    public:
        TestSubsystem& testSubsystem;
        Coms& coms;

        byte data[16];


        TestProcedure(TestSubsystem& testSubsystem, Coms& coms)
        : testSubsystem(testSubsystem), coms(coms)
        {
            this->frequency = 10; 

        }
        
        void init() override {
            std::fill(data, data+16, 0x00);
            data[0] = 6;
            data[4] = 7;
            data[8] = 8;
            data[12] = 9;
            std::cout << "Test procedure initialized!" << std::endl;
            
        }

        void execute() override {

            
            // TestDevice::TestStruct st = testSubsystem.getTestStruct();
            // byte d[sizeof(float)];
            // float num = 6.9f;

            // memcpy(d, &num, sizeof(float));

            // coms.sendData(DataType::WHEEL_RPM_FRONT_L, data, 12);
            // coms.sendData(DataType::CAR_SPEED, data, 4);
            // coms.sendData(DataType::MOTOR_RPM, data, 4);
            // coms.sendData(DataType::IMU_ROTATION_X, data, 12);
            // coms.sendData(DataType::IMU_ACCELERATION_X, data, 12);
            // coms.sendData(DataType::BRAKE_PRESSURE_REAR, data, 8);
            // coms.sendData(DataType::CVT_TEMPERATURE, data, 4);
            // coms.sendData(DataType::DISTANCE, data, 4);
            // coms.sendData(DataTypes::IMU_ACCELERATION_Z, data, 4);
            // coms.sendData(DataTypes::CAR_SPEED, data, 100);


            // coms.sendData(DataType::CVT_TEMPERATURE, d, sizeof(float));
            float randomNum = rand() % 50;
            coms.sendData(DataType::CVT_TEMPERATURE, randomNum);
            std::cout << "id 8: " << randomNum << std::endl;
            randomNum = rand() % 50;
            coms.sendData(DataType::CAR_SPEED, randomNum);
            std::cout << "id 2: " << randomNum << std::endl;
            // coms.sendData(DataType::CAR_SPEED, 2026.0f);
            
            // testSubsystem.test(true);
            // testSubsystem.test(false);
            // std::cout << std::fixed;
            // std::cout << std::setprecision(2);
            // std::cout << "Test Struct: " << st.a << " " << st.b << " " <<  st.c << " " << st.d << " " << st.e << std::endl;
        }

        void end() override {
            std::cout << "test procedure ended" << std::endl;
        }

        bool isFinished() override {
            return false;
        }

        std::string toString() override {
            return "Testing Procedure";
        }

    private:
};

}
