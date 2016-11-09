#ifndef GENOMEMAKER_FILEOPTION_H
#define GENOMEMAKER_FILEOPTION_H

#include <string>

namespace genomeMaker {
    struct FileOptions {
        //Raw genome output
        bool        _genome_flag    { false };
        std::string _genome_file    { "" };
        uint64_t    _genome_size    { 0 };
        enum class LetterSet {
            DNA,
            RNA
        } _letter_set { LetterSet::DNA };

        //Sequencer sim FASTA output
        bool        _sequencer_flag { false };
        std::string _sequencer_file { "" };
        size_t      _read_length    { 260 };
        size_t      _read_count     { 0 };
        double      _error_rate     { 0 };
    };
}

#endif //GENOMEMAKER_FILEOPTION_H
