#ifndef COLLECT_DATA_H
#define COLLECT_DATA_H


#include "./spine-leaf.h"

const uint16_t NO_DEVICE =  2*LEAF_COUNTER*SPINE_COUNTER ;

using namespace ns3;

class CollectData {

public:
    static void GetData()
    {


        for(auto it = ChannelList::Begin() ; it < ChannelList::End() ; it++)
        {
            Ptr<Channel> channel = *it;
            Ptr<PointToPointNetDevice> netDev0 = DynamicCast<PointToPointNetDevice>(channel->GetDevice(0));
            Ptr<PointToPointNetDevice> netDev1 = DynamicCast<PointToPointNetDevice>(channel->GetDevice(1));

            Node::Type nType = netDev0->GetNode()->GetType();

            if(  nType == Node::Type::LEAF)
            {
                uint16_t i_leaf = netDev0->GetNode()->GetId() ,
                         j_spine = netDev1->GetNode()->GetId();

                Ptr<DropTailQueue<Packet>> queue0 = DynamicCast<DropTailQueue<Packet>>(netDev0->GetQueue());
                Ptr<DropTailQueue<Packet>> queue1 = DynamicCast<DropTailQueue<Packet>>(netDev1->GetQueue());

                data[i_leaf][j_spine][0] =  queue0->GetCurrentSize().GetValue();
                data[i_leaf][j_spine][1] =  queue0->GetTotalDroppedBytes();
                data[i_leaf][j_spine][2] =  queue0->GetTotalReceivedBytes();


                data[j_spine][i_leaf][0] =  queue1->GetCurrentSize().GetValue();
                data[j_spine][i_leaf][1] =  queue1->GetTotalDroppedBytes();
                data[j_spine][i_leaf][2] =  queue1->GetTotalReceivedBytes();

            }
        }





    }


private:
    static uint32_t data[NO_DEVICE][NO_DEVICE][3];

};





#endif