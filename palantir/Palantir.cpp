#include "Palantir.h"


namespace Palantir
{

   const char* NamePrefix = "NAME=";

   TokenToStrMapType TokenStrMap =
   {
      {Token::DONATE,  "DONATE"},
      {Token::REQUEST, "REQUEST"}
   };

   StrToTokenMapType StrToTokenMap =
   {
      {"DONATE",  Token::DONATE},
      {"REQUEST", Token::REQUEST}
   };
}
