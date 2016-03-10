#include "DefaultStrategy.h"
#include "utils/ConfigObject.h"
#include "utils/StringUtils.h"

#include <glog/logging.h>
#include <sstream>
#include <iostream>

#include <boost/tokenizer.hpp>

namespace strategy
{
   bool StrategyBase::initialize(const ConfigObject& config)
   {
      const std::string& fileName = config.getString("COMMON.client_store");
      std::ifstream file (fileName);
      if (!file)
      {
         LOG(ERROR) << "Unable to open file " << fileName;
         return false;
      }

      std::string line = "";
      std::getline(file, line); // header - name,balance

      while (std::getline(file, line))
      {
         using Tokenizer = boost::tokenizer<boost::char_separator<char>>;
         boost::char_separator<char> sep(",");
         Tokenizer tok(line, sep);
         auto iter = tok.begin();
         auto name = *iter;
         addNewClient(name);

         iter++;
         auto balance = std::stof(*iter);
         auto& clientInfo = m_clientData.at(name);
         clientInfo->m_globalBalance = balance;
         LOG(INFO) << name << " has initial balance " << balance;
      }

      return true;
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
