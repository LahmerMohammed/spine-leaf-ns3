//
// Created by slahmer on 3/1/22.
//

#include "envdcl.h"
#include "ns3/simulator.h"
#include "ns3/random-variable-stream.h"

NS_LOG_COMPONENT_DEFINE ("envdcl");

using namespace ns3;

envdcl::envdcl ()
{
  NS_LOG_FUNCTION (this);
  m_interval = Seconds(0.1);

  Simulator::Schedule (Seconds(0.0), &envdcl::ScheduleNextStateRead, this);
}

envdcl::envdcl (Time stepTime)
{
  NS_LOG_FUNCTION (this);
  m_interval = stepTime;

  Simulator::Schedule (Seconds(0.0), &envdcl::ScheduleNextStateRead, this);
}

void
envdcl::ScheduleNextStateRead ()
{
  NS_LOG_FUNCTION (this);
  Simulator::Schedule (m_interval, &envdcl::ScheduleNextStateRead, this);
  Notify();
}

envdcl::~envdcl ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
ns3::envdcl::GetTypeId (void)
{
  static TypeId tid = TypeId ("envdcl")
                          .SetParent<OpenGymEnv> ()
                          .SetGroupName ("OpenGym")
                          .AddConstructor<envdcl> ()
      ;
  return tid;
}

void
envdcl::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

/*
Define observation space
*/
Ptr<OpenGymSpace>
envdcl::GetObservationSpace()
{
  uint32_t nodeNum = 5;
  float low = 0.0;
  float high = 10.0;
  std::vector<uint32_t> shape = {nodeNum,};
  std::string dtype = TypeNameGet<uint32_t> ();

  Ptr<OpenGymDiscreteSpace> discrete = CreateObject<OpenGymDiscreteSpace> (nodeNum);
  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  Ptr<OpenGymDictSpace> space = CreateObject<OpenGymDictSpace> ();
  space->Add("myVector", box);
  space->Add("myValue", discrete);

  NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
  return space;
}

/*
Define action space
*/
Ptr<OpenGymSpace>
envdcl::GetActionSpace()
{
  uint32_t nodeNum = 5;
  float low = 0.0;
  float high = 10.0;
  std::vector<uint32_t> shape = {nodeNum,};
  std::string dtype = TypeNameGet<uint32_t> ();

  Ptr<OpenGymDiscreteSpace> discrete = CreateObject<OpenGymDiscreteSpace> (nodeNum);
  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  Ptr<OpenGymDictSpace> space = CreateObject<OpenGymDictSpace> ();
  space->Add("myActionVector", box);
  space->Add("myActionValue", discrete);

  NS_LOG_UNCOND ("MyGetActionSpace: " << space);
  return space;
}

/*
Define game over condition
*/
bool
envdcl::GetGameOver()
{
  bool isGameOver = false;
  bool test = false;
  static float stepCounter = 0.0;
  stepCounter += 1;
  if (stepCounter == 10 && test) {
      isGameOver = true;
    }
  NS_LOG_UNCOND ("MyGetGameOver: " << isGameOver);
  return isGameOver;
}

/*
Collect observations
*/
Ptr<ns3::OpenGymDataContainer>
envdcl::GetObservation()
{
  uint32_t nodeNum = 5;
  uint32_t low = 0.0;
  uint32_t high = 10.0;
  Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();

  std::vector<uint32_t> shape = {nodeNum,};
  Ptr<OpenGymBoxContainer<uint32_t> > box = CreateObject<OpenGymBoxContainer<uint32_t> >(shape);

  // generate random data
  for (uint32_t i = 0; i<nodeNum; i++){
      uint32_t value = rngInt->GetInteger(low, high);
      box->AddValue(value);
    }

  Ptr<OpenGymDiscreteContainer> discrete = CreateObject<OpenGymDiscreteContainer>(nodeNum);
  uint32_t value = rngInt->GetInteger(low, high);
  discrete->SetValue(value);

  Ptr<OpenGymDictContainer> data = CreateObject<OpenGymDictContainer> ();
  data->Add("myVector",box);
  data->Add("myValue",discrete);

  // Print data from tuple
  Ptr<OpenGymBoxContainer<uint32_t> > mbox = DynamicCast<OpenGymBoxContainer<uint32_t> >(data->Get("myVector"));
  Ptr<OpenGymDiscreteContainer> mdiscrete = DynamicCast<OpenGymDiscreteContainer>(data->Get("myValue"));
  NS_LOG_UNCOND ("MyGetObservation: " << data);
  NS_LOG_UNCOND ("---" << mbox);
  NS_LOG_UNCOND ("---" << mdiscrete);

  return data;
}

/*
Define reward function
*/
float
envdcl::GetReward()
{
  static float reward = 0.0;
  reward += 1;
  return reward;
}

/*
Define extra info. Optional
*/
std::string
envdcl::GetExtraInfo()
{
  std::string myInfo = "testInfo";
  myInfo += "|123";
  NS_LOG_UNCOND("MyGetExtraInfo: " << myInfo);
  return myInfo;
}

/*
Execute received actions
*/
bool
envdcl::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
  Ptr<OpenGymDictContainer> dict = DynamicCast<OpenGymDictContainer>(action);
  Ptr<OpenGymBoxContainer<uint32_t> > box = DynamicCast<OpenGymBoxContainer<uint32_t> >(dict->Get("myActionVector"));
  Ptr<OpenGymDiscreteContainer> discrete = DynamicCast<OpenGymDiscreteContainer>(dict->Get("myActionValue"));

  NS_LOG_UNCOND ("MyExecuteActions: " << action);
  NS_LOG_UNCOND ("---" << box);
  NS_LOG_UNCOND ("---" << discrete);
  return true;
}