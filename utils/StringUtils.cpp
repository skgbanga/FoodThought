#include "StringUtils.h"

namespace StringUtils
{
   std::size_t formatCharArray(char *input, std::size_t N)
   {
      for (std::size_t i = 0; i < N; i++)
      {
         if ((input[i] == CR) || (input[i] == NL))
         {
            input[i] = '\0'; // early return
            return (i + 1);
         }
      }
      // truncate the input
      input[N - 1] = '\0';
      return N;
   }

   void tokenize(char* input, std::size_t N, std::vector<std::string>& vec, char delimiter)
   {
      std::string temp = "";
      for (std::size_t i = 0; i < N; i++)
      {
         if (input[i] == delimiter)
         {
            if (temp.size())
            {
               vec.push_back(temp);
               temp = "";
            }
         }
         else if ((input[i] == CR) || (input[i] == NL))
         {
            if (temp.size())
               vec.push_back(temp);
            return;
         }
         else
         {
            temp.push_back(input[i]);
         }
      }
   }
}
