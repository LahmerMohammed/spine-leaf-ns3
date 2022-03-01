
#include <random>
#include "spine-leaf.h"
#include "globals.h"
vec_stats_t StateActionManager::m_q_drops_spines = std::vector(Globals::spineCount,std::vector<uint32_t>(Globals::leafCount));
vec_stats_t StateActionManager::m_q_drops_leaves = std::vector(Globals::leafCount,std::vector<uint32_t>(Globals::spineCount));

vec_stats64_t StateActionManager::m_bandwidths_spines = std::vector(Globals::spineCount,std::vector<long double>(Globals::leafCount));
vec_stats64_t StateActionManager::m_bandwidths_leaves = std::vector(Globals::leafCount,std::vector<long double>(Globals::spineCount));

void
StateActionManager::GetData()
{
  for (uint32_t i = 0; i < Globals::spineCount+Globals::leafCount ; ++i)
  {
    if(i < Globals::spineCount){
        for (uint32_t j = 0; j < Globals::leafCount; ++j)
          {
            m_q_drops_spines[i][j] = p2pSpinesLeavesNetdevs[i][j]->GetQueue()->GetTotalDroppedPacketsBeforeEnqueue();
            //auto tmp = p2pSpinesLeavesNetdevs[i][j]->time_slot();
            //m_bandwidths_spines[i][j] = tmp.first ;//* tmp.second.GetBitRate();

          }
      }
    else{
        uint32_t leaf = i-2;

        for (uint32_t j = 0; j < Globals::spineCount; ++j)
          {

            m_q_drops_leaves[leaf][j] = p2pLeavesSpinesNetdevs[leaf][j]->GetQueue()->GetTotalDroppedPacketsBeforeEnqueue();
            auto tmp = p2pLeavesSpinesNetdevs[leaf][j]->time_slot();
            auto dura= static_cast<long double>(NANO_TO_SEC(tmp.first));
            if(dura == 0.0){
                m_bandwidths_leaves[leaf][j] = 0;
            }
            else{
                m_bandwidths_leaves[leaf][j] = static_cast<long double>(tmp.second.GetBitRate()) *  dura;//* tmp.second.GetBitRate();

            }

          }

      }
    // TODO for leaves spines interfaces

  }
  /*
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

        //m_q_drops[i_leaf][i_spine] = leaf_queue->GetTotalDroppedPacketsBeforeEnqueue();
        //m_q_drops[i_spine][i_leaf] = spine_queue->GetTotalDroppedPacketsBeforeEnqueue();
        //auto tmp1 = leaf_netDev->time_slot();
        //auto tmp2 = leaf_netDev->time_slot();

    }
    */
    //return m_data;

}


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SpineAndLeaf");















