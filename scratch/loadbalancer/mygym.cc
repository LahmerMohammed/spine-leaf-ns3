#include "mygym.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/node-list.h"
#include "ns3/log.h"
#include <iostream>
#include "globals.h"
#include "spine-leaf.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DataCenterEnv");

//NS_OBJECT_ENSURE_REGISTERED (MyGymEnv);

DataCenterEnv::DataCenterEnv ()
{
  NS_LOG_FUNCTION (this);
  m_interval = Seconds(Globals::envStepTime);

  Simulator::Schedule (Seconds(0.0), &DataCenterEnv::ScheduleNextStateRead, this);
}

DataCenterEnv::DataCenterEnv (Time stepTime)
{
  NS_LOG_FUNCTION (this);
  m_interval = stepTime;

  Simulator::Schedule (Seconds(0.0), &DataCenterEnv::ScheduleNextStateRead, this);
}

void
DataCenterEnv::ScheduleNextStateRead ()
{
  NS_LOG_FUNCTION (this);
  Simulator::Schedule (m_interval, &DataCenterEnv::ScheduleNextStateRead, this);
  Notify();
}

DataCenterEnv::~DataCenterEnv ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
DataCenterEnv::GetTypeId (void)
{
  static TypeId tid = TypeId ("DataCenterEnv")
    .SetParent<OpenGymEnv> ()
    .SetGroupName ("OpenGym")
    .AddConstructor<DataCenterEnv> ()
  ;
  return tid;
}

void
DataCenterEnv::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

/*
Define observation space
*/
Ptr<OpenGymSpace>
DataCenterEnv::GetObservationSpace()
{
  //uint32_t nodeNum = 5;
  float low = 0.0;
  float high = 10000000.0;
  uint32_t size = Globals::spineCount*Globals::leafCount*7;
  std::vector<uint32_t> shape = {size,};
  std::string dtype = TypeNameGet<float> ();

  //Ptr<OpenGymDiscreteSpace> discrete = CreateObject<OpenGymDiscreteSpace> (nodeNum);
  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);


  NS_LOG_UNCOND ("MyGetObservationSpace: " << box);
  return box;
}

/*
Define action space
*/
Ptr<OpenGymSpace>
DataCenterEnv::GetActionSpace()
{
  float low = 0.0;
  float high = 100.0;
  uint32_t size = Globals::spineCount*Globals::leafCount;
  std::vector<uint32_t> shape = {size,};
  std::string dtype = TypeNameGet<float> ();

  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);


  NS_LOG_UNCOND ("MyGetActionSpace: " << box);
  return box;
}

/*
Define game over condition
*/
bool
DataCenterEnv::GetGameOver()
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
Ptr<OpenGymDataContainer>
DataCenterEnv::GetObservation()
{

  Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();
  uint32_t size = Globals::spineCount*Globals::leafCount*7;
  std::vector<uint32_t> shape = {size,};
  Ptr<OpenGymBoxContainer<float> > box = CreateObject<OpenGymBoxContainer<float> >(shape);
  StateActionManager::GetData();
  // generate random data
  std::vector<float> data;
  auto drops_spine_leaf = StateActionManager::m_q_drops_spines;
  for (const auto &item : drops_spine_leaf)
    for (const auto &val : item){
        box->AddValue(val);
      }
  auto drops_leaf_spine = StateActionManager::m_q_drops_leaves;
  for (const auto &item : drops_leaf_spine)
    for (const auto &val : item){
        box->AddValue (val);
      }

  auto qsize_spine_leaf = StateActionManager::m_q_size_spines;
  for (const auto &item : qsize_spine_leaf)
    for (const auto &val : item){
        box->AddValue(val);
      }
  auto qsize_leaf_spine = StateActionManager::m_q_size_leaves;
  for (const auto &item : qsize_leaf_spine)
    for (const auto &val : item){
        box->AddValue (val);
      }

  auto bandiwth_spine_leaf = StateActionManager::m_bandwidths_spines;
  for (const auto &item : bandiwth_spine_leaf)
    for (const auto &val : item){
        box->AddValue (val/8000);
      }

  auto bandiwth_leaf_spine = StateActionManager::m_bandwidths_leaves;
  for (const auto &item : bandiwth_leaf_spine)
    for (const auto &val : item){
        box->AddValue (val/8000);
      }


  auto path_links_drops = StateActionManager::m_path_drops;
  //uint64_t z = 0;
  for (auto &item : path_links_drops)
    for (auto &val : item){
        box->AddValue (val);
        val = 0;
        //z++;
      }
  //std::cout<<"Z="<<z<<std::endl;

  //NS_LOG_UNCOND ("MyGetObservation: " << box);

  return box;
}


float
DataCenterEnv::GetReward()
{
  static float reward = 0.0;
  reward += 1;
  return reward;
}


std::string
DataCenterEnv::GetExtraInfo()
{
  std::string myInfo = "testInfo";
  myInfo += "|123";
  //NS_LOG_UNCOND("MyGetExtraInfo: " << myInfo);
  return myInfo;
}

bool
DataCenterEnv::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
  Ptr<OpenGymBoxContainer<float> > box = DynamicCast<OpenGymBoxContainer<float> >(action);//dict->Get("actions"));
  NS_ASSERT (box != nullptr);
  StateActionManager::ApplyNewAction (box->GetData());
  //NS_LOG_UNCOND ("MyExecuteActions: " << action);
  //NS_LOG_UNCOND ("---" << box);
  return true;
}

} // ns3 namespace