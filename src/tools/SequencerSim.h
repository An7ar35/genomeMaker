#ifndef GENOMEMAKER_SEQUENCERSIM_H
#define GENOMEMAKER_SEQUENCERSIM_H

#include <iostream>
#include <cmath>
#include <ctgmath>

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
        uint64_t calcErrorUpperBound( const uint64_t &reads_total,
                                      const double &error_rate ) const;
        std::stack<uint64_t> createErrorStack( const uint64_t &reads_total,
                                               const uint64_t &errors );
        uint64_t calcChunkReads( const uint64_t &genome_size,
                                 const uint64_t &reads_total,
                                 const uint64_t &genome_chunks,
                                 const size_t &chunk_size ) const;
        size_t calcMaxIndex( const std::streamsize &current_buffer_size,
                             const std::streamsize &next_buffer_size,
                             const size_t &read_length ) const;
        bool sequenceGenome( const size_t &read_length,
                             const size_t &read_depth,
                             const uint64_t &reads_total,
                             const uint64_t &erroneous_reads );
        bool sequenceGenomeChunk( const size_t &read_length,
                                  const uint64_t &read_count,
                                  std::stack<uint64_t> &erroneous_read_indices,
                                  Buffers &buffer );
        //Private variables
        static const size_t _LINE_SIZE = 71; //per line max char write in sequencer file output
        eadlib::io::FileReader &_reader;
        eadlib::io::FileWriter &_writer;
        Randomiser &_read_randomiser;
        Randomiser &_error_randomiser;
        uint64_t _total_reads_completed;
        uint64_t _total_read_errors;
    };
}


#endif //GENOMEMAKER_SEQUENCERSIM_H
