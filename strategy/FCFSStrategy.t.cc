#include "gtest/gtest.h"
#include "TestStrategy.t.h"
#include "FCFSStrategy.hpp"

class TestFCFSStrategy : public TestStrategy<strategy::FCFSStrategy>
{
   public:
      TestFCFSStrategy()
      {
         ConfigObject config("testData/test.ini");
         initialize(config);
      }
      bool m_result {false};
      std::string m_resultString {""};
};

TEST_F(TestFCFSStrategy, donateWithoutAdding)
{
   std::tie(m_result, m_resultString) = donate("bilbo", 42.42);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("bilbo doesn't exist", m_resultString.c_str());
}

TEST_F(TestFCFSStrategy, simpleDonate)
{
   ASSERT_TRUE(addNewClient("bilbo"));
   std::tie(m_result, std::ignore) = donate("bilbo", 42.42);
   EXPECT_TRUE(m_result);
}

TEST_F(TestFCFSStrategy, simpleRequestDenied)
{
   ASSERT_TRUE(addNewClient("bilbo"));
   std::tie(m_result, m_resultString) = request("bilbo", 30);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("requested amount too large", m_resultString.c_str());
}

TEST_F(TestFCFSStrategy, donateRequestMultiple)
{
   // frodo, sam, merry were added in config_store file (testData/test.ini)
   donate("frodo", 30);
   donate("sam",   20);
   donate("merry", 10);

   ASSERT_TRUE(addNewClient("pippin")); // pippin
   std::tie(m_result, m_resultString) = request("pippin", 45);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("merry:10 sam:20 frodo:15 ", m_resultString.c_str());

   std::tie(m_result, m_resultString) = request("pippin", 15);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("frodo:15 ", m_resultString.c_str());

   std::tie(m_result, m_resultString) = request("pippin", 10);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("requested amount too large", m_resultString.c_str());
}
