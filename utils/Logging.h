#pragma once
#include <event2/event.h>
#include <glog/logging.h>
#include <string>

#include "utils/ConfigObject.h"

// FoodThought uses Google logging module: https://github.com/google/glog
// to create its own production logs.
// This file implements the necessary callback function which is passed on
// to the LibEvent for logging internal errors and warnings

static void loggerCallbacks(int severity, const char* msg)
{
   switch (severity)
   {
      case EVENT_LOG_DEBUG:
         DLOG(INFO) << msg;
         break;
      case EVENT_LOG_MSG:
         LOG(INFO) << msg;
         break;
      case EVENT_LOG_WARN:
         LOG(WARNING) << msg;
         break;
      case EVENT_LOG_ERR:
         LOG(ERROR) << msg;
         break;
   }
}

static void loggerFatalCallback(int error)
{
   LOG(FATAL) << "A fatal error occured! Error code " << error;
}

static void ConfigureLogging(const char* progName, const ConfigObject& config)
{
   FLAGS_log_dir = config.getStringNoLog("COMMON.log_dir", "");
   google::InitGoogleLogging(progName);

   event_set_log_callback(loggerCallbacks);
   event_set_fatal_callback(loggerFatalCallback);
}

static void ConfigureNullLogging(const char* progName)
{
   FLAGS_logtostderr = true;
   FLAGS_minloglevel = 1; // WARNING
   google::InitGoogleLogging(progName);
}

static void ShutdownLogging()
{
   google::ShutdownGoogleLogging();
}
