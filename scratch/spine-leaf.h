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
#define NANO_TO_SEC(a) a / 1000000000.0
#define MICRO_TO_SEC(a) a / 1000000.0
#define MILI_TO_SEC(a) a / 1000.0

#define SIMULATION_DURATION 10.0

#define SERVER_LEAF_DELAY MilliSeconds(1)
#define SERVER_LEAF_DATA_RATE "1Gbps"
#define LEAF_SPINE_DELAY MilliSeconds(1)
#define LEAF_SPINE_DATA_RATE "1Mbps"
#define PACKET_SIZE 1000
#define INTERVAL MilliSeconds(1)
#define MAX_PACKETS 3000

#define UDP_SERVER_PORT 3000

#define BASE_NETWORK "192.168.0.0"
#define BASE_NETWORK_MASK "255.255.255.0"


static std::vector<ns3::NetDeviceContainer> p2pNetDevices;
static std::vector<std::vector<ns3::Ptr<ns3::PointToPointNetDevice>>> p2pSpinesLeavesNetdevs;
static std::vector<std::vector<ns3::Ptr<ns3::PointToPointNetDevice>>> p2pLeavesSpinesNetdevs;


typedef std::vector<std::vector<uint32_t>> vec_stats_t;
typedef std::vector<std::vector<long double>> vec_stats64_t;

const uint16_t NO_DEVICE =  2*LEAF_COUNTER*SPINE_COUNTER ;

using namespace ns3;

class CollectData {

public:
    static void GetData();
    static  vec_stats_t m_q_drops_leaves;
    static  vec_stats_t m_q_drops_spines;

    static  vec_stats64_t m_bandwidths_spines;
    static  vec_stats64_t m_bandwidths_leaves;


  /*
    inline static void start_tx(const Ptr<Node>& node, uint32_t interface){
      auto key = std::make_pair (node, interface);
      auto tmp = m_history.find (key);
      if(tmp == m_history.end()){
          m_history[key].up_duration = 0;
      }
      m_history[key].start_tx = Simulator::Now();;
    }
    static void stopped_tx(const Ptr<Node>& node, uint32_t interface){
      auto key = std::make_pair (node, interface);
      auto tmp = m_history.find (key);
      if(tmp == m_history.end()){
          std::cerr<<"called stopped TX but no entry found on the map"<<std::endl;
          exit (1);
      }
      m_history[key].up_duration += (Simulator::Now() - m_history[key].start_tx).GetMilliSeconds();

    }

    typedef struct{
      uint64_t up_duration;
      Time start_tx;
    } hist_t;

    typedef std::map<std::pair<Ptr<Node>, uint32_t>, hist_t> history_mat_t;
    */
private:



   // static history_mat_t m_history;

};
//static CollectData::history_mat_t m_history;

#endif