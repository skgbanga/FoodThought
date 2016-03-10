#pragma once

#include <random>

class DefaultRandomGenerator
{
   private:
      template <typename T>
      static constexpr auto RandNumGenerator = std::generate_canonical<double, 10, T>;
      using RandNumEngineType = std::mt19937;
      using RandNumSeedType   = std::random_device;

      RandNumSeedType m_randSeed {};
      RandNumEngineType m_randEngine {m_randSeed()};

   public:
      double next()
      {
         return RandNumGenerator<RandNumEngineType>(m_randEngine);
      }
};
