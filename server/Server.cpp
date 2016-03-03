#include "Server.h"

#include <memory>
#include <glog/logging.h>

namespace Server
{
   namespace Data
   {
      int               m_timeout {5};
      short             m_port {8080};
      int               m_maxNoClients {128};
      evutil_socket_t   m_serverSocketFd {0};
      std::unique_ptr<event_config, decltype(&event_config_free)>  m_upEventConfig {nullptr, event_config_free};
      std::unique_ptr<event_base, decltype(&event_base_free)>      m_upEventBase {nullptr, event_base_free};
      std::unique_ptr<event, decltype(&event_free)>                m_upTimerEvent {nullptr, event_free};
   }
   using namespace Data;

   // definitons of all functions
   void Initialize()
   {
      // create lib event base config
      m_upEventConfig.reset(event_config_new());
      CHECK_NOTNULL(m_upEventConfig.get());
      auto flag = event_config_set_flag(m_upEventConfig.get(), EVENT_BASE_FLAG_NOLOCK);
      CHECK_EQ(flag, 0) << "Couldn't set nolock flag!";

      // create lib event base
      m_upEventBase.reset(event_base_new_with_config(m_upEventConfig.get()));
      CHECK_NOTNULL(m_upEventBase.get());

      // create timer event
      m_upTimerEvent.reset(event_new(m_upEventBase.get(), -1, EV_TIMEOUT | EV_PERSIST, onTimeout, nullptr));
      CHECK_NOTNULL(m_upTimerEvent.get());
      timeval temp = {m_timeout, 0};
      CHECK_EQ(evtimer_add(m_upTimerEvent.get(), &temp), 0) << "Couldn't add timer event!";

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
      sockaddr_in serverAddress {AF_INET, htonl(INADDR_ANY), htons(m_port), {0}};
      auto errBind = bind(m_serverSocketFd, (sockaddr *)&serverAddress, sizeof(serverAddress));
      checkError(errBind, "binding");

      // start listening
      auto errListen = listen(m_serverSocketFd, m_maxNoClients);
      checkError(errListen, "listening");
   }

   void onTimeout(evutil_socket_t, short, void*)
   {
      // One could call event_base_gettimeofday_cached to get current cached time in this callback
      LOG(INFO) << "Called after " << m_timeout << " seconds";
   }

   void run()
   {
      event_base_dispatch(m_upEventBase.get());
   }

} // end namespace server
