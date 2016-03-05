#pragma once

#include <string>
#include <unordered_map>
#include <event2/listener.h>
#include <event2/bufferevent.h>

struct ClientInfo
{
   ClientInfo(std::string _name) : m_name(std::move(_name)) {}
   evutil_socket_t   m_fd {0};
   std::string       m_name;
};

namespace ClientHandler
{
   void acceptConnection(evconnlistener *, evutil_socket_t);
   void onClientRead(bufferevent *, void *);
   void onClientError(bufferevent *, short, void*);

   // Arguments:
   // 1. File descriptor
   // 2. Input that we receieved from socket
   // 3. No. of bytes that we received
   // 4. Output that we want to write to buffer
   // Return type
   // 1. No. of bytes to be written to output buffer
   std::size_t parseClientInput(evutil_socket_t fd, char *input, std::size_t bytes, char* output);
   void addNewClient(evutil_socket_t fd, const std::string& name);

   namespace Data
   {
      using FDToClientInfoMapType    = std::unordered_map<evutil_socket_t, std::shared_ptr<ClientInfo>>;
      using NameToClientInfoMapType  = std::unordered_map<std::string, std::shared_ptr<ClientInfo>>;
   }
}
