#pragma once

#include "MerlinStrategy.h"
#include "utils/ConfigObject.h"

#include <glog/logging.h>

namespace strategy
{
   template <typename RandGenerator>
   MerlinStrategy<RandGenerator>::MerlinStrategy(const ConfigObject& config) : Base(config)
   {
      m_alpha   = config.getInt("STRATEGY.alpha", 20);
      m_beta    = config.getInt("STRATEGY.beta",  20);
      m_gamma   = config.getInt("STRATEGY.gamma",  1);
      m_timeout = config.getInt("STRATEGY.timeout", 10);

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
      auto probability = [factor=m_alpha](auto mult)
      {
         return (100.0 - mult * factor)/100;
      };
      switch (status)
      {
         case Status::POWER_SEEDER:    return probability(0.0);
         case Status::SEEDER:          return probability(1.0);
         case Status::NEUTRAL:         return probability(2.0);
         case Status::LEECHER:         return probability(3.0);
         case Status::POWER_LEECHER:   return probability(4.0);
      }
   }

   template <typename RandGenerator>
   double MerlinStrategy<RandGenerator>::getRequestedMoneyFactor(double requestedMoney)
   {
      auto probability = [factor=m_beta](auto mult)
      {
         return (100.0 - mult * factor)/100;
      };
      if (requestedMoney <= 1.0) return probability(0.0);
      if (requestedMoney <= 2.0) return probability(1.0);
      if (requestedMoney <= 3.0) return probability(2.0);
      if (requestedMoney <= 4.0) return probability(3.0);
      else                       return probability(4.0);
   }

   template <typename RandGenerator>
   double MerlinStrategy<RandGenerator>::getTimeFactor()
   {
      return m_gamma * m_timeElapsed / LifeTime;
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
      double p1 = getStatusFactor(status);
      double p2 = getRequestedMoneyFactor(requestedAmount);
      double p3 = getTimeFactor();
      double decProb =  getDecisionFactor(p1, p2, p3);

      double random = m_rand.next();
      LOG(INFO) << "Making a decision on - StatusFactor:" << p1
                << " MoneyFactor: " << p2
                << " TimeFactor "   << p3
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
      // can result in a number greater than 1
      // in which case we have reached a point in which we will start accepting
      // all requests
      return (statusFactor * reqMoneyFactor + timeFactor);
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
}
