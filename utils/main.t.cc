#include <gtest/gtest.h>
#include "Logging.h"


int main(int argc, char* argv[])
{
   // switch INFO logging off
   ConfigureNullLogging(argv[0]);

   testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
