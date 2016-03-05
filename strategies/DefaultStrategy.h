#pragma once

#include "StrategyBase.h"

class DefaultStrategy : public StrategyBase
{
   public:
      std::size_t request(const char* name, const char* input, char* output) override;
      std::size_t donate(const char* name, const char* input, char* output) override;
};
