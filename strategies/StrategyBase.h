#pragma once

#include <cstddef>

class StrategyBase
{
   public:
      // these two functions need to be defined by each strategy deriving this
      virtual std::size_t request(const char* name, const char* input, char* output) = 0;
      virtual std::size_t donate(const char* name, const char* input, char* output) = 0;

   public:
      bool addNewClient(const char* name);
};
