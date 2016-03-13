#include "Server.h"
#include "ClientHandler.h"
#include "palantir/Palantir.h"
#include "utils/StringUtils.h"
#include "utils/ConfigObject.h"

#include "strategy/FCFSStrategy.hpp"
#include "strategy/DefaultMerlinStrategy.h"

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
      FDMapType m_fdMap {};
      std::unique_ptr<strategy::StrategyBase> m_upStrategy = nullptr;
   }
   using namespace Data;

   bool initialize(const ConfigObject& config)
   {
      try
      {
         m_upStrategy = makeStrategy(config);
         return true;
      }
      catch (std::invalid_argument& e)
      {
         LOG(ERROR) << e.what();
         return false;
      }
   }

   std::unique_ptr<strategy::StrategyBase> makeStrategy(const ConfigObject& config)
   {
      std::string strategyName = config.getString("STRATEGY.name", "fcfs");
      if (strategyName == "fcfs")
      {
         return std::make_unique<strategy::FCFSStrategy>(config);
      }
      if (strategyName == "merlin")
      {
         return std::make_unique<strategy::DefaultMerlinStrategy>(config);
      }

      throw std::invalid_argument("strategy name " + strategyName + " now known");
   }

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

         output[toWrite - 1] = '\0';
         LOG(INFO) << "Wrote to Client: " << output;
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
      std::vector<std::string> tokens {};
      StringUtils::tokenize(input, bytes, tokens, ' ');

      if (checkErrorInInput(tokens))
         return std::sprintf(output, "Wrong input!\n");

      // From now on we can assume the input is well formed and don't need to do
      // any error checks - makes the code cleaner

      using namespace Palantir;
      const std::string& prefix = tokens[0];
      auto prefixToken = StrToTokenMap.at(prefix);

      switch (prefixToken)
      {
         case Token::QUERY:
            return handleQueryToken(output);
         case Token::NAME:
            return handleNameToken(fd, tokens[1], output);
         case Token::DONATE:
            return handleDonateToken(fd, tokens[1], output);
         case Token::REQUEST:
            return handleRequestToken(fd, tokens[1], output);
      }
   }

   bool checkErrorInInput(const std::vector<std::string>& tokens)
   {
      if (tokens.empty())
         return true;

      using namespace Palantir;
      const std::string& prefix = tokens[0];
      auto found = StrToTokenMap.find(prefix);
      if (found == StrToTokenMap.end()) // check whether using palantir
         return true;

      auto palantirToken = found->second;
      switch (palantirToken)
      {
         case Token::QUERY:
            break;
         case Token::NAME:
         case Token::DONATE:
         case Token::REQUEST:
            if (tokens.size() != 2) // check whether input is well formed
               return true;
      }

      if (palantirToken != Token::NAME)
      {
      }

      // all good, no error
      return false;
   }

   std::size_t handleNameToken(evutil_socket_t fd, const std::string& name, char* output)
   {
      addNewClient(fd, name);
      return std::sprintf(output, "Hey %s, What's up!\n", name.c_str());
   }

   std::size_t handleQueryToken(char* output)
   {
      auto result = m_upStrategy->query();
      return std::sprintf(output, "Amount available: %s\n", result.c_str());
   }

   bool getAmount(const std::string& amountStr, double& amount)
   {
      try
      {
         amount = std::stof(amountStr);
         return true;
      }
      catch (...)
      {
         return false;
      }
   }

   template <typename... Args>
   std::size_t handleDonateToken(Args&&... args)
   {
      return handleRequestDonateCommon("donate", std::forward<Args>(args)...);
   }

   template <typename... Args>
   std::size_t handleRequestToken(Args&&... args)
   {
      return handleRequestDonateCommon("request", std::forward<Args>(args)...);
   }

   std::size_t handleRequestDonateCommon(const std::string& method,
                                         evutil_socket_t fd,
                                         const std::string& amountStr, char* output)
   {
      auto found = m_fdMap.find(fd);
      if (found == m_fdMap.end()) // check whether they have told their name first
         return std::sprintf(output, "Please specify name first\n");
      auto name = found->second;

      double amount = 0;
      if (not getAmount(amountStr, amount))
         return std::sprintf(output, "Wrong input!\n");

      bool result = false;
      std::string resultString = "";
      if (method == "request")
         std::tie(result, resultString) = m_upStrategy->request(name, amount);
      else
         std::tie(result, resultString) = m_upStrategy->donate(name, amount);

      return std::sprintf(output, "Status: %s, Reason: %s\n",
                         (result) ? "Accepted" : "Denied", resultString.c_str());
   }

   void addNewClient(evutil_socket_t fd, const std::string& name)
   {
      m_fdMap[fd] = name;
      CHECK(m_upStrategy->addNewClient(name));
   }

   bool shutdown()
   {
      return m_upStrategy->shutdown();
   }

   void onTimeout()
   {
      m_upStrategy->onTimeout();
   }
}
