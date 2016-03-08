#include "gtest/gtest.h"
#include "TestStrategy.t.h"
#include "DefaultStrategy.h"
#include <vector>

class TestDefaultStrategy : public TestStrategy<strategy::DefaultStrategy>
{
   public:
      using Base = TestStrategy<strategy::DefaultStrategy>;
      using Base::addNewClient;
      using Base::donate;
      using Base::request;
      using Base::clients;
   public:
      bool m_status {false};
      std::string m_statusString {""};
};

TEST_F(TestDefaultStrategy, donateWithoutAdding)
{
   std::tie(m_status, m_statusString) = donate(clients()[0], 42.42);
   EXPECT_FALSE(m_status);
   EXPECT_STREQ("couldn't add to client frodo", m_statusString.c_str());
}

TEST_F(TestDefaultStrategy, simpleDonate)
{
   ASSERT_TRUE(addNewClient(clients()[0]));
   std::tie(m_status, std::ignore) = donate(clients()[0], 42.42);
   EXPECT_TRUE(m_status);
}

TEST_F(TestDefaultStrategy, simpleRequestDenied)
{
   ASSERT_TRUE(addNewClient(clients()[0]));
   std::tie(m_status, m_statusString) = request(clients()[0], 30);
   EXPECT_FALSE(m_status);
   EXPECT_STREQ("requested amount too large", m_statusString.c_str());
}

TEST_F(TestDefaultStrategy, donateRequestMultiple)
{
   ASSERT_TRUE(addNewClient(clients()[0])); // frodo
   ASSERT_TRUE(addNewClient(clients()[1])); // sam
   ASSERT_TRUE(addNewClient(clients()[2])); // merry

   donate(clients()[0], 30);
   donate(clients()[1], 20);
   donate(clients()[2], 10);

   ASSERT_TRUE(addNewClient(clients()[3])); // pippin
   std::tie(m_status, m_statusString) = request(clients()[3], 45);
   EXPECT_TRUE(m_status);
   EXPECT_STREQ("merry:10 sam:20 frodo:15 ", m_statusString.c_str());

   std::tie(m_status, m_statusString) = request(clients()[3], 15);
   EXPECT_TRUE(m_status);
   EXPECT_STREQ("frodo:15 ", m_statusString.c_str());

   std::tie(m_status, m_statusString) = request(clients()[3], 10);
   EXPECT_FALSE(m_status);
   EXPECT_STREQ("requested amount too large", m_statusString.c_str());
}
