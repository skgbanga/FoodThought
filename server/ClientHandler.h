#pragma once

#include <cstdlib>
#include <event2/listener.h>
#include <event2/bufferevent.h>

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
}
