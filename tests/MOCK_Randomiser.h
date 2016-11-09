#ifndef SUPERBUBBLES_RANDOMISER_MOCK_H
#define SUPERBUBBLES_RANDOMISER_MOCK_H

#include "../src/tools/Randomiser.h"
#include "gmock/gmock.h"

class MOCK_Randomiser : public genomeMaker::Randomiser {
  public:
    MOCK_METHOD0( getRand, unsigned long() );
};

#endif //SUPERBUBBLES_RANDOMISER_MOCK_H
