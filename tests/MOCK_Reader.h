#ifndef GENOMEMAKER_READER_MOCK_H
#define GENOMEMAKER_READER_MOCK_H

#include "../src/io/Reader.h"
#include "gmock/gmock.h"

class MOCK_Reader : public genomeMaker::Reader {
  public:
    MOCK_METHOD0( isOpen, bool );
};

#endif //GENOMEMAKER_READER_MOCK_H
