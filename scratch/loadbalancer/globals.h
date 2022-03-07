//
// Created by slahmer on 3/1/22.
//

#ifndef NS3_GLOBALS_H
#define NS3_GLOBALS_H

class Globals{
public:
  inline static uint32_t simulationTime = 5;
  inline static uint32_t action_space = 2;
  inline static uint32_t openGymPort = 5555;
  inline static uint32_t simSeed = 1;
  inline static double envStepTime = 1.0;
  inline static uint32_t serverCount = 1;
  inline static uint32_t leafCount = 4;
  inline static uint32_t spineCount = 2;

};


#endif //NS3_GLOBALS_H
