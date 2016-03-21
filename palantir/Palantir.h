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

   struct CustomHash
   {
      std::size_t operator()(const Token& token) const
      {
         return static_cast<int>(token);
      }
   };
   using TokenToStrMapType = std::unordered_map<Token, std::string, CustomHash>;
   using StrToTokenMapType = std::unordered_map<std::string, Token>;

   extern TokenToStrMapType TokenStrMap;
   extern StrToTokenMapType StrToTokenMap;
}
