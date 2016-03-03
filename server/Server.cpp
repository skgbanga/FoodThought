#include "Server.h"

namespace Server
{

Data data {};

// definitons of all functions
void Initialize()
{
   // create lib event base config
   data.m_upEventConfig.reset(event_config_new());
   CHECK_NOTNULL(data.m_upEventConfig.get());
   auto flag = event_config_set_flag(data.m_upEventConfig.get(), EVENT_BASE_FLAG_NOLOCK);
   CHECK_EQ(flag, 0) << "Couldn't set nolock flag!";
   // More flags can be set in the config. Refer to
   // http://www.wangafu.net/~nickm/libevent-book/Ref2_eventbase.html

   // create lib event base
   data.m_upEventBase.reset(event_base_new_with_config(data.m_upEventConfig.get()));
   CHECK_NOTNULL(data.m_upEventBase.get());

   // create timer event
   data.m_upTimerEvent.reset(event_new(data.m_upEventBase.get(), -1, EV_TIMEOUT | EV_PERSIST, onTimeout, nullptr));
   CHECK_NOTNULL(data.m_upTimerEvent.get());
   timeval temp = {data.m_timeout, 0};
   CHECK_EQ(evtimer_add(data.m_upTimerEvent.get(), &temp), 0) << "Couldn't add timer event!";

   // small utility checking lambda
   auto checkError = [](int err, const char* act)
   {
      auto errCode = EVUTIL_SOCKET_ERROR();
      CHECK_GE(err, 0) << "Error while " << act
                       << ". Reason :" << evutil_socket_error_to_string(errCode);
   };

   // Create a non-blocking, re-useable server socket
   data.m_serverSocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   checkError(data.m_serverSocketFd, "creation");
   evutil_make_socket_nonblocking(data.m_serverSocketFd);
   evutil_make_listen_socket_reuseable(data.m_serverSocketFd);

   // Bind it to an address
   sockaddr_in serverAddress {AF_INET, htonl(INADDR_ANY), htons(data.m_port), {0}};
   auto errBind = bind(data.m_serverSocketFd, (sockaddr *)&serverAddress, sizeof(serverAddress));
   checkError(errBind, "binding");

   // start listening
   auto errListen = listen(data.m_serverSocketFd, data.m_maxNoClients);
   checkError(errListen, "listening");
}

void onTimeout(evutil_socket_t, short, void*)
{
   // One could call event_base_gettimeofday_cached to get current cached time in this callback
   LOG(INFO) << "Called after " << data.m_timeout << " seconds";
}

void run()
{
   event_base_dispatch(data.m_upEventBase.get());
}

} // end namespace server
