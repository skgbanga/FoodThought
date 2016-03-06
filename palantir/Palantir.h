#pragma once

#include <unordered_map>

namespace Palantir
{
   enum class Token
   {
      // Add api to query amount of money available
      DONATE,
      REQUEST
   };

   using TokenToStrMapType = std::unordered_map<Token, std::string>;
   using StrToTokenMapType = std::unordered_map<std::string, Token>;

   extern const std::string NamePrefix;
   extern TokenToStrMapType TokenStrMap;
   extern StrToTokenMapType StrToTokenMap;
}
