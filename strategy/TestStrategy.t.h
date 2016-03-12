#include <gtest/gtest.h>
#include "utils/ConfigObject.h"

// Test Strategy is templatized on a strategy and extends testing::Test
// Its only purpose is to foward addNewClient, request, donate calls to strategy on which
// it is templatzed on.


template <typename Strategy>
class TestStrategy : public testing::Test
{
   public:
      void initialize(const ConfigObject& config)
      {
         m_upStrategy = std::make_unique<Strategy>(config);
      }

      void onTimeout()
      {
         m_upStrategy->onTimeout();
      }

      template <typename... Args>
      bool addNewClient(Args&&... args)
      {
         return m_upStrategy->addNewClient(std::forward<Args>(args)...);
      }

      template <typename... Args>
      typename Strategy::RequestReturnType request(Args&&... args)
      {
         return m_upStrategy->request(std::forward<Args>(args)...);
      }

      template <typename... Args>
      typename Strategy::DonateReturnType donate(Args&&... args)
      {
         return m_upStrategy->donate(std::forward<Args>(args)...);
      }

      const std::vector<std::string>& clients()
      {
         return m_clients;
      }
   private:
      std::unique_ptr<Strategy> m_upStrategy {nullptr};
      std::vector<std::string> m_clients {"frodo", "sam", "merry", "pippin", "bilbo"};
};
