#include "logger/log.h"

int main(int, char* argv[])
{
   //FILE* pFile = std::fopen("arsenal.log", "a");
   //Output2FILE::Stream() = pFile;
   FILELog::ReportingLevel() = LogLevel::INFO;
   const int count = 3;
   FILE_LOG(LogLevel::INFO) << "A loop with " << count << " iterations";
   for (int i = 0; i != count; ++i)
   {
      FILE_LOG(LogLevel::WARNING) << "the counter i = " << i;
   }
   return 0;
}
