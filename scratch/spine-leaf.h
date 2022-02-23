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
#include <vector>
#include<tuple>



#define SPINE_COUNTER 2
#define LEAF_COUNTER 4
#define SERVER_COUNTER 1 // server per LEAF-router
#define SERVERS_COUNT SERVER_COUNTER*LEAF_COUNTER

#define SIMULATION_DURATION 10.0

#define SERVER_LEAF_DELAY MilliSeconds(1)
#define SERVER_LEAF_DATA_RATE "1Gbps"
#define LEAF_SPINE_DELAY MilliSeconds(1)
#define LEAF_SPINE_DATA_RATE "9Mbps"
#define PACKET_SIZE 1000
#define INTERVAL MilliSeconds(1)
#define MAX_PACKETS 3000

#define UDP_SERVER_PORT 3000

#define BASE_NETWORK "192.168.0.0"
#define BASE_NETWORK_MASK "255.255.255.0"


static std::vector<ns3::NetDeviceContainer> p2pNetDevices;

#define VecVecQueueDisc std::vector<std::vector<uint32_t>>
#define VecQueueDisc std::vector<uint32_t>

const uint16_t NO_DEVICE =  2*LEAF_COUNTER*SPINE_COUNTER ;

using namespace ns3;

class CollectData {

public:
    static VecVecQueueDisc GetData();


private:
    static  VecVecQueueDisc m_data;

};

#endif