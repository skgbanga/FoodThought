#include "Palantir.h"
#include <string>

namespace Palantir
{
   const std::string NamePrefix = "name=";

   TokenToStrMapType TokenStrMap =
   {
      {Token::DONATE,  "donate"},
      {Token::REQUEST, "request"}
   };

   StrToTokenMapType StrToTokenMap =
   {
      {"donate",  Token::DONATE},
      {"request", Token::REQUEST}
   };
}
