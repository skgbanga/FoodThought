#pragma once

#include <cstddef>
#include <arpa/inet.h>
#include <cstring>

namespace SocketUtil
{
   // Fill IPv4 ip from sockaddr sa upto maxlen
   void getIpFromSockAddr(const sockaddr *sa, char *ip, std::size_t maxlen)
   {
      inet_ntop(AF_INET, &(((sockaddr_in *)sa)->sin_addr), ip, maxlen);
   }

   // return port from sockaddr
   in_port_t getPortFromSockAddr(const sockaddr *sa)
   {
      return ((sockaddr_in *)sa)->sin_port;
   }

   // fill sockaddr sa with IPv4 ip and port
   void getSockAddrFromIpPort(sockaddr *sa, const char* ip, in_port_t port)
   {
      sockaddr_in* inAddr = (sockaddr_in *)sa;
      inAddr->sin_family = AF_INET;
      inAddr->sin_port   = htons(port);
      inet_pton(AF_INET, ip, &(inAddr->sin_addr));
   }
}
