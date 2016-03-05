#include "Server.h"
#include "ClientHandler.h"
#include "palantir/Palantir.h"
#include "utils/StringUtils.h"

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <glog/logging.h>
#include <memory>
#include <tuple>

namespace ClientHandler
{
   namespace Data
   {
      FDToClientInfoMapType    m_fdToClientInfoMap;
      NameToClientInfoMapType  m_nameToClientInfoMap;
   }
   using namespace Data;

   // This is one of the few cases where unique ptr has not been used for bufferevent in the code.
   // We need to construct a new bufferevent for every new connection, and have it persist
   // across the callbacks. We are manually free-ing the memory in onClientError callback
   // via bufferevent_free func call when we detect BEV_EVENT_ERROR | BEV_EVENT_EOF
   void acceptConnection(evconnlistener *listener, evutil_socket_t fd)
   {
      event_base *base = evconnlistener_get_base(listener);
      bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
      CHECK_NOTNULL(bev);

      bufferevent_setcb(bev, onClientRead, nullptr, onClientError, nullptr);
      bufferevent_enable(bev, EV_READ|EV_WRITE);
   }

   void onClientRead(bufferevent *bev, void*)
   {
      evbuffer *inputEvbuffer = bufferevent_get_input(bev);
      evbuffer *outputEvbuffer = bufferevent_get_output(bev);

      char input[128] = {0};
      char output[128] = {0};
      auto iBytes = evbuffer_remove(inputEvbuffer, input, sizeof(input));
      CHECK_NE(iBytes, -1);

      evutil_socket_t fd = bufferevent_getfd(bev);
      auto oBytes = parseClientInput(fd, input, iBytes, output);
      if (oBytes)
      {
         auto res = evbuffer_add(outputEvbuffer, output, oBytes);
         CHECK_NE(res, -1);
      }
   }

   void onClientError(bufferevent *bev, short flags, void*)
   {
      if (flags & BEV_EVENT_ERROR)
      {
         auto errCode = EVUTIL_SOCKET_ERROR();
         LOG(ERROR) << "Error in client buffered event! Error Code: " << errCode
                    << " Error text: " << evutil_socket_error_to_string(errCode);
      }
      if (flags & (BEV_EVENT_ERROR | BEV_EVENT_EOF))
      {
         LOG(INFO) << "Client buffer closed";
         // do some book keeping here
         evutil_socket_t fd = bufferevent_getfd(bev);
         m_fdToClientInfoMap.erase(fd);

         bufferevent_free(bev);
      }
   }

   std::size_t parseClientInput(evutil_socket_t fd, char* input, std::size_t bytes, char* output)
   {
      // null terminate input
      StringUtils::formatCharArray(input, bytes);
      std::string sinput(input);

    	auto res = sinput.compare(0, 5, Palantir::NamePrefix);
      if (res == 0)
      {
        // first time communicating with this client 
         std::string name = sinput.substr(5, sinput.size());
         addNewClient(fd, name);
      }
      else
      {
         auto result = Palantir::StrToTokenMap.find(sinput.c_str());
         if (result == Palantir::StrToTokenMap.end())
         {
            LOG(ERROR) << "Got weird string from client " << sinput;
            return 0; // early return for bad case
         }
         switch (result->second)
         {
            case Palantir::Token::REQUEST: break;
            case Palantir::Token::DONATE: break;
         }
      }
      return 0;
   }

   void addNewClient(evutil_socket_t fd, const std::string& name)
   {
      auto result = m_nameToClientInfoMap.find(name);
      if (result == m_nameToClientInfoMap.end())
      {
         auto inserted = false;
         std::tie(std::ignore, inserted) =
            m_nameToClientInfoMap.insert({ name, std::make_shared<ClientInfo>(name) });
         CHECK(inserted);
         LOG(INFO) << "Inserted " << name << " with " << fd << " in the map";
      }
      auto& clientInfo = m_nameToClientInfoMap[name];
      clientInfo->m_fd = fd;
      m_fdToClientInfoMap[fd] = clientInfo;
   }
}
