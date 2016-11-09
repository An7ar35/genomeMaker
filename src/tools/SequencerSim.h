#ifndef GENOMEMAKER_SEQUENCERSIM_H
#define GENOMEMAKER_SEQUENCERSIM_H

#include "eadlib/logger/Logger.h"
#include "eadlib/io/FileReader.h"
#include "eadlib/io/FileWriter.h"

#include "Randomiser.h"

namespace genomeMaker {
    class SequencerSim {
      public:
        SequencerSim( eadlib::io::FileReader &reader,
                      eadlib::io::FileWriter &writer,
                      const size_t &read_length,
                      const size_t &number_of_reads,
                      const double &error_rate );
        ~SequencerSim();
        bool start();
        size_t getReadLength() const;
        size_t getTotalReads() const;
        double getErrorRate() const;

      private:
        eadlib::io::FileReader &_reader;
        eadlib::io::FileWriter &_writer;
        size_t _read_length;
        size_t _total_reads;
        double _error_rate;
    };
}


#endif //GENOMEMAKER_SEQUENCERSIM_H
