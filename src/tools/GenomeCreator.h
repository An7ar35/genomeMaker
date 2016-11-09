#ifndef SUPERBUBBLES_CREATOR_H
#define SUPERBUBBLES_CREATOR_H

//TODO DNA base pair generation (GC, AT) at some point?
//TODO RNA base pair generation (GC, UA, CG) at some point?
//TODO completely random generation from of a set of given letters

#include <iostream>

#include "eadlib/logger/Logger.h"
#include "eadlib/io/FileWriter.h"
#include "eadlib/cli/graphic/ProgressBar.h"

#include "Randomiser.h"

namespace genomeMaker {
    class GenomeCreator {
      public:
        GenomeCreator( const Randomiser &randomiser, const std::string &file_name  );
        void create_DNA( const uint64_t &genome_size );
        void create_RNA( const uint64_t &genome_size );
        void create_SET( const uint64_t &genome_size, const std::string &set );

      private:
        void createGenomeFile( const uint64_t &genome_size, const std::vector<char> &set  );
        std::string _file_name;
        Randomiser _randomiser;
    };
}

#endif //SUPERBUBBLES_CREATOR_H
