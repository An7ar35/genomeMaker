#ifndef GENOMEMAKER_WRITER_MOCK_H
#define GENOMEMAKER_WRITER_MOCK_H

#include "../src/io/Writer.h"
#include "gmock/gmock.h"

class MOCK_Writer : public genomeMaker::Writer {
  public:
    MOCK_METHOD0( isOpen, bool );
};

#endif //GENOMEMAKER_WRITER_MOCK_H
