
#include "spine-leaf.h"
#include "topology-builder.h"
#include "mygym.h"
#include "globals.h"
using namespace ns3;
int
main(int argc , char* argv[])
{

  CommandLine cmd;
  cmd.AddValue ("openGymPort", "Port number for OpenGym env. Default: 5555", Globals::openGymPort);
  cmd.AddValue ("simSeed", "Seed for random generator. Default: 1", Globals::simSeed);
  cmd.AddValue ("simTime", "Simulation time in seconds. Default: 10s", Globals::simulationTime);
  cmd.AddValue ("stepTime", "Gym Env step time in seconds. Default: 0.1s", Globals::envStepTime);

  cmd.AddValue ("serverCount", "Extra simulation argument. Default: 0", Globals::serverCount);
  cmd.AddValue ("leafCount", "Extra simulation argument. Default: 0", Globals::leafCount);
  cmd.AddValue ("spineCount", "Extra simulation argument. Default: 0", Globals::spineCount);

  cmd.Parse (argc, argv);

  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (Globals::simSeed);


  NodeContainer spine,leaf,servers;
  std::tie(spine,leaf,servers) = TopologyBuilder::BuildTopology(Globals::spineCount, Globals::leafCount, Globals::serverCount);


  NodeContainer udpClients , udpServers;
  for(uint32_t i = 0 ; i < Globals::serverCount*Globals::leafCount ; i++)
    {
      if( i < Globals::serverCount)
        udpClients.Add(servers.Get(i));
      else
        udpServers.Add(servers.Get(i));
    }

  TopologyBuilder::generate_traffic(servers);
  TopologyBuilder::InitRLRouting (leaf);
  //Simulator::Schedule(Seconds(1) , &TopologyBuilder::GetStats );
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Enqueue", MakeCallback(&Func));
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Drop", MakeCallback(&Func));
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Enqueue", MakeCallback(&Func));
  //TopologyBuilder::animation (spine, leaf, servers);

  /*

  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> flowMonitor;
  flowMonitor = flowHelper.InstallAll();
  */

  /// OPENGYM


    /*
  // OpenGym Env
  Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (Globals::openGymPort);
  Ptr<DataCenterEnv> myGymEnv = CreateObject<DataCenterEnv> (Seconds(Globals::envStepTime));
  myGymEnv->SetOpenGymInterface(openGymInterface);
    */

  Simulator::Stop(Seconds(Globals::simulationTime));
  Simulator::Run();
  //TopologyBuilder::process_stats(flowMonitor, flowHelper);
  //flowMonitor->SerializeToXmlFile("flow2.xml", true, true);
  //openGymInterface->NotifySimulationEnd();
  Simulator::Destroy();

  return 0;
}