

#include <numeric>
#include <random>
#include "spine-leaf.h"

vec_stats_t CollectData::m_q_drops = std::vector(NO_DEVICE,std::vector<uint32_t>(NO_DEVICE));
vec_stats64_t CollectData::m_bandwidths = std::vector(NO_DEVICE,std::vector<uint64_t>(NO_DEVICE));

void
CollectData::GetData()
{
    for(size_t i = 0 ; i < p2pNetDevices.size() ; i++)
    {
        Ptr<PointToPointNetDevice> leaf_netDev = DynamicCast<PointToPointNetDevice>(p2pNetDevices[i].Get(0)) ;
        Ptr<PointToPointNetDevice> spine_netDev =  DynamicCast<PointToPointNetDevice>(p2pNetDevices[i].Get(1));

        Ptr<Node> leaf = leaf_netDev->GetNode();
        Ptr<Node> spine = spine_netDev->GetNode();

        //auto leaf_tf = leaf->GetObject<TrafficControlLayer>();
        //auto spine_tf = spine->GetObject<TrafficControlLayer>();
        auto leaf_queue = leaf_netDev->GetQueue();//leaf_tf->GetRootQueueDiscOnDevice(leaf_netDev);
        auto spine_queue = spine_netDev->GetQueue();//spine_tf->GetRootQueueDiscOnDevice(spine_netDev);

        //uint16_t i_leaf = (LEAF_COUNTER*SPINE_COUNTER-1) + SPINE_COUNTER*(leaf->GetId()-SPINE_COUNTER) + leaf_netDev->GetIfIndex() - (SERVER_COUNTER+1);
        uint16_t i_spine = LEAF_COUNTER*spine->GetId() + spine_netDev->GetIfIndex() - 1;
        uint16_t i_leaf = SPINE_COUNTER*(LEAF_COUNTER + leaf->GetId() - SPINE_COUNTER) + leaf_netDev->GetIfIndex() - (SERVER_COUNTER + 2);

        m_q_drops[i_leaf][i_spine] = leaf_queue->GetTotalDroppedPacketsBeforeEnqueue();
        m_q_drops[i_spine][i_leaf] = spine_queue->GetTotalDroppedPacketsBeforeEnqueue();
        auto tmp1 = leaf_netDev->time_slot();
        auto tmp2 = leaf_netDev->time_slot();

    }

    //return m_data;

}


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SpineAndLeaf");


std::tuple<NodeContainer,NodeContainer,NodeContainer>
BuildTopology(void)
{

    NS_LOG_INFO("Create Nodes.");

    NodeContainer spine;
    spine.Create(SPINE_COUNTER , Node::Type::SPINE);

    NodeContainer leaf;
    leaf.Create(LEAF_COUNTER , Node::Type::LEAF);

    NodeContainer servers;
    servers.Create(SERVER_COUNTER * LEAF_COUNTER , Node::Type::SERVER);


    NS_LOG_INFO("Installing Internet Stack");

    InternetStackHelper internetStack;
    internetStack.Install(leaf);
    internetStack.Install(spine);
    internetStack.Install(servers);


    NS_LOG_INFO("Installing P2P Channels");
    PointToPointHelper p2p;
    p2p.DisableFlowControl();
    p2p.SetQueue("ns3::DropTailQueue" , "MaxSize" , QueueSizeValue (QueueSize ("1500p")));


    NS_LOG_INFO("Creating P2P Connections between SERVER and LEAF");

    p2p.SetChannelAttribute("Delay" , TimeValue(SERVER_LEAF_DELAY));
    p2p.SetDeviceAttribute("DataRate" ,
                           DataRateValue(DataRate(SERVER_LEAF_DATA_RATE)));



    NetDeviceContainer netDeviceContainer;


    NS_LOG_INFO("Creating P2P Connections between Leaf and Servers");

    std::vector<NetDeviceContainer> server_leaf_netDevContainer;
    for(uint16_t i_leaf = 0 ; i_leaf < LEAF_COUNTER ; i_leaf++ )
    {

        for(uint16_t i_server = 0 ; i_server < SERVER_COUNTER ; i_server++)
        {
            netDeviceContainer = p2p.Install(servers.Get(i_server + i_leaf*SERVER_COUNTER) ,
                                             leaf.Get(i_leaf));

            server_leaf_netDevContainer.push_back(netDeviceContainer);

        }
    }



    p2p.SetChannelAttribute("Delay" , TimeValue(LEAF_SPINE_DELAY));
    p2p.SetDeviceAttribute("DataRate" ,
                           DataRateValue(DataRate(LEAF_SPINE_DATA_RATE)));

    NS_LOG_INFO("Creating P2P Connections between SPINE and LEAF");

    std::vector<NetDeviceContainer> leaf_spine_netDevContainer;
    for(uint16_t i_spine = 0 ; i_spine < SPINE_COUNTER ; i_spine++)
    {
        for(uint16_t i_leaf = 0 ; i_leaf < LEAF_COUNTER ; i_leaf++)
        {
            netDeviceContainer = p2p.Install(leaf.Get(i_leaf),
                                             spine.Get(i_spine));

            leaf_spine_netDevContainer.push_back(netDeviceContainer);
        }
    }

    p2pNetDevices = leaf_spine_netDevContainer;

    NS_LOG_INFO("Assiging ip addresses");
    Ipv4AddressHelper ipv4Address;
    ipv4Address.SetBase(BASE_NETWORK , BASE_NETWORK_MASK);
    Ipv4InterfaceContainer ipv4Interface;

    std::vector<Ipv4InterfaceContainer> server_leaf_interfaces(server_leaf_netDevContainer.size());
    for(uint16_t i = 0 ; i < server_leaf_netDevContainer.size() ; i++)
    {
        ipv4Interface = ipv4Address.Assign(server_leaf_netDevContainer[i]);
        server_leaf_interfaces[i] = ipv4Interface;

        ipv4Address.NewNetwork();

    }


    std::vector<Ipv4InterfaceContainer> leaf_spine_interfaces(leaf_spine_netDevContainer.size());

    for(uint16_t i = 0 ; i < leaf_spine_interfaces.size() ; i++ )
    {
        ipv4Interface = ipv4Address.Assign(leaf_spine_netDevContainer[i]);
        leaf_spine_interfaces[i] = ipv4Interface;

        ipv4Address.NewNetwork();
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();


    return std::make_tuple(spine,leaf,servers);
}


std::vector<uint32_t> zipf_streams(uint32_t total_quantity, uint32_t min_size=15*PACKET_SIZE){
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  x->SetAttribute ("Min", DoubleValue (0.0));
  x->SetAttribute ("Max", DoubleValue (1.0));

  std::vector<uint32_t> ret;
  double s = 0.0;
  while (s < total_quantity){
      double u = x->GetValue();
      double k = std::log10(4) / std::log10(5);
      double t = min_size / std::pow(u , (1 / k));
      s += t;
      //std::cout<<static_cast<uint32_t>(t)<<std::endl;
      ret.push_back (static_cast<uint32_t>(t));

  }

  //std::cout<<std::endl;

  ret[ret.size()-1] -= std::accumulate(ret.begin(), ret.end(), 0)-total_quantity;
  //std::cout<<"Sum: "<<std::accumulate(ret.begin(), ret.end(), 0)<<std::endl;
  return ret;
}


std::vector<std::vector<uint32_t>> generate_traffic_matrix(void){
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(1000000,100000000);
  std::vector<std::vector<uint32_t>> mat(SERVERS_COUNT, std::vector<uint32_t>(SERVERS_COUNT, 0));
  for (int i = 0; i < SERVERS_COUNT; ++i)
  {
    for (int j = 0; j < SERVERS_COUNT; ++j)
    {
      if(i == j)
        mat[i][j] = 0;
      else
        mat[i][j] = distribution(generator);
      std::cout<<mat[i][j]<<"  ";
    }
    std::cout<<std::endl;
  }
  return mat;
}

void generate_traffic(NodeContainer& servers){

  auto mat = generate_traffic_matrix();
  uint32_t port = 555;
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  x->SetAttribute ("Min", DoubleValue (0.1));
  x->SetAttribute ("Max", DoubleValue (SIMULATION_DURATION-3));

  for (int i = 0; i < SERVERS_COUNT; ++i)
  {
      for (int j = 0; j < SERVERS_COUNT; ++j)
      {
        uint32_t total_quantity = mat[i][j];
        if (i == j)
          continue;

        uint32_t tcp_quantity = static_cast<uint32_t>(0.2 * total_quantity);

        uint32_t on_off_noise = total_quantity - tcp_quantity;

        uint32_t pareto_rate = static_cast<uint32_t>(2 * (static_cast<double>(on_off_noise) / SIMULATION_DURATION));
        //double pareto_burst_time = 0.5;
        //double pareto_idle_time = 0.5;

        Ptr<Ipv4> ipv4 = servers.Get (i)->GetObject<Ipv4>();
        Ipv4Address ipv4Address = ipv4->GetAddress(1,0).GetLocal();

        OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(ipv4Address, port++)));
        onoff.SetConstantRate(DataRate(pareto_rate), PACKET_SIZE);
        onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=0.5]"));
        onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.5]"));
        onoff.Install (servers.Get (j));

        std::vector<uint32_t > flowz = zipf_streams(tcp_quantity);
        for (const auto &item : flowz){

            double flow_run = x->GetValue();

            UdpEchoServerHelper  udpServer(port);
            ApplicationContainer serversApps = udpServer.Install(servers.Get(i));



            UdpEchoClientHelper udpEchoClient(ipv4Address , port++);
            udpEchoClient.SetAttribute("PacketSize" , UintegerValue(PACKET_SIZE));
            udpEchoClient.SetAttribute("MaxPackets" , UintegerValue(static_cast<uint32_t >(item/PACKET_SIZE)));
            udpEchoClient.SetAttribute("Interval" , TimeValue(INTERVAL));


            auto clientApps = udpEchoClient.Install(servers.Get (j));

            clientApps.Start(Seconds(flow_run+0.2));
            clientApps.Stop(Seconds(SIMULATION_DURATION));
            serversApps.Start(Seconds(flow_run));
            serversApps.Stop(Seconds(SIMULATION_DURATION));
        }

      }
  }
}

void
GenerateTraffic(NodeContainer& clientNodes , NodeContainer& serverNodes)
{

    UdpEchoServerHelper  udpServer(UDP_SERVER_PORT);
    ApplicationContainer serversApps = udpServer.Install(serverNodes);

    Ptr<Node> randomNode = serverNodes.Get(0);
    Ptr<Ipv4> ipv4 = randomNode->GetObject<Ipv4>();
    Ipv4Address ipv4Address = ipv4->GetAddress(1,0).GetLocal();

    UdpEchoClientHelper udpEchoClient(ipv4Address , UDP_SERVER_PORT);
    udpEchoClient.SetAttribute("PacketSize" , UintegerValue(PACKET_SIZE));
    udpEchoClient.SetAttribute("MaxPackets" , UintegerValue(MAX_PACKETS));
    udpEchoClient.SetAttribute("Interval" , TimeValue(INTERVAL));


    auto clientApps = udpEchoClient.Install(clientNodes);

    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(100.0));
    serversApps.Start(Seconds(1.0));
    serversApps.Stop(Seconds(100.0));

}



void
GetStats()
{

    /// stats of node-2-interface-1 queue (leaf) connected to p2p channel with the node-0 (spine)
    std::cout << CollectData::m_q_drops[7][0]<<std::endl;
    Simulator::Schedule(Seconds(1) , &GetStats);
}

void process_stats(const Ptr<FlowMonitor>& flow_monitor, FlowMonitorHelper& flowhelp){
  std::fstream fout;

  // opens an existing csv file or creates a new file.
  fout.open("/home/slahmer/PycharmProjects/pythonProject/file.csv", std::ios::out | std::ios::app);
  fout<<"fid,srcaddr,srcport,destaddr,destport,first_tx,first_rx,last_tx,last_rx,delay_sum,jitter_sum,last_delay,tx_bytes,rx_bytes,tx_packets,rx_packets,lost_packets,times_forwarded,pdrop0,pdrop1,pdrop2,pdrop3,bdrop0,bdrop1,bdrop2,bdrop3\n";

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowhelp.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = flow_monitor->GetFlowStats ();
  //uint32_t fid = 0;

  for (FlowMonitor::FlowStatsContainerCI flowI = stats.begin ();
       flowI != stats.end (); flowI++)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (flowI->first);


      #define ATTRIB(name) << "," << flowI->second.name
      #define ATTRIB_TIME(name) <<","<<flowI->second.name.As (Time::NS)
      fout << flowI->first<<","<<t.sourceAddress<<","<<t.sourcePort<<","<<t.destinationAddress<<","<<t.destinationPort
      ATTRIB_TIME (timeFirstTxPacket)
      ATTRIB_TIME (timeFirstRxPacket)
      ATTRIB_TIME (timeLastTxPacket)
      ATTRIB_TIME (timeLastRxPacket)
      ATTRIB_TIME (delaySum)
      ATTRIB_TIME (jitterSum)
      ATTRIB_TIME (lastDelay)
      ATTRIB (txBytes)
      ATTRIB (rxBytes)
      ATTRIB (txPackets)
      ATTRIB (rxPackets)
      ATTRIB (lostPackets)
      ATTRIB (timesForwarded);
      if (flowI->second.packetsDropped.size() == 0){
          fout <<",0,0,0,0";
      }
      else{
          fout<<","<<flowI->second.packetsDropped[0]<<","<< flowI->second.packetsDropped[1] <<","<< flowI->second.packetsDropped[2]<<","<< flowI->second.packetsDropped[3];
      }

      if (flowI->second.bytesDropped.size() == 0){
          fout <<",0,0,0,0\n";
        }
      else{
          fout<<","<<flowI->second.bytesDropped[0]<<","<< flowI->second.bytesDropped[1] <<","<< flowI->second.bytesDropped[2]<<","<< flowI->second.bytesDropped[3]<<"\n";
        }
    }


}


int
main(int argc , char* argv[])
{
    NodeContainer spine,leaf,servers;
    std::tie(spine,leaf,servers) = BuildTopology();


    NodeContainer udpClients , udpServers;
    for(int i = 0 ; i < SERVER_COUNTER*LEAF_COUNTER ; i++)
    {
        if( i < SERVER_COUNTER)
            udpClients.Add(servers.Get(i));
        else
            udpServers.Add(servers.Get(i));
    }

    generate_traffic(servers);


    Simulator::Schedule(Seconds(1) , &GetStats );
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
    for(uint16_t i = 0 ; i < spine.GetN() ; i++)
    {
        Ptr<ConstantPositionMobilityModel> n1 = spine.Get(i)->GetObject<ConstantPositionMobilityModel>();
        n1->SetPosition(Vector(x, y, 0));
        x+=20.0;
        anim.UpdateNodeDescription(nodes_counter, "router-layer2-" + std::to_string(i));
        nodes_counter++;

    }


    x = 7.0;
    y = 22.0;
    for(uint16_t i = 0 ; i < leaf.GetN() ; i++)
    {
        Ptr<ConstantPositionMobilityModel> n1 = leaf.Get(i)->GetObject<ConstantPositionMobilityModel>();
        n1->SetPosition(Vector(x, y, 0));
        x+=15.0;
        anim.UpdateNodeDescription(nodes_counter, "router-layer1-" + std::to_string(i));
        nodes_counter++;
    }

    x = 7.0;
    y = 36.0;
    for(uint16_t i = 0 ; i < servers.GetN() ; i++)
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
    process_stats(flowMonitor, flowHelper);
    flowMonitor->SerializeToXmlFile("flow2.xml", true, true);
    Simulator::Destroy();

    return 0;
}