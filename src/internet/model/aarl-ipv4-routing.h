//
// Created by slahmer on 3/1/22.
//

#ifndef NS3_AARLIPV4ROUTING_H
#define NS3_AARLIPV4ROUTING_H
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4-route.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-address.h"
#include "ipv4-global-routing.h"
#include "ns3/ipv4-routing-protocol.h"

namespace ns3{
struct DrillRouteEntry {
  Ipv4Address network;
  Ipv4Mask networkMask;
  uint32_t port;
};



class Ipv4RlRouting : public Ipv4RoutingProtocol {


public:
  Ipv4RlRouting ();
  ~Ipv4RlRouting ();

  static TypeId GetTypeId (void);

  void AddRoute (Ipv4Address network, Ipv4Mask networkMask, uint32_t port);

  /* Inherit From Ipv4RoutingProtocol */
  virtual Ptr<Ipv4Route> RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);
  virtual bool RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
                           UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                           LocalDeliverCallback lcb, ErrorCallback ecb);
  virtual void NotifyInterfaceUp (uint32_t interface);
  virtual void NotifyInterfaceDown (uint32_t interface);
  virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void SetIpv4 (Ptr<Ipv4> ipv4);
  virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit = Time::S) const;

  virtual void DoDispose (void);

  void init_routes(Ptr<Ipv4> ipv4, const Ipv4GlobalRouting::HostRoutes&, const Ipv4GlobalRouting::NetworkRoutes&);

private:
  uint32_t m_d;
  Ptr<Ipv4> m_ipv4;
  Ipv4GlobalRouting::HostRoutes m_hostRoutes;             //!< Routes to hosts
  Ipv4GlobalRouting::NetworkRoutes m_networkRoutes;       //!< Routes to networks


  std::vector<Ptr<Ipv4Route>> Lookup (Ipv4Address dest, const Ptr<NetDevice>& oif = nullptr);
};

}

#endif //NS3_AARLIPV4ROUTING_H
