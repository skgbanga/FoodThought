#pragma once

#include "MerlinStrategy.hpp"
#include "DefaultRandomGenerator.h"

namespace strategy
{
   class DefaultMerlinStrategy : public MerlinStrategy<DefaultRandomGenerator>
   {  };
}
