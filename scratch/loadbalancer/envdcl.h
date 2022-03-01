//
// Created by slahmer on 3/1/22.
//

#ifndef NS3_ENVDCL_H
#define NS3_ENVDCL_H


#include "ns3/opengym-module.h"
#include "ns3/nstime.h"

namespace ns3 {

class envdcl : public OpenGymEnv
{
public:
  envdcl ();
  envdcl (Time stepTime);
  virtual ~envdcl ();
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

#endif //NS3_ENVDCL_H
