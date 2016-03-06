#pragma once

#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <glog/logging.h>

#include "StrategyBase.h"

class DefaultStrategy : public StrategyBase
{
   public:
      std::size_t request(const std::string& name,
                          const std::vector<std::string>& tokens,
                          char* output) override;
      std::size_t donate(const std::string& name,
                                 const std::vector<std::string>& tokens,
                                 char* output) override;
   private:
      double m_globalUnused {0};
};
