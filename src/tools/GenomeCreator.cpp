#include "GenomeCreator.h"

/**
 * Constructor
 * @param file_name File name
 */
genomeMaker::GenomeCreator::GenomeCreator( const genomeMaker::Randomiser &randomiser, const std::string &file_name ) :
    _file_name( file_name ),
    _randomiser( randomiser )
{}

/**
 * Creates a DNA genome
 * @param genome_size Size of the genome to create
 */
void genomeMaker::GenomeCreator::create_DNA( const uint64_t &genome_size ) {
    std::vector<char> letters = { 'C', 'G', 'A', 'T' };
    createGenomeFile( genome_size, letters );
}

/**
 * Creates a RNA genome
 * @param genome_size Size of the genome to create
 */
void genomeMaker::GenomeCreator::create_RNA( const uint64_t &genome_size ) {
    std::vector<char> letters = { 'G', 'U', 'A', 'C' };
    createGenomeFile( genome_size, letters );
}

/**
 * Creates a genome from a set of letters
 * @param genome_size Size of the genome to create
 * @param set         Set of letters to use to create genome
 */
void genomeMaker::GenomeCreator::create_SET( const uint64_t &genome_size, const std::string &set ) {
    std::vector<char> letters;
    for( const char &c : set ) {
        letters.emplace_back( c );
    }
    createGenomeFile( genome_size, letters );
}

/**
 * Creates a genome
 * @param genome_size Size of the genome to create
 * @param set         Set of letters to use to create genome
 */
void genomeMaker::GenomeCreator::createGenomeFile( const uint64_t &genome_size, const std::vector<char> &set ) {
    _randomiser.setPoolRange( 0, set.size() - 1 );
    std::cout << "Creating " << genome_size << " bits of synthetic genome.." << std::endl;
    eadlib::cli::ProgressBar progress( genome_size, 70);
    eadlib::io::FileWriter writer( _file_name + ".genome" );
    for( uint64_t i = 1; i <= genome_size; i++ ) {
        writer.write( set.at( _randomiser.getRand() ) );
        std::cout << progress++;
    }
    std::cout << std::endl;
}









