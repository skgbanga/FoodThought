#pragma once

#include <sstream>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <unordered_map>
#include <iomanip>

// A primitive logger taken more or less from http://www.drdobbs.com/cpp/logging-in-c/201804215

enum class LogLevel
{
   ERROR,
   WARNING,
   INFO,
   DEBUG,
   ALL
};

using LogLevelStrMapType = std::unordered_map<LogLevel, const char*>;
static LogLevelStrMapType LogLevelStrMap {
   {LogLevel::ERROR, "ERROR"},
   {LogLevel::WARNING, "WARNING"},
   {LogLevel::INFO, "INFO"},
   {LogLevel::DEBUG, "DEBUG"},
   {LogLevel::ALL, "ALL"}
};

template <typename T>
class Log
{
   public:
      Log() {};
      virtual ~Log();
      Log(const Log&) = delete;
      Log& operator =(const Log&) = delete;

      static LogLevel& ReportingLevel();
      std::ostringstream& Get(LogLevel level = LogLevel::INFO);
   protected:
      std::ostringstream os;
};

template <typename T>
std::ostringstream& Log<T>::Get(LogLevel level)
{
   auto t = std::time(nullptr);
   os << "- " << std::put_time(std::localtime(&t), "%T");
   os << " " << LogLevelStrMap[level] << ": ";
   return os;
}

template <typename T>
Log<T>::~Log()
{
    os << "\n";
    T::Output((this->os).str());
}

template <typename T>
LogLevel& Log<T>::ReportingLevel()
{
    static LogLevel reportingLevel = LogLevel::INFO;
    return reportingLevel;
}

class Output2FILE
{
public:
    static FILE*& Stream();
    static void Output(const std::string& msg);
};

inline FILE*& Output2FILE::Stream()
{
    static FILE* pStream = stderr;
    return pStream;
}

inline void Output2FILE::Output(const std::string& msg)
{
    FILE* pStream = Stream();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
}

class FILELog : public Log<Output2FILE> {};

#define FILE_LOG(level) \
    if (level > FILELog::ReportingLevel() || !Output2FILE::Stream()) ;\
    else FILELog().Get(level)
