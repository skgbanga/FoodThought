#pragma once

#include <cstdlib>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <string>

class ConfigObject;

namespace ClientHandler
{
   bool initialize(const ConfigObject&);
   void acceptConnection(evconnlistener *, evutil_socket_t);
   void onClientRead(bufferevent *, void *);
   void onClientError(bufferevent *, short, void*);
   void shutdown();

   // Arguments:
   // 1. File descriptor
   // 2. Input that we receieved from socket
   // 3. No. of bytes that we received
   // 4. Output that we want to write to buffer
   // Return type
   // 1. No. of bytes to be written to output buffer
   // Users of this function should ensure that they pass sufficiently large output buffer
   std::size_t parseClientInput(evutil_socket_t fd, char *input, std::size_t bytes, char* output);
   void addNewClient(evutil_socket_t fd, const std::string& name);
}
