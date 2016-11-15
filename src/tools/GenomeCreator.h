#ifndef SUPERBUBBLES_CREATOR_H
#define SUPERBUBBLES_CREATOR_H

//TODO Full DNA base pair generation (GC, AT) at some point?
//TODO Full RNA base pair generation (GC, UA, CG) at some point?
//TODO implement completely random generation from of a set of given letters functionality in the future

#include <iostream>

#include "eadlib/logger/Logger.h"
#include "eadlib/io/FileWriter.h"
#include "eadlib/cli/graphic/ProgressBar.h"

#include "Randomiser.h"

namespace genomeMaker {
    class GenomeCreator {
      public:
        GenomeCreator( const Randomiser &randomiser, eadlib::io::FileWriter &writer );
        bool create_DNA( const uint64_t &genome_size );
        bool create_RNA( const uint64_t &genome_size );
        bool create_SET( const uint64_t &genome_size, const std::string &set );

      private:
        bool createGenomeFile( const uint64_t &genome_size, const std::vector<char> &set  );
        eadlib::io::FileWriter &_writer;
        Randomiser _randomiser;
    };
}

#endif //SUPERBUBBLES_CREATOR_H
