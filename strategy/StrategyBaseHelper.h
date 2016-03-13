#pragma once

#include <unordered_map>
#include "StrategyBase.h"

class ConfigObject;

// Strategy Base helper makes it easy to write a strategy by providing some helper functions.
// It also defines three pure virtual methods
// - addNewClient(...)
// - shutdown(...)
// - onTimeout(...)
// However the following three remaining pure virtual methods still need to be defined by new strategies
// deriving this
// - request(...)
// - donate(...)
// - query(...)

namespace strategy
{
   template <typename CustomStrategy>
   class StrategyBaseHelper : public StrategyBase
   {
      private:
         struct ClientInfo
         {
            double m_unused {0};
            double m_globalBalance {0};
            typename CustomStrategy::DataStoreType m_customDataStore {};
         };
         using ClientDataType    = std::unordered_map<std::string, std::unique_ptr<ClientInfo>>;
         using DataStoreType     = bool; // place holder

         ClientDataType m_clientData {};
         std::string m_clientDataFileName {""};
         std::size_t m_backDays {10};
      protected:
         // extract request amount from m_clientData and returns who donated how much
         // as return value in form of "name1:amount1 name2:amound2..." 
         std::string extractRequestedAmount(double requestedAmount);

         // whether this client exist in the map or not
         bool doesClientExist(const std::string& name);

         // deposit donatedAmount into name's client data
         void addToClientDonation(const std::string& name, double donatedAmount);

         // add requestedAmount into name's client data
         void addToClientRequest(const std::string& name, double requestedAmount);

         // children can have access to client data
         ClientDataType& getClientData();

         // children have access to no of back days
         std::size_t getNoBackDays();
      public:
         explicit StrategyBaseHelper(const ConfigObject& config) noexcept(false);

         bool addNewClient(const std::string& name) override;
         bool shutdown() override;
         void onTimeout() override;
   };
}
