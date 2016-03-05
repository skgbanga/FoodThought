#pragma once

#include <unordered_map>

namespace Palantir
{
   enum class Token
   {
      DONATE,
      REQUEST
   };

   using TokenToStrMapType = std::unordered_map<Token, const char*>;
   using StrToTokenMapType = std::unordered_map<const char*, Token>;

   extern const char* NamePrefix;
   extern TokenToStrMapType TokenStrMap;
   extern StrToTokenMapType StrToTokenMap;
}
