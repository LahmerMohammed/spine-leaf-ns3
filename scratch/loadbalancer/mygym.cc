/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Technische Universität Berlin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Piotr Gawlowicz <gawlowicz@tkn.tu-berlin.de>
 */

#include "mygym.h"
#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/node-list.h"
#include "ns3/log.h"
#include <sstream>
#include <iostream>
#include "globals.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DataCenterEnv");

//NS_OBJECT_ENSURE_REGISTERED (MyGymEnv);

DataCenterEnv::DataCenterEnv ()
{
  NS_LOG_FUNCTION (this);
  m_interval = Seconds(0.1);

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
  float high = 10.0;
  std::vector<uint32_t> shape = {Globals::spineCount*Globals::leafCount,};
  std::string dtype = TypeNameGet<float> ();

  //Ptr<OpenGymDiscreteSpace> discrete = CreateObject<OpenGymDiscreteSpace> (nodeNum);
  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  Ptr<OpenGymDictSpace> space = CreateObject<OpenGymDictSpace> ();
  space->Add("observation", box);
  //space->Add("myValue", discrete);

  NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
  return space;
}

/*
Define action space
*/
Ptr<OpenGymSpace>
DataCenterEnv::GetActionSpace()
{
  uint32_t nodeNum = 5;
  float low = 0.0;
  float high = 1.0;
  std::vector<uint32_t> shape = {Globals::spineCount*Globals::leafCount,};
  std::string dtype = TypeNameGet<float> ();

  Ptr<OpenGymDiscreteSpace> discrete = CreateObject<OpenGymDiscreteSpace> (nodeNum);
  Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace> (low, high, shape, dtype);

  Ptr<OpenGymDictSpace> space = CreateObject<OpenGymDictSpace> ();
  space->Add("actions", box);
  //space->Add("myActionValue", discrete);

  NS_LOG_UNCOND ("MyGetActionSpace: " << space);
  return space;
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
  uint32_t nodeNum = 5;
  uint32_t low = 0.0;
  uint32_t high = 10.0;
  Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();

  std::vector<uint32_t> shape = {Globals::spineCount*Globals::leafCount,};
  Ptr<OpenGymBoxContainer<float> > box = CreateObject<OpenGymBoxContainer<float> >(shape);

  // generate random data
  for (uint32_t i = 0; i<nodeNum; i++){
    float value = rngInt->GetValue(low, high);
    box->AddValue(value);
  }
  /*
  Ptr<OpenGymDiscreteContainer> discrete = CreateObject<OpenGymDiscreteContainer>(nodeNum);
  uint32_t value = rngInt->GetInteger(low, high);
  discrete->SetValue(value);
  */
  Ptr<OpenGymDictContainer> data = CreateObject<OpenGymDictContainer> ();
  data->Add("observation",box);
  //data->Add("myValue",discrete);

  // Print data from tuple
  Ptr<OpenGymBoxContainer<float> > mbox = DynamicCast<OpenGymBoxContainer<float> >(data->Get("observation"));
  //Ptr<OpenGymDiscreteContainer> mdiscrete = DynamicCast<OpenGymDiscreteContainer>(data->Get("myValue"));
  NS_LOG_UNCOND ("MyGetObservation: " << data);
  NS_LOG_UNCOND ("---" << mbox);
  //NS_LOG_UNCOND ("---" << mdiscrete);

  return data;
}

/*
Define reward function
*/
float
DataCenterEnv::GetReward()
{
  static float reward = 0.0;
  reward += 1;
  return reward;
}

/*
Define extra info. Optional
*/
std::string
DataCenterEnv::GetExtraInfo()
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
DataCenterEnv::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
  Ptr<OpenGymDictContainer> dict = DynamicCast<OpenGymDictContainer>(action);
  Ptr<OpenGymBoxContainer<float> > box = DynamicCast<OpenGymBoxContainer<float> >(dict->Get("actions"));
  //Ptr<OpenGymDiscreteContainer> discrete = DynamicCast<OpenGymDiscreteContainer>(dict->Get("myActionValue"));

  NS_LOG_UNCOND ("MyExecuteActions: " << action);
  NS_LOG_UNCOND ("---" << box);
  //NS_LOG_UNCOND ("---" << discrete);
  return true;
}

} // ns3 namespace