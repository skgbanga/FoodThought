#pragma once

#include <memory>
#include <glog/logging.h>
#include <event2/event.h>

#include "LibEventDeleters.h"

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
      std::unique_ptr<event_config, Deleters::EventConfig>  m_upEventConfig {nullptr};
      std::unique_ptr<event_base, Deleters::EventBase>      m_upEventBase {nullptr};
      std::unique_ptr<event, Deleters::Event>               m_upTimerEvent {nullptr};
   };
}
