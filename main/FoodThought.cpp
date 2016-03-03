#include "utils/Logging.h"
#include "server/Server.h"

int main(int argc, char* argv[]) {

   // first thing, configure logging
   ConfigureLogging(argv[0]);

   // Initialize FoodThought Server
   Server::Initialize();
   Server::run();
}
