#pragma once

#include "MerlinStrategy.h"
#include "utils/ConfigObject.h"

#include <glog/logging.h>
#include <algorithm>

namespace strategy
{
   template <typename RandGenerator>
   MerlinStrategy<RandGenerator>::MerlinStrategy(const ConfigObject& config) : Base(config)
   {
      m_alpha   = config.getDouble("STRATEGY.alpha", m_alpha);
      m_d1      = config.getDouble("STRATEGY.d1", m_d1);

      m_beta    = config.getDouble("STRATEGY.beta",  m_beta);
      m_d2      = config.getDouble("STRATEGY.d2", m_d2);

      m_gamma     = config.getDouble("STRATEGY.gamma",  m_gamma);
      m_timeout   = config.getInt("STRATEGY.timeout_s", m_timeout);
      m_totalTime = config.getInt("STRATEGY.total_time_s", m_totalTime);

      for (auto& clientInfo : getClientData())
      {
         auto& data = clientInfo.second;
         Status status = assignStatus(data->m_globalBalance);
         LOG(INFO) << "Setting status " << static_cast<int>(status) << " for " << clientInfo.first
                   << " for balance " << data->m_globalBalance;
         data->m_customDataStore.setStatus(status);
      }
   }

   template <typename RandGenerator>
   typename MerlinStrategy<RandGenerator>::Status
   MerlinStrategy<RandGenerator>::assignStatus(double globalBalance)
   {
      double balance = globalBalance / getNoBackDays();
      if (balance >= 4.0)  return Status::POWER_SEEDER;
      if (balance >= 2.0)  return Status::SEEDER;
      if (balance >= 0.0)  return Status::NEUTRAL;
      if (balance >= -2.0) return Status::LEECHER;
      else                 return Status::POWER_LEECHER;
   }

   template <typename RandGenerator>
   double MerlinStrategy<RandGenerator>::getStatusFactor(Status status)
   {
      auto factor = [a=m_alpha, d=m_d1](auto n)
      {
         return std::min(1.0, (a + n * (n + 1) * d/2));
      };
      switch (status)
      {
         case Status::POWER_SEEDER:    return factor(4);
         case Status::SEEDER:          return factor(3);
         case Status::NEUTRAL:         return factor(2);
         case Status::LEECHER:         return factor(1);
         case Status::POWER_LEECHER:   return factor(0);
      }
   }

   template <typename RandGenerator>
   double MerlinStrategy<RandGenerator>::getRequestedMoneyFactor(double requestedMoney)
   {
      auto factor = [a=m_beta, d=m_d2](auto n)
      {
         return std::min(1.0, (a + n * (n + 1) * d/2));
      };
      if (requestedMoney <= 1.0) return factor(4);
      if (requestedMoney <= 2.0) return factor(3);
      if (requestedMoney <= 3.0) return factor(2);
      if (requestedMoney <= 4.0) return factor(1);
      else                       return factor(0);
   }

   template <typename RandGenerator>
   double MerlinStrategy<RandGenerator>::getTimeFactor()
   {
      return std::exp(m_gamma * (m_timeElapsed - m_totalTime));
   }

   template <typename RandGenerator>
   typename MerlinStrategy<RandGenerator>::RequestReturnType
   MerlinStrategy<RandGenerator>::request(const std::string& name, double requestedAmount)
   {
      if (not doesClientExist(name))
         return std::make_pair(false, name + "doesn't exist");

      // sanity check
      if (requestedAmount == 0)
         return std::make_pair(false, "0 requested amount");

      // avoid entertatining repeated requests!
      auto& data = getClientData().at(name);
      if (data->m_customDataStore.requestsExhausted())
         return std::make_pair(false, "Requests exhausted!");
      data->m_customDataStore.incrementRequests();

      if (requestedAmount > m_globalUnused)
         return std::make_pair(false, "requested amount too large");

      Status status = data->m_customDataStore.getStatus();
      double f1 = getStatusFactor(status);
      double f2 = getRequestedMoneyFactor(requestedAmount);
      double f3 = getTimeFactor();
      double decProb =  getDecisionFactor(f1, f2, f3);

      double random = m_rand.next();
      LOG(INFO) << "Making a decision on - StatusFactor:" << f1
                << " MoneyFactor: " << f2
                << " TimeFactor "   << f3
                << " DecisionFactor: " << decProb
                << " randomNumber: " << random;

      if (random > decProb)
         return std::make_pair(false, "Sorry, not selected for allocation");

      addToClientRequest(name, requestedAmount);
      m_globalUnused -= requestedAmount;
      return std::make_pair(true, extractRequestedAmount(requestedAmount));
   }

   template <typename RandGenerator>
   double MerlinStrategy<RandGenerator>::getDecisionFactor(double statusFactor,
                                                           double reqMoneyFactor,
                                                           double timeFactor)
   {
      return std::min(1.0, (statusFactor * reqMoneyFactor + timeFactor));
   }


   template <typename RandGenerator>
   typename MerlinStrategy<RandGenerator>::DonateReturnType
   MerlinStrategy<RandGenerator>::donate(const std::string& name, double donatedAmount)
   {
      if (not doesClientExist(name))
         return std::make_pair(false, name + "doesn't exist");

      if (donatedAmount == 0)
         return std::make_pair(false, "0 donated amount");

      addToClientDonation(name, donatedAmount);
      m_globalUnused += donatedAmount;
      return std::make_pair(true, "");
   }

   template <typename RandGenerator>
   void MerlinStrategy<RandGenerator>::onTimeout()
   {
      m_timeElapsed += m_timeout;
   }

   template <typename RandGenerator>
   std::string MerlinStrategy<RandGenerator>::query()
   {
      return std::to_string(m_globalUnused);
   }
}
