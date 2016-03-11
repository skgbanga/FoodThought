#include "gtest/gtest.h"
#include "TestStrategy.t.h"
#include "FCFSStrategy.hpp"

class TestFCFSStrategy : public TestStrategy<strategy::FCFSStrategy>
{
   public:
      using Base = TestStrategy<strategy::FCFSStrategy>;
      using Base::addNewClient;
      using Base::donate;
      using Base::request;
      using Base::clients;
   public:
      bool m_result {false};
      std::string m_resultString {""};
};

TEST_F(TestFCFSStrategy, donateWithoutAdding)
{
   std::tie(m_result, m_resultString) = donate(clients()[0], 42.42);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("frodo doesn't exist", m_resultString.c_str());
}

TEST_F(TestFCFSStrategy, simpleDonate)
{
   ASSERT_TRUE(addNewClient(clients()[0]));
   std::tie(m_result, std::ignore) = donate(clients()[0], 42.42);
   EXPECT_TRUE(m_result);
}

TEST_F(TestFCFSStrategy, simpleRequestDenied)
{
   ASSERT_TRUE(addNewClient(clients()[0]));
   std::tie(m_result, m_resultString) = request(clients()[0], 30);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("requested amount too large", m_resultString.c_str());
}

TEST_F(TestFCFSStrategy, donateRequestMultiple)
{
   ASSERT_TRUE(addNewClient(clients()[0])); // frodo
   ASSERT_TRUE(addNewClient(clients()[1])); // sam
   ASSERT_TRUE(addNewClient(clients()[2])); // merry

   donate(clients()[0], 30);
   donate(clients()[1], 20);
   donate(clients()[2], 10);

   ASSERT_TRUE(addNewClient(clients()[3])); // pippin
   std::tie(m_result, m_resultString) = request(clients()[3], 45);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("merry:10 sam:20 frodo:15 ", m_resultString.c_str());

   std::tie(m_result, m_resultString) = request(clients()[3], 15);
   EXPECT_TRUE(m_result);
   EXPECT_STREQ("frodo:15 ", m_resultString.c_str());

   std::tie(m_result, m_resultString) = request(clients()[3], 10);
   EXPECT_FALSE(m_result);
   EXPECT_STREQ("requested amount too large", m_resultString.c_str());
}
