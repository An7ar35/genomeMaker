#include "cli.h"

/**
 * Loads up all the cli options flags, descriptions and examples
 * @param argv cli arguments
 * @param parser Parser
 */
void genomeMaker::cli::setupOptions( char **argv, const std::string &program_title, eadlib::cli::Parser &parser ) {
    //Description block
    parser.addTitleLine( std::string( "=================" + program_title + "=================" ) );
    parser.addDescriptionLine( "Program for creating synthetic genome data and\n"
                                   "the simulated sequencer reads. This was made to\n"
                                   "create both base data and FASTA reads for testing\n"
                                   "De-Novo genome assembly implementations with\n"
                                   "custom sized data sets.\n" );
    parser.addDescriptionLine( "TL;DR: Used to compare the re-assembled genome to\n"
                                   "an original for testing.\n" );
    parser.addDescriptionLine( "The program can be used in the following cases:" );
    parser.addDescriptionLine( "(a) Create a synthetic genome alone." );
    parser.addDescriptionLine( "(b) Create a simulated sequencer reads file (FASTA)\n"
                                   "    from an existing genome file." );
    parser.addDescriptionLine( "(c) Create both the synthetic genome file and its\n"
                                   "    simulated sequencer reads file." );
    parser.addDescriptionLine( "\nUsage:" + std::string( argv[ 0 ] ) + " -<option> <argument>" );
    //Pipeline both genome creation and sequencer simulation section
    parser.option( "All-In-One", "-p", "-pipeline", "Create both genome and sequencer files.", false,
                   { { std::regex( "[a-z][0-9]+" ), "File name must be composed of only letter/numbers." } }  );
    //Genome file loader/creation section
    parser.option( "Genome", "-g", "-genome", "Name of the genome file to create.", false,
                   { { std::regex( "[a-z][0-9]+" ), "File name must be composed of only letter/numbers." } }  );
    parser.option( "Genome", "-s", "-size", "Size of the genome in bytes.", false,
                   { { std::regex( "[0-9]+" ), "Size value must be integer." } }  );
    parser.option( "Genome", "-t", "-type", "Type of letter set for genome creation (DNA, RNA).", false,
                   { { } }  );
    //Simulated sequencer reads file creation section
    parser.option( "Sequencer", "-f", "-fasta", "Name of the FASTA file to create.", false,
                   { { std::regex( "[a-z][0-9]+" ), "File name must be composed of only letter/numbers." } }  );
    parser.option( "Sequencer", "-l", "-length", "Character length of each reads.", false,
                   { { std::regex( "[0-9]+" ), "Character length value must be integer." } }  );
    parser.option( "Sequencer", "-c", "-count", "Number of reads.", false,
                   { { std::regex( "[0-9]+" ), "Number of reads value must be integer." } }  );
    parser.option( "Sequencer", "-e", "-error", "Error rate of the simulated sequencer (0 <= x <= 1).", false,
                   { { } }  );

    //Example block
    parser.addExampleLine( std::string( argv[ 0 ] ) + " genome_file 100000" );
    parser.addExampleLine( std::string( argv[ 0 ] ) + " -o genome_file -s 100000" );
    parser.addExampleLine( std::string( argv[ 0 ] ) + " -o genome_file -s 100000 -t RNA" );
    parser.addExampleLine( std::string( argv[ 0 ] ) + " -o genome_file -s 100000 -t SET ABCDEFG" );

    //TODO sort out all the regular expressions for the arguments
}

/**
 * Loads parser options into container
 * @param parser Parser
 * @param options Option container
 */
void genomeMaker::cli::loadOptionsIntoContainer( const eadlib::cli::Parser &parser, FileOptions &options ) {
    try {
        auto converter = eadlib::tool::Convert();
        //All-in-One
        if( parser.getValueFlags( "-pipeline" ).at( 0 ) ) {
            std::string file_name = parser.getValues( "-pipeline" ).at( 0 );
            options._genome_file    = file_name + ".genome";
            options._sequencer_file = file_name + ".fasta";
            options._genome_flag    = true;
            options._sequencer_flag = true;
        }
        //Genome file
        if( parser.getValueFlags( "-genome" ).at( 0 ) ) {
            options._genome_file = parser.getValues( "-genome" ).at( 0 );
        }
        if( parser.getValueFlags( "-size" ).at( 0 ) ) {
            options._genome_flag = true;
            options._genome_size = converter.string_to_type<uint64_t>( parser.getValues( "-size" ).at( 0 ));
        }
        if( parser.getValueFlags( "-type" ).at( 0 ) ) {
            std::string val = parser.getValues( "-type" ).at( 0 );
            if( val == "DNA" ) {
                options._letter_set = FileOptions::LetterSet::DNA;
            } else if( val == "RNA" ) {
                options._letter_set = FileOptions::LetterSet::RNA;
            } else {
                //TODO error control on bad letter set
            }
        }
        //Sequencer sim file
        if( parser.getValueFlags( "-fasta" ).at( 0 ) ) {
            options._sequencer_file = parser.getValues( "-fasta" ).at( 0 );
            options._sequencer_flag = true;
        }
        if( parser.getValueFlags( "-length" ).at( 0 ) ) {
            options._read_length = converter.string_to_type<size_t>( parser.getValues( "-length" ).at( 0 ) );
        }
        if( parser.getValueFlags( "-count" ).at( 0 ) ) {
            options._read_count = converter.string_to_type<size_t>( parser.getValues( "-count" ).at( 0 ) );
        }
        if( parser.getValueFlags( "-error" ).at( 0 ) ) {
            options._error_rate = converter.string_to_type<double>( parser.getValues( "-error" ).at( 0 ) );
        }
    } catch ( std::exception e ) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Checks if there are any file conflicts with chosen file name in option container
 * @param option_container FileOptions container
 * @return Conflict state
 */
bool genomeMaker::cli::existFileConflicts( const genomeMaker::FileOptions &option_container ) {
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