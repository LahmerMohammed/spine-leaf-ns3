
#include "spine-leaf.h"
#include "topology-builder.h"
#include "mygym.h"
#include "globals.h"
using namespace ns3;
int
main(int argc , char* argv[])
{
  //LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  CommandLine cmd;
  cmd.AddValue ("openGymPort", "Port number for OpenGym env. Default: 5555", Globals::openGymPort);
  cmd.AddValue ("simSeed", "Seed for random generator. Default: 1", Globals::simSeed);
  cmd.AddValue ("simTime", "Simulation time in seconds. Default: 10s", Globals::simulationTime);
  cmd.AddValue ("stepTime", "Gym Env step time in seconds. Default: 0.1s", Globals::envStepTime);
  cmd.AddValue ("action space", "Extra simulation argument. Default: 4", Globals::action_space);
  cmd.AddValue ("serverCount", "Extra simulation argument. Default: 1", Globals::serverCount);
  cmd.AddValue ("leafCount", "Extra simulation argument. Default: 4", Globals::leafCount);
  cmd.AddValue ("spineCount", "Extra simulation argument. Default: 2", Globals::spineCount);
  std::cout<<"Configuration:"<<std::endl;
  std::cout<<"openGymPort="<<Globals::openGymPort<<std::endl;
  std::cout<<"simTime="<<Globals::simSeed<<std::endl;
  std::cout<<"stepTime="<<Globals::envStepTime<<std::endl;
  std::cout<<"serverCount="<<Globals::serverCount<<std::endl;
  std::cout<<"leafCount="<<Globals::leafCount<<std::endl;
  std::cout<<"spineCount="<<Globals::spineCount<<std::endl;

  Globals::action_space = Globals::spineCount;
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
  TopologyBuilder::poisson_traffic_generator(servers);

  //TopologyBuilder::generate_traffic(servers);
  TopologyBuilder::InitRLRouting (leaf);
  Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Drop", MakeCallback(&StateActionManager::TraceP2PDevQueueDrop));



  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> flowMonitor;
  flowMonitor = flowHelper.InstallAll();

  //TopologyBuilder::animation (spine, leaf, servers);

  /// OPENGYM



  // OpenGym Env

  //Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (Globals::openGymPort);
  //Ptr<DataCenterEnv> myGymEnv = CreateObject<DataCenterEnv> (Seconds(Globals::envStepTime));
  //myGymEnv->SetOpenGymInterface(openGymInterface);

  Simulator::Schedule(Seconds(1) , &TopologyBuilder::GetStats );
  StateActionManager::init ();
  Simulator::Stop(Seconds(Globals::simulationTime));
  Simulator::Run();

  TopologyBuilder::process_stats(flowMonitor, flowHelper, "/home/slahmer/PycharmProjects/pythonProject/file.csv");
  //flowMonitor->SerializeToXmlFile("flow2.xml", true, true);
  //openGymInterface->NotifySimulationEnd();
  Simulator::Destroy();

  return 0;
}