#pragma once

#include <string>
#include <tuple>

// This is the abstract base class for all strategies which are used by ClientHandler
namespace strategy
{
   class StrategyBase
   {
      public:
         ~StrategyBase() {  }

         // returns false only if we tried to add a client and couldn't.
         // return true if the name if already present
         virtual bool addNewClient(const std::string& name) = 0;

         // shutdown/cleanup function to be called at the end. (against RAII)
         // This can't be put in the destructor because it is part of the static
         // class which gets destroyed at the end of the overall process. At that stage
         // things like logger and others are not present
         virtual bool shutdown() = 0;

         // onTimeout callbacks from server
         virtual void onTimeout() = 0;

         // queries the amount of money available in the global pool
         std::string virtual query() = 0;

         using RequestReturnType = std::tuple<bool, std::string>;
         using DonateReturnType  = std::tuple<bool, std::string>;

         // For request - tuple's first argument tells whether request was successful or not.
         //      - if successful, a string of who helped and by how much
         //      - if not, reason (usually no money, or unknown name)
         // For donate - tuple's first arguments tells whether donation was succssful or not.
         //      - if successful, string argument to be ignored
         //      - if not, reason (usually unknown name)
         virtual RequestReturnType request(const std::string& name, double amount) = 0;
         virtual DonateReturnType donate(const std::string& name, double amount) = 0;
   };
}
