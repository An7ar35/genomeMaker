#include "GenomeCreator.h"

/**
 * Constructor
 * @param file_name File name
 */
genomeMaker::GenomeCreator::GenomeCreator( const genomeMaker::Randomiser &randomiser, eadlib::io::FileWriter &writer ) :
    _writer( writer ),
    _randomiser( randomiser )
{}

/**
 * Creates a DNA genome
 * @param genome_size Size of the genome to create
 * @return Success
 */
bool genomeMaker::GenomeCreator::create_DNA( const uint64_t &genome_size ) {
    std::vector<char> letters = { 'C', 'G', 'A', 'T' };
    return createGenomeFile( genome_size, letters );
}

/**
 * Creates a RNA genome
 * @param genome_size Size of the genome to create
 * @return Success
 */
bool genomeMaker::GenomeCreator::create_RNA( const uint64_t &genome_size ) {
    std::vector<char> letters = { 'G', 'U', 'A', 'C' };
    return createGenomeFile( genome_size, letters );
}

/**
 * Creates a genome from a set of letters
 * @param genome_size Size of the genome to create
 * @param set         Set of letters to use to create genome
 * @return Success
 */
bool genomeMaker::GenomeCreator::create_SET( const uint64_t &genome_size, const std::string &set ) {
    std::vector<char> letters;
    for( const char &c : set ) {
        letters.emplace_back( c );
    }
    return createGenomeFile( genome_size, letters );
}

/**
 * Creates a genome
 * @param genome_size Size of the genome to create
 * @param set         Set of letters to use to create genome
 * @return Success
 */
bool genomeMaker::GenomeCreator::createGenomeFile( const uint64_t &genome_size, const std::vector<char> &set ) {
    if( !_writer.open() ) {
        std::cerr << "Error: Could not open stream to '" << _writer.getFileName() << "'. Aborting." << std::endl;
        return false;
    }
    _randomiser.setPoolRange( 0, set.size() - 1 );
    std::cout << "-> creating " << genome_size << " byte(s) of synthetic genome.." << std::endl;
    eadlib::cli::ProgressBar progress( genome_size, 70);
    uint64_t step = genome_size / 100;
    for( uint64_t i = 1; i <= genome_size; i++ ) {
        _writer.write( set.at( _randomiser.getRand() ) );
        if( i % step == 0 ) {
            progress += step;
            progress.printPercentBar( std::cout, 0 );
        }
    }
    std::cout << std::endl;
    return true;
}









