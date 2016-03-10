#include <iostream>

#include "utils/Logging.h"
#include "utils/ConfigObject.h"
#include "server/Server.h"

int main(int argc, char* argv[]) {

   if (argc != 2)
   {
      std::cerr << "Usage: ./program_name config.txt" << std::endl;
      return -1;
   }

   const char* programName = argv[0];
   const char* configFile = argv[1];

   // build the config file
   ConfigObject config(configFile);

   // first thing, configure logging
   ConfigureLogging(programName, config);

   // Initialize/run FoodThought Server
   if (not Server::initialize(config))
      return -1;

   Server::run();

   return 0;
}
