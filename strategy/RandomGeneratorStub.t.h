#pragma once

class RandomGeneratorStub
{
   public:
      static double random;
      double next()
      {
         return random;
      }
};

double RandomGeneratorStub::random = 0.0;
