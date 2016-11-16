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
            if( !option_container._genome_flag && !option_container._sequencer_flag ) {
                std::cerr << "Error: Not enough options supplied to do anything." << std::endl;
                return -1;
            }
            if( genomeMaker::existFileConflicts( option_container ) ) {
                return -1;
            }

            std::cout << "|=========[ " << GENOMEMAKER_DESC << " ]=========|\n" << std::endl;
            /////////////////////////////
            // Genome creation section //
            /////////////////////////////
            if( option_container._genome_flag ) {
                std::cout << "===| genome creation |===" << std::endl;
                if( !genomeMaker::checkGenomeOptions( option_container ) ) {
                    return -1;
                }
                //Printing info
                genomeMaker::printGenomeOptions( option_container );
                //Creating synthetic genome data
                eadlib::io::FileWriter writer( option_container._genome_file );
                auto creator = genomeMaker::GenomeCreator( genomeMaker::Randomiser(), writer );
                switch( option_container._letter_set ) {
                    case genomeMaker::FileOptions::LetterSet::DNA:
                        if( !creator.create_DNA( option_container._genome_size ) ) {
                            return -1;
                        }
                        break;
                    case genomeMaker::FileOptions::LetterSet::RNA:
                        if( creator.create_RNA( option_container._genome_size ) ) {
                            return -1;
                        }
                        break;
                }
                std::cout << "-> Genome created." << std::endl;
            }

            //////////////////////////////////
            // Sequencer simulation section //
            //////////////////////////////////
            if( option_container._sequencer_flag ) {
                std::cout << "===| sequencer simulation |===" << std::endl;
                if( !genomeMaker::checkSequencerOptions( option_container ) ) {
                    return -1;
                }
                //Error control on opening streams to the genome and new sequencer files
                eadlib::io::FileReader reader( option_container._genome_file );
                if( !reader.open() ) {
                    LOG_ERROR( "[main(..)] FileReader had a problem opening stream to genome file input '", reader.getFileName(), "'." );
                    std::cerr << "Error: FileReader had problem opening stream to genome file input. For more see the log." << std::endl;
                    return -1;
                }
                eadlib::io::FileWriter writer( option_container._sequencer_file );
                if( !writer.open() ) {
                    LOG_ERROR( "[main(..)] FileWriter had a problem opening stream to sequencer file output '", writer.getFileName(), "'." );
                    std::cerr << "Error: FileWriter had problem opening stream to sequencer file output. For more see the log." << std::endl;
                    return -1;
                }
                //Creating Randomiser objects
                auto read_randomiser  = genomeMaker::Randomiser();
                auto error_randomiser = genomeMaker::Randomiser();
                //Printing info
                genomeMaker::printSequencerOptions( option_container );
                //Simulating sequencer reads...
                auto sequencer = genomeMaker::SequencerSim( reader,
                                                            writer,
                                                            read_randomiser,
                                                            error_randomiser );
                sequencer.start( option_container._read_length,
                                 option_container._read_depth,
                                 option_container._error_rate );
                std::cout << "-> Sequencer reads file created." << std::endl;
            }
            std::cout << "-> Finished." << std::endl;
        }
    } catch( std::exception e ) {
        std::cerr << "Something really wrong happened..." << std::endl;
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
        if( option_container._read_depth < 1 ) { //No read depth given or 0
            std::cout << "-> No read depth was given." << std::endl;
            return false;
        }
        if( option_container._read_length < 1 || option_container._read_length > 1000 ) {
            std::cout << "-> Invalid read length (" << option_container._read_length << ") given.";
            std::cout << " Must be between 1-1000 inc. Aborting." << std::endl;
            return false;
        }
        if( option_container._error_rate < 0 || option_container._error_rate > 1 ) {
            std::cout << "-> Invalid error rate. Must be between 0-1 inc. Aborting." << std::endl;
            return false;
        }
        off_t genome_file_size = genomeMaker::getFileSize( option_container._genome_file );
        if( genome_file_size < 1 ) {
            std::cerr << "Error: Genome file looks empty. Aborting." << std::endl;
            return false;
        }
        return true;
    } catch ( std::runtime_error e ) {
        LOG_ERROR( "[genomeMaker::checkSequencerOptions( <genomeMaker::FileOptions> )] "
                       "Could not get size of genome file '", option_container._genome_file, "'." );
        std::cerr << "Error: Couldn't get the size of the specified Genome file.";
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
    std::cout << "\tRead depth: " << option_container._read_depth << std::endl;
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
            try {
                if( getFileSize( option_container._genome_file ) < 1 ) {
                    std::cerr << "Error: Genome file looks empty. Aborting." << std::endl;
                    return false;
                }
            } catch ( std::runtime_error e ) {
                std::cerr << "Error: Reference genome file doesn't exist. Aborting." << std::endl;
                return false;
            }
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
        LOG_ERROR( "[genomeMaker::getFileSize( ", file_name, " )] FileStats could not gain access to the file." );
        throw std::runtime_error( "FileStats could not gain access to the genome file. For more see log." );
    }
    return file_stats.getSize();
}