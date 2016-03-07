#include "DefaultStrategy.h"

namespace strategy
{
   std::size_t
   DefaultStrategy::request(const std::string& name,
                            const std::vector<std::string>& tokens,
                            char* output)
   {
      std::size_t written = 0;
      try
      {
         auto requestedAmount = std::atof(tokens[1].c_str());
         if (requestedAmount <= m_globalUnused)
         {
            m_globalUnused -= requestedAmount;
            addToClientRequest(name, requestedAmount);
            std::string result = extractRequestedAmount(requestedAmount);
            written = std::sprintf(output, "%s\n", result.c_str());
         }
         else
         {
            written = std::sprintf(output, "Sorry, only %f amount available\n", m_globalUnused);
         }
      }
      catch (std::out_of_range e)
      {
         written = std::sprintf(output, "%s!, I don't know you!\n", name.c_str());
      }
      return written;
   }

   std::size_t
   DefaultStrategy::donate(const std::string& name,
                           const std::vector<std::string>& tokens,
                           char* output)
   {
      std::size_t written = 0;
      try
      {
         auto donatedAmount = std::atof(tokens[1].c_str());
         m_globalUnused += donatedAmount;
         addToClientDonation(name, donatedAmount);
         written = std::sprintf(output, "Thanks %s for donating %f \n", name.c_str(), donatedAmount);
      }
      catch (std::out_of_range e)
      {
         written = std::sprintf(output, "%s!, I don't know you!\n", name.c_str());
      }
      return written;
   }
}
