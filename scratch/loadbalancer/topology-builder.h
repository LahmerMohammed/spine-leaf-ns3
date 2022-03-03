//
// Created by slahmer on 3/1/22.
//

#ifndef NS3_TOPOLOGY_BUILDER_H
#define NS3_TOPOLOGY_BUILDER_H
#include <random>
#include "spine-leaf.h"
#include "globals.h"
#include "ns3/internet-module.h"
using namespace ns3;

class TopologyBuilder{

public:
  inline static std::vector<Ptr<ns3::Ipv4RlRouting>> leaf_rl_routers = {};
  inline static void InitRLRouting(const NodeContainer& leaf){
    std::cout<<"InitRouting"<<::std::endl;
    for(uint32_t curr_index = 0; curr_index < leaf.GetN() ; curr_index++){

        auto ipv4 = leaf.Get (curr_index)->GetObject<Ipv4> ();
        auto tmp = ipv4->GetRoutingProtocol();
        auto tmp2 = DynamicCast<Ipv4ListRouting>(tmp);
        int16_t priority;
        auto rl_routing = tmp2->GetRoutingProtocol (0, priority);
        auto global_routing = tmp2->GetRoutingProtocol (2, priority);
        auto tmp22 = DynamicCast<ns3::Ipv4GlobalRouting> (global_routing);
        auto tmp21 = DynamicCast<ns3::Ipv4RlRouting> (rl_routing);
        if (tmp22 and tmp21)
          std::cout<<"Init RL Router: <<"<<curr_index+1<<::std::endl;
        else{
            std::cout<<"Casting problem <<"<<curr_index+1<<::std::endl;
            exit (1);
          }
        leaf_rl_routers.push_back (tmp21);
        tmp21->init_routes(ipv4, tmp22->GetHostRoutes(), tmp22->GetNetworkRoutes(), Globals::action_space);
        // TODO change it later
        std::vector<float> initial_probs = {0.5, 0.5};// here we are only using two nodes, this is to be changed later, TODO use number of spines
        tmp21->SetDistribution (initial_probs);

      }


  }

  inline static std::tuple<NodeContainer,NodeContainer,NodeContainer>
  BuildTopology(uint32_t spine_count, uint32_t leaf_count, uint32_t servers_count)
  {

    //NS_LOG_INFO("Create Nodes.");

    NodeContainer spine;
    spine.Create(spine_count , Node::Type::SPINE);

    NodeContainer leaf;
    leaf.Create(leaf_count , Node::Type::LEAF);

    NodeContainer servers;
    servers.Create(servers_count * leaf_count , Node::Type::SERVER);

    //Ipv4DrbHelper drb;


    //NS_LOG_INFO("Installing Internet Stack");

    InternetStackHelper internetStack;
    InternetStackHelper::rl_enable = true;
    internetStack.Install(leaf);
    InternetStackHelper::rl_enable = false;
    internetStack.Install(spine);
    internetStack.Install(servers);


    //NS_LOG_INFO("Installing P2P Channels");
    PointToPointHelper p2p;
    p2p.DisableFlowControl();
    p2p.SetQueue("ns3::DropTailQueue" , "MaxSize" , QueueSizeValue (QueueSize ("1500p")));


    //NS_LOG_INFO("Creating P2P Connections between SERVER and LEAF");

    p2p.SetChannelAttribute("Delay" , TimeValue(SERVER_LEAF_DELAY));
    p2p.SetDeviceAttribute("DataRate" ,
                            DataRateValue(DataRate(SERVER_LEAF_DATA_RATE)));



    NetDeviceContainer netDeviceContainer;


    //NS_LOG_INFO("Creating P2P Connections between Leaf and Servers");

    std::vector<NetDeviceContainer> server_leaf_netDevContainer;
    for(uint32_t i_leaf = 0 ; i_leaf < leaf_count ; i_leaf++ )
      {

        for(uint32_t i_server = 0 ; i_server < servers_count ; i_server++)
          {
            netDeviceContainer = p2p.Install(servers.Get(i_server + i_leaf*servers_count) ,
                                              leaf.Get(i_leaf));

            server_leaf_netDevContainer.push_back(netDeviceContainer);

          }
      }



    p2p.SetChannelAttribute("Delay" , TimeValue(LEAF_SPINE_DELAY));
    p2p.SetDeviceAttribute("DataRate" ,
                            DataRateValue(DataRate(LEAF_SPINE_DATA_RATE)));

    //NS_LOG_INFO("Creating P2P Connections between SPINE and LEAF");

    std::vector<NetDeviceContainer> leaf_spine_netDevContainer;
    std::vector<std::vector<Ptr<PointToPointNetDevice>>> spines_leaves_netdevs(spine_count, std::vector<Ptr<PointToPointNetDevice>>(leaf_count, nullptr));
    std::vector<std::vector<Ptr<PointToPointNetDevice>>> leaves_spines_netdevs(leaf_count, std::vector<Ptr<PointToPointNetDevice>>(spine_count, nullptr));

    for(uint32_t i_spine = 0 ; i_spine < spine_count ; i_spine++)
      {

        for(uint32_t i_leaf = 0 ; i_leaf < leaf_count ; i_leaf++)
          {
            netDeviceContainer = p2p.Install(leaf.Get(i_leaf),
                                              spine.Get(i_spine));

            Ptr<PointToPointNetDevice> leaf_netDev = DynamicCast<PointToPointNetDevice>(netDeviceContainer.Get(0)) ;
            Ptr<PointToPointNetDevice> spine_netDev =  DynamicCast<PointToPointNetDevice>(netDeviceContainer.Get(1));
            spines_leaves_netdevs[i_spine][i_leaf] = spine_netDev;
            leaves_spines_netdevs[i_leaf][i_spine] = leaf_netDev;


            leaf_spine_netDevContainer.push_back(netDeviceContainer);
          }
      }

    //p2pNetDevices = leaf_spine_netDevContainer;
    p2pSpinesLeavesNetdevs = spines_leaves_netdevs;
    p2pLeavesSpinesNetdevs = leaves_spines_netdevs;
    //NS_LOG_INFO("Assiging ip addresses");
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

  inline static void
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
  inline static std::vector<std::vector<uint64_t>> generate_traffic_matrix(uint32_t servers_count){
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> distribution(10000000,100000000);
    std::vector<std::vector<uint64_t>> mat(servers_count, std::vector<uint64_t>(servers_count, 0));
    for (uint32_t i = 0; i < servers_count; ++i)
      {
        for (uint32_t j = 0; j < servers_count; ++j)
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

  inline static void generate_traffic(NodeContainer& servers){

    auto mat = generate_traffic_matrix(servers.GetN());
    uint32_t port = 555;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.1,Globals::simulationTime-1.0);


    for (uint32_t i = 0; i < servers.GetN(); ++i)
      {
        for (uint32_t j = 0; j < servers.GetN(); ++j)
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

                double flow_run = distribution(generator);

                UdpEchoServerHelper  udpServer(port);
                ApplicationContainer serversApps = udpServer.Install(servers.Get(i));



                UdpEchoClientHelper udpEchoClient(ipv4Address , port++);
                udpEchoClient.SetAttribute("PacketSize" , UintegerValue(PACKET_SIZE));
                udpEchoClient.SetAttribute("MaxPackets" , UintegerValue(static_cast<uint32_t >(item/PACKET_SIZE)));
                udpEchoClient.SetAttribute("Interval" , TimeValue(INTERVAL));


                auto clientApps = udpEchoClient.Install(servers.Get (j));

                clientApps.Start(Seconds(flow_run));
                clientApps.Stop(Seconds(SIMULATION_DURATION));
                serversApps.Start(Seconds(flow_run));
                serversApps.Stop(Seconds(SIMULATION_DURATION));
              }

          }
      }
  }

  inline static std::vector<uint32_t> zipf_streams(uint32_t total_quantity, uint32_t min_size=15*PACKET_SIZE){
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);

    std::vector<uint32_t> ret;
    double s = 0.0;
    while (s < total_quantity){
        double u = distribution(generator);
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

  inline static void
  GetStats()
  {
    StateActionManager::GetData();
    /// stats of node-2-interface-1 queue (leaf) connected to p2p channel with the node-0 (spine)
    //leaf1-spine1
    //std::cout << CollectData::m_q_drops[7][0]<<std::endl;
    std::cout << StateActionManager::m_q_drops_leaves[0][0]<<" "<<StateActionManager::m_bandwidths_leaves[0][0]<<" "<<Simulator::Now().GetMilliSeconds()<<std::endl;
    Simulator::Schedule(Seconds(1) , &GetStats);
  }

  inline static void process_stats(const Ptr<FlowMonitor>& flow_monitor, FlowMonitorHelper& flowhelp){
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

  inline static void animation(NodeContainer& spine, NodeContainer& leaf, NodeContainer& servers){

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

  }

};


#endif //NS3_TOPOLOGY_BUILDER_H
