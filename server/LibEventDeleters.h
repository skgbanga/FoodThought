#pragma once
#include <event2/event.h>

namespace Deleters
{
   struct EventConfig
   {
      void operator()(event_config *cfg)
      {
         event_config_free(cfg);
      }
   };

   struct EventBase
   {
      void operator()(event_base* base)
      {
         event_base_free(base);
      }
   };

   struct Event
   {
      void operator()(event* ev)
      {
         event_free(ev);
      }
   };
}
