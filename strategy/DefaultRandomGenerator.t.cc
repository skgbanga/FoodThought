#include "gtest/gtest.h"
#include "DefaultRandomGenerator.h"

TEST(DefaultRandomGenerator, SimpleTest)
{
   DefaultRandomGenerator rand;
   double random = rand.next();
   ASSERT_LE(0, random);
   ASSERT_GT(1, random);
}
