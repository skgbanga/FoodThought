#include "DefaultStrategy.h"

#include <glog/logging.h>
#include <sstream>

namespace strategy
{
   bool StrategyBase::configure(const ConfigObject& config)
   {
      return true;
      //const char* filename = config.get("client_store");
      //std::ifstream file (filename);
      //if (!file)
      //{
         //std::cerr << "unable to open" << filename << std::endl;
         //return;
      //}

      //std::string line = "";
      //std::getline(file, line); // header - name,balance
   }

   bool StrategyBase::addNewClient(const std::string& name)
   {
      auto result = m_clientData.find(name);
      if (result == m_clientData.end())
      {
         auto inserted = false;
         std::tie(std::ignore, inserted) = m_clientData.emplace(name, std::make_unique<ClientInfo>());
         LOG_IF(INFO, inserted) << "inserted " << name << " in the client map";
         return inserted;
      }
      return true;
   }

   std::string StrategyBase::extractRequestedAmount(double requestedamount)
   {
      std::stringstream ss;
      for (auto& client : m_clientData)
      {
         auto& clientunused = client.second->m_unused; // reference
         if (clientunused == 0)
            continue;
         auto fulfilled = std::min(requestedamount, clientunused);
         ss << client.first << ":" << fulfilled << " ";
         requestedamount -= fulfilled;
         clientunused -= fulfilled;
         if (requestedamount == 0)
            break;
      }
      LOG(INFO) << "served request with " << ss.str();
      return ss.str();
   }

   bool StrategyBase::doesClientExist(const std::string& name)
   {
      auto found = m_clientData.find(name);
      return found != m_clientData.end();
   }

   bool StrategyBase::addToClientRequest(const std::string& name, double requestedAmount)
   {
      if (not doesClientExist(name))
         return false;

      auto& clientInfo = m_clientData.at(name);
      clientInfo->m_globalBalance -= requestedAmount;
      LOG(INFO) << name << " requested " << requestedAmount;
      return true;
   }

   bool StrategyBase::addToClientDonation(const std::string& name, double donatedAmount)
   {
      if (not doesClientExist(name))
         return false;

      auto& clientInfo = m_clientData.at(name);
      clientInfo->m_unused += donatedAmount;
      clientInfo->m_globalBalance += donatedAmount;
      LOG(INFO) << name << " donated " << donatedAmount;
      return true;
   }
}
