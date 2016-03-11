#pragma once

#include "StrategyBaseHelper.h"
#include "utils/ConfigObject.h"
#include "utils/StringUtils.h"

#include <glog/logging.h>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <boost/tokenizer.hpp>

namespace strategy
{
   template <typename CustomStrategy>
   StrategyBaseHelper<CustomStrategy>::StrategyBaseHelper(const ConfigObject& config)
   {
      m_backDays = config.getInt("STRATEGY.back_days", 10);
      m_clientDataFileName = config.getString("COMMON.client_store", "");
      std::ifstream file (m_clientDataFileName);
      if (!file)
      {
         LOG(ERROR) << "Unable to open file " << m_clientDataFileName;
         throw std::invalid_argument("Couldn't initalize strategy!");
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
         clientInfo->m_globalBalance = balance * m_backDays;
         LOG(INFO) << "Loaded " << clientInfo->m_globalBalance << " for " << name;
      }
   }

   template <typename CustomStrategy>
   bool StrategyBaseHelper<CustomStrategy>::addNewClient(const std::string& name)
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

   template <typename CustomStrategy>
   std::string StrategyBaseHelper<CustomStrategy>::extractRequestedAmount(double requestedamount)
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

   template <typename CustomStrategy>
   bool StrategyBaseHelper<CustomStrategy>::doesClientExist(const std::string& name)
   {
      auto found = m_clientData.find(name);
      return found != m_clientData.end();
   }

   template <typename CustomStrategy>
   typename StrategyBaseHelper<CustomStrategy>::ClientDataType& StrategyBaseHelper<CustomStrategy>::getClientData()
   {
      return m_clientData;
   }

   template <typename CustomStrategy>
   std::size_t StrategyBaseHelper<CustomStrategy>::getNoBackDays()
   {
      return m_backDays;
   }

   template <typename CustomStrategy>
   void StrategyBaseHelper<CustomStrategy>::addToClientRequest(const std::string& name, double requestedAmount)
   {
      auto& clientInfo = m_clientData.at(name);
      clientInfo->m_globalBalance -= requestedAmount;
      LOG(INFO) << name << " requested " << requestedAmount;
   }

   template <typename CustomStrategy>
   void StrategyBaseHelper<CustomStrategy>::addToClientDonation(const std::string& name, double donatedAmount)
   {
      auto& clientInfo = m_clientData.at(name);
      clientInfo->m_unused += donatedAmount;
      clientInfo->m_globalBalance += donatedAmount;
      LOG(INFO) << name << " donated " << donatedAmount;
   }

   template <typename CustomStrategy>
   bool StrategyBaseHelper<CustomStrategy>::shutdown()
   {
      std::string dumpFile = m_clientDataFileName + ".today";
      std::ofstream ofs(dumpFile, std::ofstream::out);
      if (!ofs)
      {
         LOG(ERROR) << "Couldn't open " << dumpFile << " for dumping data!";
         return false;
      }
      ofs << "name,balance\n"; // write header first
      for (const auto& item : m_clientData)
      {
         double averageBalance = item.second->m_globalBalance / (m_backDays + 1);
         LOG(INFO) << "Dumping " << averageBalance << " " << item.first;
         ofs << item.first << "," << averageBalance << std::endl;
      }
      ofs.close();

      return true;
   }
}
