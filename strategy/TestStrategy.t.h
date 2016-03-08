#include "gtest/gtest.h"


// Test Strategy is templatized on a strategy and extends testing::Test
// Its only purpose is to foward request, donate calls to strategy on which
// it is templatzed on.


template <typename Strategy>
class TestStrategy : public testing::Test
{
   public:
      bool addNewClient(const std::string& name)
      {
         return m_strategy.addNewClient(name);
      }

      template <typename... Args>
      typename Strategy::RequestReturnType request(Args&&... args)
      {
         return m_strategy.request(std::forward<Args>(args)...);
      }

      template <typename... Args>
      typename Strategy::DonateReturnType donate(Args&&... args)
      {
         return m_strategy.donate(std::forward<Args>(args)...);
      }

      const std::vector<std::string>& clients()
      {
         return m_clients;
      }
   private:
      Strategy m_strategy {};
      std::vector<std::string> m_clients {"frodo", "sam", "merry", "pippin"};
};
