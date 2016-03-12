#pragma once

#include "StrategyBaseHelper.hpp"

namespace strategy
{
   // A more complete/fair/efficient strategy that takes into account 4 parameters
   // 1. The amount of money you are asking for (controlled by factor beta)
   // 2. Your standing among seeders and leechers (controlled by factor alpha)
   // 3. Elapsed time (controlled by factor gamma)
   // 4. Num of people higher than you in ordering who haven't yet ordered (TODO(sgupta))
   template <typename RandGenerator>
   class MerlinStrategy : public StrategyBaseHelper<MerlinStrategy<RandGenerator> >
   {
      private:
         enum class Status
         {
            POWER_SEEDER,
            SEEDER,
            NEUTRAL,
            LEECHER,
            POWER_LEECHER
         };

         struct Data
         {
            private:
            static constexpr std::size_t MaxRequests = 3;
            std::size_t m_requests {0};
            Status m_status {Status::NEUTRAL};

            public:
            bool requestsExhausted()
            {
               return m_requests == MaxRequests;
            }
            void incrementRequests()
            {
               m_requests++;
            }
            void setStatus(Status status)
            {
               m_status = status;
            }
            Status getStatus()
            {
               return m_status;
            }
         };

      private:
         static constexpr double LifeTime = 14400.0; // 4 hours
         int m_alpha {20};
         int m_beta  {20};
         int m_gamma {1};
         int m_timeout {10};
         int m_timeElapsed {0};
         double m_globalUnused {0};
         RandGenerator m_rand {};

         Status assignStatus(double balance);

         double getStatusFactor(Status status);
         double getRequestedMoneyFactor(double requestedMoney);
         double getTimeFactor();
         double getDecisionFactor(double p1, double p2, double p3);

      public:
         using DataStoreType = Data;
         using Base = StrategyBaseHelper<MerlinStrategy<RandGenerator> >;
         using Base::Base;
         using typename Base::RequestReturnType;
         using typename Base::DonateReturnType;
         using Base::getNoBackDays;
         using Base::getClientData;
         using Base::doesClientExist;
         using Base::addToClientRequest;
         using Base::extractRequestedAmount;
         using Base::addToClientDonation;

         explicit MerlinStrategy(const ConfigObject&) noexcept(false);

         RequestReturnType request(const std::string& name, double amount) override;
         DonateReturnType donate(const std::string& name, double amount) override;
         void onTimeout() override;
   };
}
