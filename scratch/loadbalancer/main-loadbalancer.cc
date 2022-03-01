
#include "spine-leaf.h"
#include "topology-builder.h"
#include "mygym.h"
using namespace ns3;
int
main(int argc , char* argv[])
{
  uint32_t simSeed = 1;
  double simulationTime = 10; //seconds
  double envStepTime = 1; //seconds, ns3gym env step time interval
  uint32_t openGymPort = 5555;
  uint32_t testArg = 0;

  CommandLine cmd;
  // required parameters for OpenGym interface
  cmd.AddValue ("openGymPort", "Port number for OpenGym env. Default: 5555", openGymPort);
  cmd.AddValue ("simSeed", "Seed for random generator. Default: 1", simSeed);
  cmd.AddValue ("simTime", "Simulation time in seconds. Default: 10s", simulationTime);
  cmd.AddValue ("stepTime", "Gym Env step time in seconds. Default: 0.1s", envStepTime);
  cmd.AddValue ("testArg", "Extra simulation argument. Default: 0", testArg);
  cmd.Parse (argc, argv);

  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (simSeed);


  NodeContainer spine,leaf,servers;
  std::tie(spine,leaf,servers) = TopologyBuilder::BuildTopology();


  NodeContainer udpClients , udpServers;
  for(int i = 0 ; i < SERVER_COUNTER*LEAF_COUNTER ; i++)
    {
      if( i < SERVER_COUNTER)
        udpClients.Add(servers.Get(i));
      else
        udpServers.Add(servers.Get(i));
    }

  TopologyBuilder::generate_traffic(servers);


  //Simulator::Schedule(Seconds(1) , &TopologyBuilder::GetStats );
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Enqueue", MakeCallback(&Func));
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Drop", MakeCallback(&Func));
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Enqueue", MakeCallback(&Func));

  /*
  TopologyBuilder::animation (spine, leaf, servers);

  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> flowMonitor;
  flowMonitor = flowHelper.InstallAll();
  */

  /// OPENGYM



  // OpenGym Env
  Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (openGymPort);
  Ptr<DataCenterEnv> myGymEnv = CreateObject<DataCenterEnv> (Seconds(envStepTime));
  myGymEnv->SetOpenGymInterface(openGymInterface);


  Simulator::Stop(Seconds(simulationTime));
  Simulator::Run();
  //TopologyBuilder::process_stats(flowMonitor, flowHelper);
  //flowMonitor->SerializeToXmlFile("flow2.xml", true, true);
  openGymInterface->NotifySimulationEnd();
  Simulator::Destroy();

  return 0;
}