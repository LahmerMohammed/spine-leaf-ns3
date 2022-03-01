
#include "spine-leaf.h"
#include "topology-builder.h"
using namespace ns3;
int
main(int argc , char* argv[])
{
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


  Simulator::Schedule(Seconds(1) , &TopologyBuilder::GetStats );
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Enqueue", MakeCallback(&Func));
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Drop", MakeCallback(&Func));
  //Config::Connect("/NodeList/*/DeviceList/*/TxQueue/Enqueue", MakeCallback(&Func));

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install(leaf);
  mobility.Install(spine);
  mobility.Install(servers);


  AnimationInterface anim("SpinAndLeafAnim.xml");


  uint16_t nodes_counter = 0;
  double x = 15.0 , y = 8.0;
  for(uint32_t i = 0 ; i < spine.GetN() ; i++)
    {
      Ptr<ConstantPositionMobilityModel> n1 = spine.Get(i)->GetObject<ConstantPositionMobilityModel>();
      n1->SetPosition(Vector(x, y, 0));
      x+=20.0;
      anim.UpdateNodeDescription(nodes_counter, "router-layer2-" + std::to_string(i));
      nodes_counter++;

    }


  x = 7.0;
  y = 22.0;
  for(uint32_t i = 0 ; i < leaf.GetN() ; i++)
    {
      Ptr<ConstantPositionMobilityModel> n1 = leaf.Get(i)->GetObject<ConstantPositionMobilityModel>();
      n1->SetPosition(Vector(x, y, 0));
      x+=15.0;
      anim.UpdateNodeDescription(nodes_counter, "router-layer1-" + std::to_string(i));
      nodes_counter++;
    }

  x = 7.0;
  y = 36.0;
  for(uint32_t i = 0 ; i < servers.GetN() ; i++)
    {
      Ptr<ConstantPositionMobilityModel> n1 = servers.Get(i)->GetObject<ConstantPositionMobilityModel>();
      n1->SetPosition(Vector(x, y, 0));
      x+=15.0;
      anim.UpdateNodeDescription(nodes_counter, "server-" + std::to_string(i));
      nodes_counter++;
    }

  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> flowMonitor;
  flowMonitor = flowHelper.InstallAll();


  Simulator::Stop(Seconds(SIMULATION_DURATION+2.0));
  Simulator::Run();
  TopologyBuilder::process_stats(flowMonitor, flowHelper);
  flowMonitor->SerializeToXmlFile("flow2.xml", true, true);
  Simulator::Destroy();

  return 0;
}