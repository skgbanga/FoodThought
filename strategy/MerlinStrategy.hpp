#pragma once

#include "MerlinStrategy.h"
#include "utils/ConfigObject.h"

#include <glog/logging.h>

namespace strategy
{
   template <typename RandGenerator>
   template <typename ConfigObject>
   bool MerlinStrategy<RandGenerator>::initialize(const ConfigObject& config)
   {
      m_alpha = config.getInt("STRATEGY.alpha", 20);
      m_beta  = config.getInt("STRATEGY.beta",  20);

      if (not Base::initialize(config))
      {
         LOG(ERROR) << "Unable to initalzize base strategy";
         return false;
      }

      for (auto& clientInfo : getClientData())
      {
         auto& data = clientInfo.second;
         data->m_customDataStore.setStatus(assignStatus(data->m_globalBalance));
      }

      return true;
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
         return (100 - mult * factor)/100;
      };
      switch (status)
      {
         case Status::POWER_SEEDER:    return probability(0);
         case Status::SEEDER:          return probability(1);
         case Status::NEUTRAL:         return probability(2);
         case Status::LEECHER:         return probability(3);
         case Status::POWER_LEECHER:   return probability(4);
      }
   }

   template <typename RandGenerator>
   double MerlinStrategy<RandGenerator>::getRequestedMoneyFactor(double requestedMoney)
   {
      auto probability = [factor=m_beta](auto mult)
      {
         return (100 - mult * factor)/100;
      };
      if (requestedMoney <= 1.0) return probability(0);
      if (requestedMoney <= 2.0) return probability(1);
      if (requestedMoney <= 3.0) return probability(2);
      if (requestedMoney <= 4.0) return probability(3);
      else                       return probability(4);
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
      if (not data->m_customDataStore.requestsExhausted())
         return std::make_pair(false, "Requests exhausted!");
      data->m_customDataStore.incrementRequests();

      if (requestedAmount > m_globalUnused)
         return std::make_pair(false, "requested amount too large");

      Status status = data->m_customDataStore.getStatus();
      double p1 = getStatusFactor(status);
      double p2 = getRequestedMoneyFactor(requestedAmount);
      double decProb =  getDecisionProbablity(p1, p2);

      double random = m_rand.next();
      LOG(INFO) << "Making a decision on - StatusFactor:" << p1
                << " MoneyFactor: " << p2
                << " DecisionProbablity: " << decProb
                << " randomNumber: " << random;

      if (random > decProb)
         return std::make_pair(false, "Sorry, not selected for allocation");

      addToClientRequest(name, requestedAmount);
      m_globalUnused -= requestedAmount;
      return std::make_pair(true, extractRequestedAmount(requestedAmount));
   }

   template <typename RandGenerator>
   double MerlinStrategy<RandGenerator>::getDecisionProbablity(double statusFactor, double reqMoneyFactor)
   {
      return statusFactor * reqMoneyFactor;
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
}
