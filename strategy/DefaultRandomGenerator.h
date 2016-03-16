#pragma once

#include <random>

// DefaultRandomGenerator is based on mersenne twister algorithm with mersenne number = 19937
// Random numbers are uniformly choosen from range [0, 1) (with 10 bits of entropy)

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
