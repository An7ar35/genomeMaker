#include "cli.h"

/**
 * Loads up all the cli options flags, descriptions and examples
 * @param argv cli arguments
 * @param parser Parser
 */
void genomeMaker::cli::setupOptions( char **argv, const std::string &program_title, eadlib::cli::Parser &parser ) {
    //Program title line
    parser.addTitleLine( std::string( "=================" + program_title + "=================" ) );
    //Description block
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
                   { { std::regex( "([0-9a-zA-Z]+)" ), "File name must be composed of only letter/numbers with no extension." } } );
    //Genome file loader/creation section
    parser.option( "Genome", "-g", "-genome", "Name of the genome file to create.", false,
                   { { std::regex( "^(.*/)?(?:$|(.+?)(?:(\\.[^.]*$)|$))+" ), "Invalid filename." } }  );
    parser.option( "Genome", "-s", "-size", "Size of the genome in bytes.", false,
                   { { std::regex( "[0-9]+" ), "Size value must be integer." } }  );
    parser.option( "Genome", "-t", "-type", "Type of letter set for genome creation (DNA, RNA).", false,
                   { { std::regex( "^DNA$|^RNA$", std::regex::icase ), "Letter type must be either \'DNA\' or \'RNA\'", "DNA" } } );
    //Simulated sequencer reads file creation section
    parser.option( "Sequencer", "-f", "-fasta", "Name of the FASTA file to create.", false,
                   { { std::regex( "^(.*/)?(?:$|(.+?)(?:(\\.[^.]*$)|$))+" ), "Invalid filename." } }  );
    parser.option( "Sequencer", "-l", "-length", "Character length of each reads.", false,
                   { { std::regex( "[0-9]+" ), "Character length value must be integer.", "260" } }  );
    parser.option( "Sequencer", "-d", "-depth", "Depth of reads.", false,
                   { { std::regex( "[0-9]+" ), "Depth of reads value must be integer." } }  );
    parser.option( "Sequencer", "-e", "-error", "Error rate of the simulated sequencer (0 <= x <= 1).", false,
                   { { std::regex( "^[0-1]$|^0\\.[0-9]+$" ), "Error rate should be between 0-1 inclusive.", "0" } }  );
    //Example block
    parser.addExampleLine( "(a) Just a synthetic genome file of 100,000,000 bytes (100MB)\n"
                               "    with the RNA letter set:");
    parser.addExampleLine( "    " + std::string( argv[ 0 ] ) + " -g genome_file -s 100000000 -t rna" );
    parser.addExampleLine( "(b) Just a sequencer file named 'reads.fasta' with the default\n"
                               "    read length of 260, error rate of 0.01, depth of 200 and based\n"
                               "    on a genome file called 'genome.genome':" );
    parser.addExampleLine( "    " + std::string( argv[ 0 ] ) + " -g genome -f reads -d 200 -e 0.01" );
    parser.addExampleLine( "(c) Complete pipeline with a genome file called 'my_file.genome'\n"
                               "    of 100 000 bytes and a sequencer file 'my_file.fasta' with reads\n"
                               "    of 10 characters and a depth of 5:" );
    parser.addExampleLine( "    " + std::string( argv[ 0 ] ) + " -p my_file -s 100000 -l 10 -d 5" );
}

/**
 * Loads parser options into container
 * @param parser Parser
 * @param options Option container
 */
void genomeMaker::cli::loadOptionsIntoContainer( const eadlib::cli::Parser &parser, FileOptions &options ) {
    auto converter = eadlib::tool::Convert();
    //All-in-One
    if( parser.getValueFlags( "-pipeline" ).at( 0 )) {
        std::string file_name = parser.getValues( "-pipeline" ).at( 0 );
        options._genome_file = file_name + ".genome";
        options._sequencer_file = file_name + ".fasta";
        options._genome_flag = true;
        options._sequencer_flag = true;
    }
    //Genome file
    if( parser.getValueFlags( "-genome" ).at( 0 ) ) {
        options._genome_file = parser.getValues( "-genome" ).at( 0 );
    }
    if( parser.getValueFlags( "-size" ).at( 0 ) ) {
        options._genome_flag = true;
        options._genome_size = converter.string_to_type<uint64_t>( parser.getValues( "-size" ).at( 0 ) );
        LOG( "What?" );
    }
    if( parser.getValueFlags( "-type" ).at( 0 ) ) {
        std::string val = parser.getValues( "-type" ).at( 0 );
        if( val == "DNA" || val == "dna" ) {
            options._letter_set = FileOptions::LetterSet::DNA;
        } else if( val == "RNA" || val == "rna" ) {
            options._letter_set = FileOptions::LetterSet::RNA;
        } else {
            std::cerr << "Error: Letter set for genome given is invalid."  << std::endl;
            throw std::invalid_argument( "Letter set given for type of genome is invalid." );
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
    if( parser.getValueFlags( "-depth" ).at( 0 ) ) {
        options._read_depth = converter.string_to_type<unsigned>( parser.getValues( "-depth" ).at( 0 ) );
    }
    if( parser.getValueFlags( "-error" ).at( 0 ) ) {
        options._error_rate = converter.string_to_type<double>( parser.getValues( "-error" ).at( 0 ) );
    }
}