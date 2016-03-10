#pragma once

#include <event2/event.h>
#include <event2/listener.h>

// Libevent refrence manual: http://www.wangafu.net/~nickm/libevent-book/TOC.html
// Most libevent functions take callback functions as arguments, and since non-static
// memeber functions can't be passed, this makes writing Server as a class useless.
// (A single monolithic static server class?)
// Hence, defining Server as a namespace containing non-member functions which can be used
// as callback functions in libevent API
// If you have a better idea, please contact sandeep.skg3@gmail.com

class ConfigObject;

namespace Server
{
   bool initialize(const ConfigObject&);

   void setUpData(const ConfigObject&);
   void setUpEventBase();
   void setUpGlobalTimer();
   void setUpConnectionListener();

   [[deprecated("use setupConnectionListener instead")]]
   void bindAndStartListening();

   void onTimeout(evutil_socket_t, short, void*);
   void onConnection(evconnlistener *, evutil_socket_t, sockaddr *, int, void*);
   void onConnectionError(evconnlistener *, void*);
   void run();
}
