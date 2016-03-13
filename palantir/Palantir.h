#pragma once

#include <unordered_map>

namespace Palantir
{
   enum class Token
   {
      NAME,
      QUERY,
      DONATE,
      REQUEST
   };

   using TokenToStrMapType = std::unordered_map<Token, std::string>;
   using StrToTokenMapType = std::unordered_map<std::string, Token>;

   extern TokenToStrMapType TokenStrMap;
   extern StrToTokenMapType StrToTokenMap;
}
