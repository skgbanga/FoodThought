#include "server/DefaultTypeDict.h"
#include "utils/logging.h"
#include "server/Server.h"

// Use your own type dict here (which might contain a different strategy)
using TypeDict = DefaultTypeDict;

int main(int argc, char* argv[]) {

   // first thing, configure logging
   ConfigureLogging(argv[0]);

   // Initialize FoodThought Server
   Server::Initialize();
   Server::run();
}
