#include <gtest/gtest.h>
#include "ConfigObject.h"
#include <string>
#include <stdlib.h>


TEST(ConfigObjectTest, readDollarVar)
{
   ConfigObject config("test.ini");
   auto val1 = config.getString("A.var1", "");
   auto val2 = config.getString("A.var2", "");
   auto val3 = config.getString("A.var3", "");

   EXPECT_STREQ("Jerry", val1.c_str());
   EXPECT_STREQ("TomAndJerry", val2.c_str());
   EXPECT_STREQ("TomAndJerryAndSpike", val3.c_str());
}

TEST(ConfigObject, readEnvVar)
{
   // set the env variable
   ASSERT_TRUE(!setenv("PUP", "Tyke", true));
   ConfigObject config("test.ini");
   auto val1 = config.getString("B.var1", "");

   EXPECT_STREQ("SpikeLovesTyke", val1.c_str());
}
