#pragma once

#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <glog/logging.h>

#include "StrategyBaseHelper.hpp"

namespace strategy
{
   // FCFS is First Come First Serve strategy. It maintains a global
   // pool of available money and allocate it in FCFS manner

   class FCFSStrategy : public StrategyBaseHelper<FCFSStrategy>
   {
      public:
         using Base = StrategyBaseHelper<FCFSStrategy>;
         using Base::Base;

         RequestReturnType request(const std::string& name, double amount) override;
         DonateReturnType donate(const std::string& name, double amount) override;
         std::string query() override;
      private:
         double m_globalUnused {0};
   };
}
