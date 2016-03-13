#pragma once

#include "FCFSStrategy.h"

namespace strategy
{
   FCFSStrategy::RequestReturnType
   FCFSStrategy::request(const std::string& name, double requestedAmount)
   {
      if (not doesClientExist(name))
         return std::make_pair(false, name + "doesn't exist");

      if (requestedAmount == 0)
         return std::make_pair(false, "0 requested amount");

      if (requestedAmount > m_globalUnused)
         return std::make_pair(false, "requested amount too large");

      addToClientRequest(name, requestedAmount);
      m_globalUnused -= requestedAmount;
      return std::make_pair(true, extractRequestedAmount(requestedAmount));
   }

   FCFSStrategy::DonateReturnType
   FCFSStrategy::donate(const std::string& name, double donatedAmount)
   {
      if (not doesClientExist(name))
         return std::make_pair(false, name + " doesn't exist");

      if (donatedAmount == 0)
         return std::make_pair(false, "0 donated amount");

      addToClientDonation(name, donatedAmount);
      m_globalUnused += donatedAmount;
      return std::make_pair(true, "");
   }

   std::string FCFSStrategy::query()
   {
      return std::to_string(m_globalUnused);
   }
}
