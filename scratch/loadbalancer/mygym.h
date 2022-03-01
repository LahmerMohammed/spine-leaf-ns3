
#ifndef MY_GYM_ENTITY_H
#define MY_GYM_ENTITY_H

#include "ns3/opengym-module.h"
#include "ns3/nstime.h"

namespace ns3 {

class DataCenterEnv : public OpenGymEnv
{
public:
  DataCenterEnv ();
  DataCenterEnv (Time stepTime);
  virtual ~DataCenterEnv ();
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  Ptr<OpenGymSpace> GetActionSpace();
  Ptr<OpenGymSpace> GetObservationSpace();
  bool GetGameOver();
  Ptr<OpenGymDataContainer> GetObservation();
  float GetReward();
  std::string GetExtraInfo();
  bool ExecuteActions(Ptr<OpenGymDataContainer> action);

private:
  void ScheduleNextStateRead();

  Time m_interval;
};

}


#endif // MY_GYM_ENTITY_H
