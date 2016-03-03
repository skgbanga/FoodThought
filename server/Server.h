#pragma once

#include <memory>
#include <glog/logging.h>
#include <event2/event.h>

#include "LibEventDeleters.h"

namespace Server
{
   // functions
   void Initialize();
   void onTimeout(evutil_socket_t, short, void*);
   void run();
}
