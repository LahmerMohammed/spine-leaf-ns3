#ifndef SPINE_LEAF_H
#define SPINE_LEAF_H

#include "ns3/traffic-control-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/v4ping.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/v4ping-helper.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/packet-socket.h"
#include "vector"
#include "tuple"
#include "ns3/aarl-ipv4-routing.h"
#include "globals.h"
#define NANO_TO_SEC(a) a / 1000000000.0
#define MICRO_TO_SEC(a) a / 1000000.0
#define MILI_TO_SEC(a) a / 1000.0
#define SIMULATION_DURATION 30.0
#define SERVER_LEAF_DELAY MilliSeconds(1)
#define SERVER_LEAF_DATA_RATE "100Mbps"
#define LEAF_SPINE_DELAY MilliSeconds(1)
#define LEAF_SPINE_DATA_RATE "100Mbps"
#define PACKET_SIZE 1000
#define INTERVAL MilliSeconds(1)
#define MAX_PACKETS 3000
#define UDP_SERVER_PORT 3000

#define BASE_NETWORK "192.168.0.0"
#define BASE_NETWORK_MASK "255.255.255.0"





typedef std::vector<std::vector<uint32_t>> vec_stats_t;
typedef std::vector<std::vector<long double>> vec_statsd64_t;
typedef std::vector<std::vector<uint64_t>> vec_statsu64_t;


using namespace ns3;

class StateActionManager {

public:
    static void GetData();
    static void ApplyNewAction(std::vector<float>);

    inline static void TraceP2PDevQueueDrop(std::string context, Ptr<const Packet> droppedPacket){
        MyTag tagCopy;
        droppedPacket->PeekPacketTag (tagCopy);
        //droppedPacket->PrintPacketTags (std::cout);
        if(tagCopy.GetLeafId() != 99999){
            Ptr<Packet> copy = droppedPacket->Copy();

            // Headers must be removed in the order they're present.
            PppHeader pppHeader;
            copy->RemoveHeader(pppHeader);
            Ipv4Header ipHeader;
            copy->RemoveHeader(ipHeader);
            //std::cout<<context<<std::endl;
            //std::cout << "\tsrc="<<ipHeader.GetSource()<<" dst="<<ipHeader.GetDestination()<<std::endl;
            //tagCopy.Print(std::cout<<"\t");
            uint32_t i = tagCopy.GetInterfaceId()-1;
            uint32_t j = tagCopy.GetLeafId()-Globals::spineCount;
            //std::cout<<"("<<i<<","<<j<<")"<<std::endl;
            //std::cout<<"\t("<<StateActionManager::m_path_drops.size()<<","<<StateActionManager::m_path_drops[0].size()<<")"<<std::endl;

            StateActionManager::m_path_drops[i][j]++;
        }

    }

    static  vec_stats_t m_q_drops_leaves;
    static  vec_stats_t m_q_drops_spines;

    static  vec_statsd64_t m_bandwidths_spines;
    static  vec_statsd64_t m_bandwidths_leaves;

    static vec_statsu64_t m_q_size_spines;
    static vec_statsu64_t m_q_size_leaves;


    static vec_statsu64_t m_path_drops;


    static void init();

};

#endif