#include <iostream>

#include "eadlib/logger/Logger.h"
#include "eadlib/cli/parser/Parser.h"
#include "eadlib/io/FileReader.h"
#include "eadlib/io/FileWriter.h"
#include "eadlib/io/FileStats.h"

#include "containers/FileOptions.h"
#include "cli/cli.h"
#include "tools/GenomeCreator.h"
#include "tools/SequencerSim.h"

/**
 * Entry into program
 * @param argc Number of arguments passed
 * @param argv Arguments
 * @return Exit code
 */
int main( int argc, char *argv[] ) {
    const std::string GENOMEMAKER_DESC = "genomeMaker v0.1a";
    try {
        auto parser = eadlib::cli::Parser();
        genomeMaker::cli::setupOptions( argv, GENOMEMAKER_DESC, parser );
        if( parser.parse( argc, argv ) ) {
            auto option_container = genomeMaker::FileOptions();
            genomeMaker::cli::loadOptionsIntoContainer( parser, option_container );

            std::cout << "|-------------------------------------------|\n"
                      << "|=========[ " << GENOMEMAKER_DESC << " ]=========|\n"
                      << "|-------------------------------------------|\n"
                      << std::endl;

            if( genomeMaker::cli::existFileConflicts( option_container ) ) {
                return -1;
            }


            //All-in-One
            //  >[DONE] if file(s) exist
            //      >[DONE] warn and STOP
            //  >else check the relevant flags required to create the stuff...
            //Genome file:
            //  >[DONE] does file exist?
            //  >if yes then load
            //      >if load empty then warn and continue to creation stage
            //      >else load file
            //  >[DONE] else check if size is given !-> no size = STOP
            //  >[DONE] check type (no type = default)
            //      >[DONE] if OK continue
            //      >[DONE] if !OK warn and go default (DNA)
            //
            //Sequencer file
            //  >[DONE] if file exists then warn and stop
            //  >check length is given (none = 260 [default])
            //  >check count is given (none = calculate default based on genome file size)
            //  >check error is given (none = 0 [default])

            //Genome creation section
            if( option_container._genome_flag ) {
                if( option_container._genome_size < 1 ) {
                    std::cerr << "Error: no genome size specified. Aborting." << std::endl;
                    return -1;
                }
                auto creator = genomeMaker::GenomeCreator( genomeMaker::Randomiser(),
                                                           option_container._genome_file );
                switch( option_container._letter_set ) {
                    case genomeMaker::FileOptions::LetterSet::DNA:
                        creator.create_DNA( option_container._genome_size );
                        break;
                    case genomeMaker::FileOptions::LetterSet::RNA:
                        creator.create_RNA( option_container._genome_size );
                }
                std::cout << "-> Genome created." << std::endl;
            }
            //Sequencer simulation and FASTA file creation section
            if( option_container._sequencer_flag ) {
                if( option_container._read_count < 1 ) {
                    std::cerr << "Error: read count for sequencer simulation is too low. Aborting." << std::endl;
                    return -1;
                }
                eadlib::io::FileReader reader( option_container._genome_file );
                if( !reader.open() ) {
                    throw std::runtime_error( "FileReader had problem opening stream to genome file input. For more see the log." );
                }
                eadlib::io::FileWriter writer( option_container._sequencer_file );
                if( !writer.open() ) {
                    throw std::runtime_error( "FileWriter had problem opening stream to sequencer file output. For more see the log." );
                }
                auto sequencer = genomeMaker::SequencerSim( reader,
                                                            writer,
                                                            option_container._read_length,
                                                            option_container._read_count,
                                                            option_container._error_rate );
                sequencer.start();
                std::cout << "-> Sequencer reads file created." << std::endl;
            }
            std::cout << "-> Finished." << std::endl;
        }
    } catch( std::exception e ) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}