#include "Server.h"
#include "ClientHandler.h"
#include "palantir/Palantir.h"
#include "utils/StringUtils.h"
#include "strategy/AllStrategies.h"

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <glog/logging.h>
#include <memory>
#include <tuple>
#include <cstdio>
#include <unordered_map>
#include <cstring>

namespace ClientHandler
{
   namespace Data
   {
      using FDMapType = std::unordered_map<evutil_socket_t, std::string>;
      FDMapType m_fdMap {};;
      // use a macro here and have a different library for each strategy
      DefaultStrategy m_strategy {};
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
         LOG_IF(ERROR, oBytes > sizeof(output)) << "Asking me to write more data than my output buffer!";
         auto toWrite = std::min(oBytes, sizeof(output)); // in case more was written than is the buffer
         auto res = evbuffer_add(outputEvbuffer, output, toWrite);
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
         m_fdMap.erase(fd);

         bufferevent_free(bev);
      }
   }

   std::size_t parseClientInput(evutil_socket_t fd, char* input, std::size_t bytes, char* output)
   {
      std::size_t written = 0;
      std::vector<std::string> tokens;
      StringUtils::tokenize(input, bytes, tokens);

      if (tokens.empty())
      {
         LOG(ERROR) << "Wrong input!";
         return 0;
      }

      const std::string& token1 = tokens[0];
    	auto res = token1.compare(0, 5, Palantir::NamePrefix);
      if (res == 0)
      {
        // first time communicating with this client in this session
         std::string name = token1.substr(5, token1.size());
         addNewClient(fd, name);
         written = std::sprintf(output, "Hey %s, What's up!\n", name.c_str());
      }
      else
      {
         using namespace Palantir;
         auto result = StrToTokenMap.find(token1);
         if (result == StrToTokenMap.end())
         {
            written = std::sprintf(output, "Please talk language which I can understand\n");
            return written;
         }
         auto search = m_fdMap.find(fd);
         if (search == m_fdMap.end())
         {
            written = std::sprintf(output, "Who are you! Did you tell me your name first\n");
            return written;
         }
         auto& name = search->second;
         switch (result->second)
         {
            case Token::REQUEST:
               LOG(INFO) << name << " requesting " << tokens[1];
               written = m_strategy.request(name, tokens, output);
               break;
            case Token::DONATE:
               LOG(INFO) << name << " donating " << tokens[1];
               written = m_strategy.donate(name, tokens, output);
               break;
         }
      }
      return written;
   }

   void addNewClient(evutil_socket_t fd, const std::string& name)
   {
      m_fdMap[fd] = name;
      CHECK(m_strategy.addNewClient(name));
   }
}
