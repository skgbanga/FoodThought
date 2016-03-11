#pragma once

#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <glog/logging.h>

#include "StrategyBaseHelper.hpp"

namespace strategy
{
   class FCFSStrategy : public StrategyBaseHelper<FCFSStrategy>
   {
      public:
         using Base = StrategyBaseHelper<FCFSStrategy>;
         using Base::Base;

         RequestReturnType request(const std::string& name, double amount) override;
         DonateReturnType donate(const std::string& name, double amount) override;
      private:
         double m_globalUnused {0};
   };
}
