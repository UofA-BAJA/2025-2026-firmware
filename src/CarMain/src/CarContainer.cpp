/*
 *
 *  Class: CarContainer
 *
 *         Author:  Matthew Larson, Karsten Yin, Julian Rendon
 *
 *        Purpose:  Defines the functionality of the car. All subsystems and procedures are defined,
 *        created, and bound here. 
 *
 *  Inherits From:  None
 *
 *     Interfaces:  None
 *
 *+-----------------------------------------------------------------------
 *
 *      Constants:  None
 *
 *+-----------------------------------------------------------------------
 *
 *   Constructors:  CarContainer(ProcedureScheduler* procedureScheduler, CANDispatcher* canDispatcher, DataStorage* dataStorage, Coms* coms) --
 *   the location to define all the subsystems and procedures in the car and subsequently bind the procedures to commands.
 *
 *  Class Methods:  [List the names, arguments, and return types of all
 *                   public class methods.]
 *
 * Inst. Methods:  [List the names, arguments, and return types of all
 *                   public instance methods.]
 *
*/


#include <iostream>

#include "CarContainer.h"

namespace BajaWildcatRacing
{

    CarContainer::CarContainer(
                               ProcedureScheduler& procedureScheduler, 
                               CANDispatcher& canDispatcher,
                               DataStorage& dataStorage,
                               Coms& coms)
    : imuSubsystem(canDispatcher)
    , drivetrainSubsystem(canDispatcher)
    , dashSubsystem(canDispatcher)
    , testSubsystem(canDispatcher)
    , gpsSubsystem(canDispatcher)
    {

        ///////////////////////////////////////////////////////////
        // Testing procedures
        ///////////////////////////////////////////////////////////

        // procedureScheduler.bindCommand<TestProcedure>(
        //     std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}),
        //     std::unordered_set<Command>({Command::END_LOG}),
        //     testSubsystem, coms, dataStorage
        // );

        ///////////////////////////////////////////////////////////
        // Logging procedures
        ///////////////////////////////////////////////////////////

        procedureScheduler.bindCommand<IMULoggingProcedure>(
             std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}), 
             std::unordered_set<Command>({Command::END_LOG}), 
             imuSubsystem, dataStorage, coms
        );
        
       	procedureScheduler.bindCommand<TemperatureLoggingProcedure>(
            std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}),
             std::unordered_set<Command>({Command::END_LOG}),
              drivetrainSubsystem, dataStorage, coms, dashSubsystem
        );

        procedureScheduler.bindCommand<CarStateProcedure>(
             std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}),
             std::unordered_set<Command>({Command::END_LOG}),
             drivetrainSubsystem, dashSubsystem, dataStorage, coms
        );


         procedureScheduler.bindCommand<RPMLoggingProcedure>(
             std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}),
             std::unordered_set<Command>({Command::END_LOG}),
             drivetrainSubsystem, dataStorage, coms
        );

        procedureScheduler.bindCommand<SpeedLoggingProcedure>(
             std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}),
             std::unordered_set<Command>({Command::END_LOG}),
             drivetrainSubsystem, dataStorage, coms
         );

        procedureScheduler.bindCommand<DisplacementLoggingProcedure>(
            std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}),
           std::unordered_set<Command>({Command::END_LOG}),
           drivetrainSubsystem, dataStorage, coms
        );

        procedureScheduler.bindCommand<BrakePressureLoggingProcedure>(
            std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}),
           std::unordered_set<Command>({Command::END_LOG}),
            drivetrainSubsystem, dataStorage, coms
        );

         procedureScheduler.bindCommand<GPSLoggingProcedure>(
             std::unordered_set<Command>({Command::DEFAULT_CAR_START, Command::START_LOG}),
             std::unordered_set<Command>({Command::END_LOG}),
             gpsSubsystem, dataStorage, coms
         );

        ///////////////////////////////////////////////////////////
        // Other Procedures
        ///////////////////////////////////////////////////////////

        // The dash is always running.
         procedureScheduler.bindCommand<DashProcedure>( 
            std::unordered_set<Command>({Command::DEFAULT_CAR_START}),
            std::unordered_set<Command>({}),
            dashSubsystem, drivetrainSubsystem, imuSubsystem
         );



        CarLogger::Log("Car Started");
        std::cout << "Car Container Constructor called" << std::endl;
    }

}
