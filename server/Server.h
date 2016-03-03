#pragma once

#include <memory>
#include <glog/logging.h>
#include <event2/event.h>

namespace Server
{
   void Initialize();
   void onTimeout(evutil_socket_t, short, void*);
   void run();

   struct Data
   {
      int               m_timeout {5};
      short             m_port {8080};
      int               m_maxNoClients {128};
      evutil_socket_t   m_serverSocketFd {0};
      std::unique_ptr<event_config, decltype(&event_config_free)>  m_upEventConfig {nullptr, event_config_free};
      std::unique_ptr<event_base, decltype(&event_base_free)>      m_upEventBase {nullptr, event_base_free};
      std::unique_ptr<event, decltype(&event_free)>                m_upTimerEvent {nullptr, event_free};
   };
}
