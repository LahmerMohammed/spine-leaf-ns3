//
// Created by slahmer on 3/1/22.
//

#include "aarl-ipv4-routing.h"
#include "ns3/core-module.h"

NS_LOG_COMPONENT_DEFINE ("Ipv4RlRouting");


ns3::Ipv4RlRouting::Ipv4RlRouting ()
{
  std::cout<<"==========================================Ipv4RlRouting()"<<this->m_ipv4<<std::endl;

}


ns3::Ipv4RlRouting::~Ipv4RlRouting ()
{
}


ns3::TypeId
ns3::Ipv4RlRouting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Ipv4RlRouting")
                          .SetParent<Object> ()
                          .SetGroupName ("RlRouting")
                          .AddConstructor<Ipv4RlRouting> ()
                          .AddAttribute ("d", "Sample d random outputs queue",
                                         UintegerValue (2),
                                         MakeUintegerAccessor (&Ipv4RlRouting::m_d),
                                         MakeUintegerChecker<uint32_t> ())
      ;

  return tid;
}


void
ns3::Ipv4RlRouting::AddRoute (ns3::Ipv4Address network, ns3::Ipv4Mask networkMask, uint32_t port)
{
  std::cout<<"==========================================AddRoute"<<std::endl;

}


std::vector<ns3::DrillRouteEntry>
ns3::Ipv4RlRouting::LookupDrillRouteEntries (ns3::Ipv4Address dest)
{
  std::cout<<"==========================================LookupDrillRouteEntries()"<<this->m_ipv4<<std::endl;

  return std::vector<DrillRouteEntry> ();
}


uint32_t
ns3::Ipv4RlRouting::CalculateQueueLength (uint32_t interface)
{
  std::cout<<"==========================================CalculateQueueLength()"<<this->m_ipv4<<std::endl;

  return 0;
}


ns3::Ptr<ns3::Ipv4Route>
ns3::Ipv4RlRouting::ConstructIpv4Route (uint32_t port, ns3::Ipv4Address destAddress)
{
  std::cout<<"==========================================ConstructIpv4Route()"<<this->m_ipv4<<std::endl;

  return {};
}


ns3::Ptr<ns3::Ipv4Route>
ns3::Ipv4RlRouting::RouteOutput (ns3::Ptr<ns3::Packet> p, const ns3::Ipv4Header &header,
                                 ns3::Ptr<ns3::NetDevice> oif, ns3::Socket::SocketErrno &sockerr)
{
  //std::cout<<"==========================================RouteOutput()"<<this->m_ipv4<<std::endl;

  return {};
}


bool
ns3::Ipv4RlRouting::RouteInput (ns3::Ptr<const ns3::Packet> p, const ns3::Ipv4Header &header,
                                ns3::Ptr<const ns3::NetDevice> idev,
                                ns3::Ipv4RoutingProtocol::UnicastForwardCallback ucb,
                                ns3::Ipv4RoutingProtocol::MulticastForwardCallback mcb,
                                ns3::Ipv4RoutingProtocol::LocalDeliverCallback lcb,
                                ns3::Ipv4RoutingProtocol::ErrorCallback ecb)
{

  //std::cout<<"==========================================RouteInput()"<<this->m_ipv4<<std::endl;

  return false;
}


void
ns3::Ipv4RlRouting::NotifyInterfaceUp (uint32_t interface)
{
  std::cout<<"==========================================NotifyInterfaceUp()"<<this->m_ipv4<<std::endl;

}


void
ns3::Ipv4RlRouting::NotifyInterfaceDown (uint32_t interface)
{
  std::cout<<"==========================================NotifyInterfaceDown()"<<this->m_ipv4<<std::endl;

}


void
ns3::Ipv4RlRouting::NotifyAddAddress (uint32_t interface, ns3::Ipv4InterfaceAddress address)
{
  std::cout<<"==========================================NotifyAddAddress()"<<this->m_ipv4<<std::endl;

}


void
ns3::Ipv4RlRouting::NotifyRemoveAddress (uint32_t interface, ns3::Ipv4InterfaceAddress address)
{
  std::cout<<"==========================================NotifyRemoveAddress()"<<this->m_ipv4<<std::endl;

}


void
ns3::Ipv4RlRouting::SetIpv4 (ns3::Ptr<ns3::Ipv4> ipv4)
{

}


void
ns3::Ipv4RlRouting::PrintRoutingTable (ns3::Ptr<ns3::OutputStreamWrapper> stream,
                                       ns3::Time::Unit unit) const
{
}


void
ns3::Ipv4RlRouting::DoDispose (void)
{
  Object::DoDispose ();
}
