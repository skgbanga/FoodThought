#include <gtest/gtest.h>
#include "utils/ConfigObject.h"
#include "TestStrategy.t.h"
#include "MerlinStrategy.hpp"
#include "RandomGeneratorStub.t.h"


class TestMerlinStrategy : public TestStrategy<strategy::MerlinStrategy<RandomGeneratorStub> >
{
   public:
      TestMerlinStrategy()
      {
         ConfigObject config("testData/test.ini");
         initialize(config);
      }
   public:
      bool m_result {false};
      std::string m_resultString {""};
};

TEST_F(TestMerlinStrategy, simpleRequestDenied)
{
   donate("frodo", 5);
   RandomGeneratorStub::random = 0.13;

   std::tie(m_result, m_resultString) = request("pippin", 3);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("Sorry, not selected for allocation", m_resultString.c_str());
}

TEST_F(TestMerlinStrategy, simpleRequestAccepted)
{
   donate("frodo", 5);
   RandomGeneratorStub::random = 0.11;

   std::tie(m_result, m_resultString) = request("pippin", 3);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("frodo:3 ", m_resultString.c_str());
}

TEST_F(TestMerlinStrategy, checkPriority)
{
   donate("frodo", 5);
   RandomGeneratorStub::random = 0.20;

   // make request from a low priority guy
   std::tie(m_result, m_resultString) = request("pippin", 3);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("Sorry, not selected for allocation", m_resultString.c_str());

   // make request from a high priroty guy
   std::tie(m_result, m_resultString) = request("sam", 3);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("frodo:3 ", m_resultString.c_str());
}

TEST_F(TestMerlinStrategy, checkNoOfRequest)
{
   donate("frodo", 5);
   RandomGeneratorStub::random = 0.13;

   // 3 tries
   for (std::size_t i = 0; i < 3; i++)
   {
      std::tie(m_result, m_resultString) = request("pippin", 3);
      EXPECT_FALSE(m_result);
      EXPECT_STREQ("Sorry, not selected for allocation", m_resultString.c_str());
   }

   // 4th time should be a different reason
   std::tie(m_result, m_resultString) = request("pippin", 3);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("Requests exhausted!", m_resultString.c_str());
}

TEST_F(TestMerlinStrategy, checkTimeAdvantage)
{
   donate("frodo", 5);
   RandomGeneratorStub::random = 0.20;

   // make request while elapsed time is 0
   // so that you have 0 time advantage
   std::tie(m_result, m_resultString) = request("pippin", 3);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("Sorry, not selected for allocation", m_resultString.c_str());

   // increase by one hour
   // default value of m_timeout is 10 secs, so call onTimeout 360 times
   for (int i = 0; i < 360; i++)
   {
      onTimeout();
   }

   // make request again 
   std::tie(m_result, m_resultString) = request("pippin", 3);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("frodo:3 ", m_resultString.c_str());
}
