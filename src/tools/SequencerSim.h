#ifndef GENOMEMAKER_SEQUENCERSIM_H
#define GENOMEMAKER_SEQUENCERSIM_H

#include <iostream>

#include "eadlib/logger/Logger.h"
#include "eadlib/io/FileReader.h"
#include "eadlib/io/FileWriter.h"
#include "eadlib/cli/graphic/ProgressBar.h"

#include "Randomiser.h"
#include "../containers/Buffers.h"

namespace genomeMaker {
    class SequencerSim {
      public:
        SequencerSim( eadlib::io::FileReader &reader,
                      eadlib::io::FileWriter &writer,
                      genomeMaker::Randomiser &read_randomiser,
                      genomeMaker::Randomiser &error_randomiser );
        ~SequencerSim();
        bool start( const size_t &read_length,
                    const size_t &read_depth,
                    const double &error_rate );

      private:
        //Private methods
        uint64_t calcReadCount( const std::streampos &genome_size,
                                const size_t &read_length,
                                const size_t &read_depth ) const;
        size_t calcMaxIndex( const std::streamsize &current_buffer_size,
                             const std::streamsize &next_buffer_size,
                             const size_t &read_length ) const;
        bool sequenceGenome( const size_t &read_length,
                             const size_t &read_depth,
                             const double &error_rate,
                             const uint64_t &reads_total );
        bool sequenceGenomeChunk( const size_t &read_length,
                                  const uint64_t &read_count,
                                  uint64_t &total_reads_completed,
                                  Buffers &buffer );
        //Private variables
        static const size_t LINE_SIZE = 71; //per line max char write in sequencer file output
        eadlib::io::FileReader &_reader;
        eadlib::io::FileWriter &_writer;
        genomeMaker::Randomiser &_read_randomiser;
        genomeMaker::Randomiser &_error_randomiser;
    };
}


#endif //GENOMEMAKER_SEQUENCERSIM_H
