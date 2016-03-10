#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <tuple>

namespace strategy
{
   template <typename CustomStrategy>
   class StrategyBase
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
         template <typename ConfigObject>
         bool initialize(const ConfigObject& config);
         // these two functions need to be defined by each strategy deriving this
         // For request - tuple's first argument tells whether request was successful or not.
         //      - if successful, a string of who helped and by how much
         //      - if not, reason (usually no money, or unknown name)
         // For donate - tuple's first arguments tells whether donation was succssful or not.
         //      - if successful, string argument to be ignored
         //      - if not, reason (usually unknown name)
         using RequestReturnType = std::tuple<bool, std::string>;
         using DonateReturnType  = std::tuple<bool, std::string>;

         virtual RequestReturnType request(const std::string& name, double amount) = 0;
         virtual DonateReturnType donate(const std::string& name, double amount) = 0;

         // returns false only if we tried to add a client and couldn't.
         // return true if the element if already present
         bool addNewClient(const std::string& name);

         // this gets called on shutdown
         void shutdown();
   };
}
