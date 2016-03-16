#pragma once

#include "StrategyBaseHelper.hpp"

namespace strategy
{
   // A more complete/fair/efficient strategy that takes into account 4 parameters
   // 1. Elapsed time (controlled by factor gamma, and totalTime(t))
   // 2. Your standing among seeders and leechers (controlled by factor alpha, and d1)
   // 3. The amount of money you are asking for (controlled by factor beta, and d2)
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
            bool requestsExhausted()      { return m_requests == MaxRequests; }
            void incrementRequests()      { m_requests++;      }
            void setStatus(Status status) { m_status = status; }
            Status getStatus()            { return m_status;   }
         };

      private:
         // time factors
         double m_gamma {0.0008};
         int m_timeout {10};
         int m_timeElapsed {0};
         int m_totalTime {10800}; // 3 hours


         // alpha and d1
         double m_alpha {0.2};
         double m_d1    {0.08};

         // beta and d2
         double m_beta {0.2};
         double m_d2   {0.08};

         double m_globalUnused {0};
         RandGenerator m_rand {};

         // Status is assigned on your past "food history"
         Status assignStatus(double balance);

         // controlled by alpha and d1
         double getStatusFactor(Status status);
         // controlled by beta and d2
         double getRequestedMoneyFactor(double requestedMoney);
         // controlled by gamma and total time
         double getTimeFactor();

         // controlled by above three factors
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
         std::string query() override;
   };
}
