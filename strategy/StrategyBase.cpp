#include "DefaultStrategy.h"
#include <glog/logging.h>

namespace strategy
{
   bool StrategyBase::addNewClient(const std::string& name)
   {
      auto result = m_clientData.find(name);
      if (result == m_clientData.end())
      {
         auto inserted = false;
         std::tie(std::ignore, inserted) = m_clientData.emplace(name, std::make_unique<ClientInfo>());
         LOG_IF(INFO, inserted) << "Inserted " << name << " in the client map";
         return inserted;
      }
      return true;
   }

   std::string StrategyBase::extractRequestedAmount(double requestedAmount)
   {
      std::stringstream ss;
      ss << "Thank these people! - ";
      for (auto& client : m_clientData)
      {
         auto& clientUnused = client.second->m_unused; // reference
         if (clientUnused == 0)
            continue;
         auto fulfilled = std::min(requestedAmount, clientUnused);
         ss << client.first << ":" << fulfilled << " ";
         requestedAmount -= fulfilled;
         clientUnused -= fulfilled;
         if (requestedAmount == 0)
            break;
      }
      LOG(INFO) << "Served request with " << ss.str();
      return ss.str();
   }


   void StrategyBase::addToClientRequest(const std::string& name, double requestedAmount)
   {
      auto& clientInfo = m_clientData.at(name);
      clientInfo->m_globalRequest += requestedAmount;
      LOG(INFO) << name << " requested " << requestedAmount;
   }

   void StrategyBase::addToClientDonation(const std::string& name, double donatedAmount)
   {
      auto& clientInfo = m_clientData.at(name);
      clientInfo->m_unused += donatedAmount;
      clientInfo->m_globalDonate += donatedAmount;
      LOG(INFO) << name << " donated " << donatedAmount;
   }
}
