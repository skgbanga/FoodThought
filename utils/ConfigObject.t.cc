#include <gtest/gtest.h>
#include "ConfigObject.h"
#include <string>


TEST(ConfigObjectTest, readDollarVar)
{
   ConfigObject config("test.ini");
   std::string val1 = config.getString("A.var1", "");
   std::string val2 = config.getString("A.var2", "");
   std::string val3 = config.getString("A.var3", "");

   EXPECT_STREQ("Jerry", val1.c_str());
   EXPECT_STREQ("TomAndJerry", val2.c_str());
   EXPECT_STREQ("TomAndJerryAndSpike", val3.c_str());
}
