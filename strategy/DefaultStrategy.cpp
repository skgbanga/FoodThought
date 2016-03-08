#include "DefaultStrategy.h"

namespace strategy
{
   DefaultStrategy::RequestReturnType
   DefaultStrategy::request(const std::string& name, double requestedAmount)
   {
      if (requestedAmount == 0)
         return std::make_pair(false, "0 requested amount");

      if (requestedAmount > m_globalUnused)
         return std::make_pair(false, "requested amount too large");

      if (not addToClientRequest(name, requestedAmount))
         return std::make_pair(false, "couldn't add to client " + name);

      m_globalUnused -= requestedAmount;
      return std::make_pair(true, extractRequestedAmount(requestedAmount));
   }

   DefaultStrategy::DonateReturnType
   DefaultStrategy::donate(const std::string& name, double donatedAmount)
   {
      if (donatedAmount == 0)
         return std::make_pair(false, "0 donated amount");

      if (not addToClientDonation(name, donatedAmount))
         return std::make_pair(false, "couldn't add to client " + name);

      m_globalUnused += donatedAmount;
      return std::make_pair(true, "");
   }
}
