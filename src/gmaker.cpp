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

namespace genomeMaker {
    bool checkGenomeOptions( genomeMaker::FileOptions &option_container );
    bool checkSequencerOptions( genomeMaker::FileOptions &option_container );
    void printGenomeOptions( const genomeMaker::FileOptions &option_container );
    void printSequencerOptions( const genomeMaker::FileOptions &option_container );
    bool existFileConflicts( const genomeMaker::FileOptions &option_container );
    off_t getFileSize( const std::string &file_name );
}

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

            if( genomeMaker::existFileConflicts( option_container ) ) {
                return -1;
            }

            /////////////////////////////
            // Genome creation section //
            /////////////////////////////
            if( option_container._genome_flag ) {
                std::cout << "-> ==| genome creation |==" << std::endl;
                if( !genomeMaker::checkGenomeOptions( option_container ) ) {
                    return -1;
                }
                //Printing info
                genomeMaker::printGenomeOptions( option_container );
                //Creating synthetic genome data
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

            //////////////////////////////////
            // Sequencer simulation section //
            //////////////////////////////////
            if( option_container._sequencer_flag ) {
                std::cout << "-> ==| sequencer simulation |==" << std::endl;
                if( !genomeMaker::checkSequencerOptions( option_container ) ) {
                    return -1;
                }
                //Error control on opening streams to the genome and new sequencer files
                eadlib::io::FileReader reader( option_container._genome_file );
                if( !reader.open() ) {
                    throw std::runtime_error( "FileReader had problem opening stream to genome file input. For more see the log." );
                }
                eadlib::io::FileWriter writer( option_container._sequencer_file );
                if( !writer.open() ) {
                    throw std::runtime_error( "FileWriter had problem opening stream to sequencer file output. For more see the log." );
                }
                //Printing info
                genomeMaker::printSequencerOptions( option_container );
                //Simulating sequencer reads...
                auto randomiser = genomeMaker::Randomiser();
                auto sequencer = genomeMaker::SequencerSim( reader,
                                                            writer,
                                                            randomiser,
                                                            option_container._read_length,
                                                            option_container._read_depth,
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

/**
 * Makes sure the genome creation options are valid
 * @param option_container FileOptions container
 * @return Success
 */
bool genomeMaker::checkGenomeOptions( genomeMaker::FileOptions &option_container ) {
    if( option_container._genome_size < 1 ) {
        std::cerr << "Error: no genome size specified. Aborting." << std::endl;
        return false;
    }
    return true;
}

/**
 * Makes sure the Sequencer Sim options are valid
 * @param option_container FileOptions container
 * @return Success
 */
bool genomeMaker::checkSequencerOptions( genomeMaker::FileOptions &option_container ) {
    try {
        if( option_container._read_depth < 1 ) { //No read count given or 0
            std::cout << "-> No read count given. A depth will be calculated." << std::endl;
        }
        off_t genome_file_size = genomeMaker::getFileSize( option_container._genome_file );
        if( genome_file_size < 1 ) {
            std::cerr << "Error: Genome file looks empty. Aborting." << std::endl;
            return false;
        }
        return true;
    } catch ( std::runtime_error e ) {
        std::cerr << "Error: Couldn't get the size of the specified Genome file.";
        std::cerr << e.what() << std::endl;
        return false;
    }
}

/**
 * Prints genome options from the option container
 * @param option_container FileOptions container
 */
void genomeMaker::printGenomeOptions( const genomeMaker::FileOptions &option_container ) {
    std::cout << "-> Genome file options: " << std::endl;
    std::cout << "\tGenome file: " << option_container._genome_file << std::endl;
    std::cout << "\tGenome size: " << option_container._genome_size << std::endl;
    std::cout << "\tGenome type: ";
    switch( option_container._letter_set ) {
        case FileOptions::LetterSet::DNA:
            std::cout << "DNA" << std::endl;
            break;
        case FileOptions::LetterSet::RNA:
            std::cout << "RNA" << std::endl;
            break;
    }
}

/**
 * Prints sequencer options from the option container
 * @param option_container FileOptions container
 */
void genomeMaker::printSequencerOptions( const genomeMaker::FileOptions &option_container ) {
    std::cout << "-> Sequencer file options: " << std::endl;
    std::cout << "\tRead file : " << option_container._sequencer_file << std::endl;
    std::cout << "\tRead count: " << option_container._read_depth << std::endl;
    std::cout << "\tRead size : " << option_container._read_length << std::endl;
    std::cout << "\tError rate: " << option_container._error_rate << std::endl;
}

/**
 * Checks if there are any file conflicts with chosen file name in option container
 * @param option_container FileOptions container
 * @return Conflict state
 */
bool genomeMaker::existFileConflicts( const genomeMaker::FileOptions &option_container ) {
    bool genome_file_exists = access( option_container._genome_file.c_str(), F_OK ) != -1;
    bool sequencer_file_exists = access( option_container._sequencer_file.c_str(), F_OK ) != -1;
    if( option_container._genome_flag && option_container._sequencer_flag ) { //Everything
        if( genome_file_exists ) {
            std::cerr << "Error: genome file already exists." << std::endl;
            return true;
        }
        if( sequencer_file_exists ) {
            std::cerr << "Error: sequencer file already exists." << std::endl;
            return true;
        }
    } else if( option_container._genome_flag && !option_container._sequencer_flag ) { //Genome only
        if( genome_file_exists ) {
            std::cerr << "Error: genome file already exists." << std::endl;
            return true;
        }
    } else if( !option_container._genome_flag && option_container._sequencer_flag ) { //Sequencer only
        if( genome_file_exists ) {
            //TODO load file
            //TODO if empty then warn + stop
        } else {
            std::cerr << "Error: genome file does not exists. Cannot simulate sequencer on nothing!." << std::endl;
            return true;
        }
        if( sequencer_file_exists ) {
            std::cerr << "Error: sequencer file already exists." << std::endl;
            return true;
        }
    }
    return false;
}

/**
 * Gets the size of a file
 * @param file_name File name
 * @return Sixe of file in bytes
 * @throws std::runtime_error when file cannot be accessed
 */
off_t genomeMaker::getFileSize( const std::string &file_name ) {
    auto file_stats = eadlib::io::FileStats( file_name );
    if( !file_stats.isValid() ) {
        throw std::runtime_error( "FileStats could not gain access to the genome file. For more see log." );
    }
    return file_stats.getSize();
}