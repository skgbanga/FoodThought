#pragma once

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string>
#include "strategy/StrategyBase.h"
#include <vector>


class ConfigObject;

namespace ClientHandler
{
   void acceptConnection(evconnlistener *, evutil_socket_t);
   void onClientRead(bufferevent *, void *);
   void onClientError(bufferevent *, short, void*);
   void onTimeout();
   bool shutdown();


   bool initialize(const ConfigObject&);
   std::unique_ptr<strategy::StrategyBase> makeStrategy(const ConfigObject&);

   // Arguments:
   // 1. File descriptor
   // 2. Input that we receieved from socket
   // 3. No. of bytes that we received
   // 4. Output that we want to write to buffer
   // Return type
   // 1. No. of bytes to be written to output buffer
   // Users of this function should ensure that they pass sufficiently large output buffer
   std::size_t parseClientInput(evutil_socket_t fd, char *input, std::size_t bytes, char* output);

   std::size_t handleQueryToken(char*);
   std::size_t handleNameToken(evutil_socket_t, const std::string& name, char*);

   template <typename... Args>
   std::size_t handleDonateToken(Args&&...);
   template <typename... Args>
   std::size_t handleRequestToken(Args&&...);

   std::size_t handleRequestDonateCommon(const std::string&, evutil_socket_t, const std::string&, char*);

   bool checkErrorInInput(const std::vector<std::string>&);
   bool getAmount(const std::string& str, double& amount);
   void addNewClient(evutil_socket_t fd, const std::string& name);
}
