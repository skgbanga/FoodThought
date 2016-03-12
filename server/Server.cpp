#include "Server.h"
#include "ClientHandler.h"
#include "utils/SocketUtil.h"
#include "utils/ConfigObject.h"

#include <iostream>
#include <memory>
#include <glog/logging.h>
#include <event2/event.h>
#include <event2/listener.h>

namespace Server
{
   namespace Data
   {
      int               m_timeout {10};
      in_port_t         m_port {8080};
      std::string       m_addr {"127.0.0.1"};
      [[deprecated]] constexpr int     m_maxNoClients {128};
      [[deprecated]] evutil_socket_t   m_serverSocketFd {0};

      using EventConfigUPType  = std::unique_ptr<event_config, decltype(&event_config_free)>;
      using EventBaseUPType    = std::unique_ptr<event_base, decltype(&event_base_free)>;
      using EventUPType        = std::unique_ptr<event, decltype(&event_free)>;
      using ConnListenerUPType = std::unique_ptr<evconnlistener, decltype(&evconnlistener_free)>;

      EventConfigUPType    m_upEventConfig {nullptr, event_config_free};
      EventBaseUPType      m_upEventBase {nullptr, event_base_free};
      EventUPType          m_upTimerEvent {nullptr, event_free};
      EventUPType          m_upSignalEvent {nullptr, event_free};
      ConnListenerUPType   m_upConnListener {nullptr, evconnlistener_free};
   }
   using namespace Data;

   bool initialize(const ConfigObject& config)
   {
      setUpData(config);
      setUpEventBase();
      setUpGlobalTimer();
      setUpSignalHandler();

      setUpConnectionListener();
      return ClientHandler::initialize(config);
   }

   void setUpData(const ConfigObject& config)
   {
      m_timeout = config.getInt("SERVER.timeout", 10);
      m_port = config.getInt("SERVER.port", 8080);
      m_addr = config.getString("SERVER.ip", "127.0.0.1");
   }

   void run()
   {
      event_base_dispatch(m_upEventBase.get());
   }

   void setUpEventBase()
   {
      // create lib event base config
      m_upEventConfig.reset(event_config_new());
      CHECK_NOTNULL(m_upEventConfig.get());
      auto flag = event_config_set_flag(m_upEventConfig.get(), EVENT_BASE_FLAG_NOLOCK);
      CHECK_EQ(flag, 0) << "Couldn't set nolock flag!";

      // create lib event base
      m_upEventBase.reset(event_base_new_with_config(m_upEventConfig.get()));
      CHECK_NOTNULL(m_upEventBase.get());
   }

   void setUpGlobalTimer()
   {
      CHECK_NOTNULL(m_upEventBase.get());
      // create timer event (evtimer_new is wrong because it doesn't set EV_PERSIST flag!)
      m_upTimerEvent.reset(event_new(m_upEventBase.get(), -1, EV_TIMEOUT | EV_PERSIST,
                                     onTimeout, nullptr));
      CHECK_NOTNULL(m_upTimerEvent.get());
      timeval temp = {m_timeout, 0};
      CHECK_EQ(evtimer_add(m_upTimerEvent.get(), &temp), 0) << "Couldn't add timer event!";
   }

   void setUpSignalHandler()
   {
      // Right now we are only handling SIGINT.
      // Add more as your desire
      CHECK_NOTNULL(m_upEventBase.get());
      m_upSignalEvent.reset(evsignal_new(m_upEventBase.get(), SIGINT, onSIGINT, nullptr));
      CHECK_NOTNULL(m_upSignalEvent.get());
      CHECK_EQ(evsignal_add(m_upSignalEvent.get(), nullptr), 0) << "Couldn't add signal event!";
   }

   void bindAndStartListening()
   {
      // small utility checking lambda
      auto checkError = [](int err, const char* act)
      {
         auto errCode = EVUTIL_SOCKET_ERROR();
         CHECK_GE(err, 0) << "Error while " << act
                          << ". Reason :" << evutil_socket_error_to_string(errCode);
      };

      // Create a non-blocking, re-useable server socket
      m_serverSocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      checkError(m_serverSocketFd, "creation");
      evutil_make_socket_nonblocking(m_serverSocketFd);
      evutil_make_listen_socket_reuseable(m_serverSocketFd);

      // Bind it to an address
      sockaddr sAddr {};
      SocketUtil::getSockAddrFromIpPort(&sAddr, m_addr.c_str(), m_port);
      auto errBind = bind(m_serverSocketFd, &sAddr, sizeof(sAddr));
      checkError(errBind, "binding");

      // start listening
      auto errListen = listen(m_serverSocketFd, m_maxNoClients);
      checkError(errListen, "listening");
   }

   /*
    * setupConnectionListener: 
    * sets a connection listener, binds to server address and start listening on that server
    * through evconnlistener_new_bind libevent call
    **/
   void setUpConnectionListener()
   {
      CHECK_NOTNULL(m_upEventBase.get());

      sockaddr sAddr {};
      SocketUtil::getSockAddrFromIpPort(&sAddr, m_addr.c_str(), m_port);

      m_upConnListener.reset(evconnlistener_new_bind(m_upEventBase.get(), onConnection, nullptr,
                                                     LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                                     -1, // libevent will choose a sane value
                                                     &sAddr,
                                                     sizeof(sAddr)));

      CHECK_NOTNULL(m_upConnListener.get());
      evconnlistener_set_error_cb(m_upConnListener.get(), onConnectionError);
   }

   void onTimeout(evutil_socket_t, short, void*)
   {
      // One could call event_base_gettimeofday_cached to get current cached time in this callback
      LOG(INFO) << "Called after " << m_timeout << " seconds";
      ClientHandler::onTimeout();
   }

   void onSIGINT(evutil_socket_t, short, void*)
   {
      LOG(INFO) << "Shutdown called";
      bool shutdownResult = ClientHandler::shutdown();
      LOG_IF(ERROR, !shutdownResult) << "Not able to properly shutdown strategy!";
      event_base_loopexit(m_upEventBase.get(), nullptr);
   }

   void onConnection(evconnlistener* listener, evutil_socket_t socket, sockaddr* addr, int, void*)
   {
      // Got a new connection!
      char ip[16];
      SocketUtil::getIpFromSockAddr(addr, ip, sizeof(ip));
      LOG(INFO) << "Got a new connection from " << ip << ":" << SocketUtil::getPortFromSockAddr(addr);
      ClientHandler::acceptConnection(listener, socket);
   }

   void onConnectionError(evconnlistener *, void*)
   {
      // gets called if there is an error on accept(). Nothing when it disconnects.
      auto errCode = EVUTIL_SOCKET_ERROR();
      LOG(ERROR) << "Error on connection listener! Error Code: " << errCode
                 << " Error text: " << evutil_socket_error_to_string(errCode);
   }
} // end namespace server
