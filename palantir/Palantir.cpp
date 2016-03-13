#include "Palantir.h"
#include <string>

namespace Palantir
{
   TokenToStrMapType TokenStrMap =
   {
      { Token::NAME,    "name"    },
      { Token::QUERY,   "query"   },
      { Token::DONATE,  "donate"  },
      { Token::REQUEST, "request" }
   };

   StrToTokenMapType StrToTokenMap =
   {
      { "name", Token::NAME        },
      { "query", Token::QUERY      },
      { "donate",  Token::DONATE   },
      { "request", Token::REQUEST  }
   };
}
