#pragma once

#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <glog/logging.h>

#include "StrategyBase.h"

namespace strategy
{
   class DefaultStrategy : public StrategyBase
   {
      public:
         RequestReturnType request(const std::string& name, double amount) override;
         DonateReturnType donate(const std::string& name, double amount) override;
      private:
         double m_globalUnused {0};
   };
}
