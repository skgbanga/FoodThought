#include <gtest/gtest.h>
#include "utils/ConfigObject.h"
#include "TestStrategy.t.h"
#include "MerlinStrategy.hpp"
#include "RandomGeneratorStub.t.h"


class TestMerlinStrategy : public TestStrategy<strategy::MerlinStrategy<RandomGeneratorStub> >
{
   public:
      using Base = TestStrategy<strategy::MerlinStrategy<RandomGeneratorStub> >;
      using Base::addNewClient;
      using Base::donate;
      using Base::request;
      using Base::clients;
   public:
      bool m_result {false};
      std::string m_resultString {""};
};

TEST_F(TestMerlinStrategy, Init)
{
   ConfigObject config("test.ini");
   ASSERT_TRUE(initialize(config));
}

TEST_F(TestMerlinStrategy, simpleRequestDenied)
{
   ConfigObject config("test.ini");
   ASSERT_TRUE(initialize(config));

   donate(clients()[0], 5);
   RandomGeneratorStub::random = 0.13;

   std::tie(m_result, m_resultString) = request(clients()[3], 3);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("Sorry, not selected for allocation", m_resultString.c_str());
}

TEST_F(TestMerlinStrategy, simpleRequestAccepted)
{
   ConfigObject config("test.ini");
   ASSERT_TRUE(initialize(config));

   donate(clients()[0], 5);
   RandomGeneratorStub::random = 0.11;

   std::tie(m_result, m_resultString) = request(clients()[3], 3);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("frodo:3 ", m_resultString.c_str());
}

TEST_F(TestMerlinStrategy, checkPriority)
{
   ConfigObject config("test.ini");
   ASSERT_TRUE(initialize(config));

   donate(clients()[0], 5);
   RandomGeneratorStub::random = 0.20;

   // make request from a low priority guy
   std::tie(m_result, m_resultString) = request(clients()[3], 3);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("Sorry, not selected for allocation", m_resultString.c_str());

   // make request from a high priroty guy
   std::tie(m_result, m_resultString) = request(clients()[1], 3);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("frodo:3 ", m_resultString.c_str());
}
