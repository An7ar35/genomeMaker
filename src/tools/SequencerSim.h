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
                      Randomiser &randomiser,
                      const size_t &read_length,
                      const size_t &read_depth,
                      const double &error_rate );
        ~SequencerSim();
        bool start();
        size_t getReadLength() const;
        size_t getReadDepth() const;
        double getErrorRate() const;
        uint64_t getReadCount() const;

      private:
        //Private methods
        uint64_t calcReadCount( const std::streampos &genome_size,
                                const size_t &read_length,
                                const size_t &read_depth ) const;
        //Private variables
        eadlib::io::FileReader &_reader;
        eadlib::io::FileWriter &_writer;
        Randomiser &_randomiser;
        size_t   _read_length;
        size_t   _read_depth;
        uint64_t _read_count;
        double   _error_rate;
    };
}


#endif //GENOMEMAKER_SEQUENCERSIM_H
