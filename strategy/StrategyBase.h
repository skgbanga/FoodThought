#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace strategy
{
   class StrategyBase
   {
      public:
         // these two functions need to be defined by each strategy deriving this
         virtual std::size_t request(const std::string& name,
                                     const std::vector<std::string>& tokens,
                                     char* output) = 0;
         virtual std::size_t donate(const std::string& name,
                                    const std::vector<std::string>& tokens,
                                    char* output) = 0;

         // returns false only if we tried to add a client and couldn't.
         // return true if the element if already present
         bool addNewClient(const std::string& name);

         // extract request amount from m_clientData and returns who donated how much
         // as return value in form of "name1:amount1 name2:amound2..." 
         std::string extractRequestedAmount(double requestedAmount);

         // deposit donatedAmount into name's client data
         void addToClientDonation(const std::string& name, double donatedAmount);

         // add requestedAmount into name's client data
         void addToClientRequest(const std::string& name, double requestedAmount);

         struct ClientInfo
         {
            double m_unused {0};
            double m_globalRequest {0};
            double m_globalDonate {0};
         };
         using ClientDataType = std::unordered_map<std::string, std::unique_ptr<ClientInfo>>;
         ClientDataType m_clientData;
   };
}
