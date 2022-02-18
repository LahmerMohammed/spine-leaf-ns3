


#include "spine-leaf.h"

VecVecQueueDisc CollectData::m_data = std::vector(NO_DEVICE,VecQueueDisc(NO_DEVICE));

VecVecQueueDisc
CollectData::GetData()
{
    for(size_t i = 0 ; i < p2pNetDevices.size() ; i++)
    {
        Ptr<PointToPointNetDevice> leaf_netDev = DynamicCast<PointToPointNetDevice>(p2pNetDevices[i].Get(0)) ;
        Ptr<PointToPointNetDevice> spine_netDev =  DynamicCast<PointToPointNetDevice>(p2pNetDevices[i].Get(1));

        Ptr<Node> leaf = leaf_netDev->GetNode();
        Ptr<Node> spine = spine_netDev->GetNode();

        auto leaf_tf = leaf->GetObject<TrafficControlLayer>();
        auto spine_tf = spine->GetObject<TrafficControlLayer>();

        auto leaf_queue = leaf_tf->GetRootQueueDiscOnDevice(leaf_netDev);
        auto spine_queue = spine_tf->GetRootQueueDiscOnDevice(spine_netDev);

        uint16_t i_leaf = leaf->GetId() + leaf_netDev->GetIfIndex() - 1;
        uint16_t i_spine = spine->GetId() + spine_netDev->GetIfIndex() - 1;

        m_data[i_leaf][i_spine] = leaf_queue->GetStats();
        m_data[i_spine][i_leaf] = spine_queue->GetStats();
    }

    return m_data;

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

    p2p.SetQueue("ns3::DropTailQueue" , "MaxSize" , QueueSizeValue (QueueSize ("3p")));


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


/**
 * \brief generate traffic between two sets (clients and servers)
 * \param two NodeContainer refrences (clients , servers)
 */
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

    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(10.0));
    serversApps.Start(Seconds(2.0));
    serversApps.Stop(Seconds(10.0));

}



void
GetStats()
{
    /// stats of node-2 queue (leaf) connected to a channel with the node-0 (spine)
    std::cout << CollectData::GetData()[2][0] << std::endl;

    Simulator::Schedule(Seconds(1) , &GetStats);

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

    GenerateTraffic(udpClients,udpServers);


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



    Simulator::Stop(Seconds(21.0));

    Simulator::Run();

    flowMonitor->SerializeToXmlFile("flow2" , true , true);

    Simulator::Destroy();

    return 0;
}