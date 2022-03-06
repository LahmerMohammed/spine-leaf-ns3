
#include <random>
#include "spine-leaf.h"
#include "globals.h"
#include "topology-builder.h"
vec_stats_t StateActionManager::m_q_drops_spines = std::vector(Globals::spineCount,std::vector<uint32_t>(Globals::leafCount));
vec_stats_t StateActionManager::m_q_drops_leaves = std::vector(Globals::leafCount,std::vector<uint32_t>(Globals::spineCount));

vec_statsd64_t StateActionManager::m_bandwidths_spines = std::vector(Globals::spineCount,std::vector<long double>(Globals::leafCount));
vec_statsd64_t StateActionManager::m_bandwidths_leaves = std::vector(Globals::leafCount,std::vector<long double>(Globals::spineCount));


vec_statsu64_t StateActionManager::m_q_size_spines = std::vector(Globals::spineCount,std::vector<uint64_t>(Globals::leafCount));
vec_statsu64_t StateActionManager::m_q_size_leaves = std::vector(Globals::leafCount,std::vector<uint64_t>(Globals::spineCount));

vec_statsu64_t StateActionManager::m_path_drops = std::vector(Globals::spineCount,std::vector<uint64_t>(Globals::leafCount,0));




void
StateActionManager::GetData()
{
  for (uint32_t i = 0; i < Globals::spineCount+Globals::leafCount ; ++i)
  {
    if(i < Globals::spineCount){
        for (uint32_t j = 0; j < Globals::leafCount; ++j)
          {
            auto zz = TopologyBuilder::p2pSpinesLeavesNetdevs[i][j]->GetQueue();
            m_q_drops_spines[i][j] = zz->GetTotalDroppedPacketsBeforeEnqueue();
            zz->resetStats();
            m_q_size_spines[i][j] = zz->GetCurrentSize().GetValue();

            auto tmp = TopologyBuilder::p2pSpinesLeavesNetdevs[i][j]->time_slot();
            auto dura= static_cast<long double>(NANO_TO_SEC(tmp.first));
            long double max =  Globals::envStepTime * tmp.second.GetBitRate();
            if(dura == 0.0){
                m_bandwidths_spines[i][j] = max;
              }
            else{
                m_bandwidths_spines[i][j] = max - static_cast<long double>(tmp.second.GetBitRate()) *  dura;//* tmp.second.GetBitRate();
            }
            //m_bandwidths_spines[i][j] = tmp.first ;//* tmp.second.GetBitRate();
          }
      }
    else{
        uint32_t leaf = i-2;

        for (uint32_t j = 0; j < Globals::spineCount; ++j)
          {
            auto xx=  TopologyBuilder::p2pLeavesSpinesNetdevs[j][leaf];
            m_q_drops_leaves[leaf][j] = xx->GetQueue()->GetTotalDroppedPacketsBeforeEnqueue();
            xx->GetQueue()->resetStats();
            m_q_size_leaves[leaf][j] = xx->GetQueue()->GetCurrentSize().GetValue();

            auto tmp = xx->time_slot();
            long double max =  Globals::envStepTime * tmp.second.GetBitRate();
            auto dura= static_cast<long double>(NANO_TO_SEC(tmp.first));
            if(dura == 0.0){
                m_bandwidths_leaves[leaf][j] = max;
            }
            else{
                m_bandwidths_leaves[leaf][j] = max - static_cast<long double>(tmp.second.GetBitRate()) *  dura;//* tmp.second.GetBitRate();
            }

          }
      }
  }

  //Print distribution
  std::cout<<"Statistics"<<std::endl;
  for (uint64_t i = 0; i < Globals::leafCount; ++i)
    {
      std::cout<<"Leaf("<<i<<")"<<std::endl;
      auto tmp = TopologyBuilder::leaf_rl_routers[i];
      auto s = tmp->getStats();
      auto s2 = tmp->getProbs();

      std::cout<<"\troute1="<<s[0]<<" , route2="<<s[1]<<std::endl;
      std::cout<<"\tprob1="<<s2[0]<<" , prob2="<<s2[1]<<std::endl;

    }
}
void
StateActionManager::ApplyNewAction (std::vector<float> actions)
{
  //std::cout<<"Apply Action"<<std::endl;
  NS_ASSERT (actions.size() % Globals::action_space == 0);
  NS_ASSERT (actions.size() / Globals::action_space == Globals::leafCount);
  //std::cout<<"Assert Done (Action size):"<<Globals::action_space <<std::endl;

  //uint32_t next_elm = 0;
  //uint32_t next_rl_router = 0;

  std::vector<float> tmp = {actions[0], actions[1]};
  TopologyBuilder::leaf_rl_routers[0]->resetStats();
  TopologyBuilder::leaf_rl_routers[0]->SetDistribution (tmp);

}
void
StateActionManager::init ()
{
}

using namespace ns3;
















