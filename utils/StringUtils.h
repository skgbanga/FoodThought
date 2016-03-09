#pragma once

#include <cstdlib>
#include <vector>
#include <string>

namespace StringUtils
{
   // If the character array contains a carriage return in the end, change that to new line and return
   // If neither carriage return nor new line character is present, truncate  the input to include
   // null character. Returns the new size of array (including new line character)
   // Ascii value of carriage return is 13.
   static constexpr char CR = 13;
   static constexpr char NL = '\n';

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

   // tokensize the input of size N on delimiter, and put the token into vector vec 
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
