#pragma once

#include <memory>
#include <glog/logging.h>
#include <event2/event.h>

// Libevent refrence manual: http://www.wangafu.net/~nickm/libevent-book/TOC.html
// Most libevent functions take callback functions as arguments, and since non-static
// memeber functions can't be passed, this makes writing Server as a class useless.
// (A single monolithic static server class? No)
// Hence, defining Server as a namespace containing non-member functions which can be used
// as callback functions in libevent API

namespace Server
{
   void Initialize();
   void onTimeout(evutil_socket_t, short, void*);
   void run();
}
