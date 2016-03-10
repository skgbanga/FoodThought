#pragma once

#include <cstdlib>
#include <vector>
#include <string>

namespace StringUtils
{
   static constexpr char CR = 13;
   static constexpr char NL = '\n';

   // If the character array contains a carriage return in the end, change that to new line and return
   // If neither carriage return nor new line character is present, truncate  the input to include
   // null character. Returns the new size of array (including new line character)
   std::size_t formatCharArray(char *input, std::size_t N);

   // tokensize the input of size N on delimiter, and put the token into vector vec 
   void tokenize(char* input, std::size_t N, std::vector<std::string>& vec, char delimiter);
}
