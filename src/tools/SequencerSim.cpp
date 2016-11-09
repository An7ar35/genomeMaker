#include "SequencerSim.h"

/**
 * Constructor
 * @param reader          EADlib File Reader
 * @param writer          EADlib File Writer
 * @param read_length     Number of characters per reads
 * @param read_depth Total number of simulated reads
 * @param error_rate      Error rate of the simulator on the reads (0 to 1)
 */
genomeMaker::SequencerSim::SequencerSim( eadlib::io::FileReader &reader,
                                         eadlib::io::FileWriter &writer,
                                         genomeMaker::Randomiser &randomiser,
                                         const size_t &read_length,
                                         const size_t &read_depth,
                                         const double &error_rate ) :
    _reader( reader ),
    _writer( writer ),
    _randomiser( randomiser ),
    _read_length( read_length ),
    _read_depth( read_depth ),
    _error_rate( error_rate )
{
    //Error checks
    if( _read_length < 1 || _read_length > 1000 ) {
        _read_length = 260;
        LOG_ERROR( "[genomeMaker::SequencerSim( <Reader>, <Writer>, ", read_length, ", ", read_depth, ", ", error_rate, " )] "
            "Invalid read length of ", read_length, ". Defaulting to '", _read_length, "'." );
    }
    if( _read_depth < 1 ) {
        _read_depth = 1;
        LOG_ERROR( "[genomeMaker::SequencerSim( <Reader>, <Writer>, ", read_length, ", ", read_depth, ", ", error_rate, " )] "
            "Invalid read depth. Defaulted to '", _read_depth, "'." );
    }
    if( _error_rate < 0 || _error_rate > 1 ) {
        _error_rate = 0;
        LOG_ERROR( "[genomeMaker::SequencerSim( <Reader>, <Writer>, ", read_length, ", ", read_depth, ", ", error_rate, " )] "
            "Invalid error rate of ", error_rate, ". Defaulting to '", _error_rate, "'." );
    }
    _read_count = calcReadCount( _reader.size(), _read_length, _read_depth );
    std::cout << "-> Calculated and applied a read count of: " << _read_depth << std::endl;
}

/**
 * Destructor
 */
genomeMaker::SequencerSim::~SequencerSim() {}

/**
 * Starts sequence read simulation
 * @return Success
 */
bool genomeMaker::SequencerSim::start() {
    //Error control
    if( _reader.getFileName() == _writer.getFileName() ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::start()] Input and Output files are the same ('", _reader.getFileName(), "')!" );
        return false;
    }
    //Stats output
    LOG( "[genomeMaker::SequencerSim::start()] Reading from file: '", _reader.getFileName() , "'." );
    LOG( "[genomeMaker::SequencerSim::start()] Read length: ", _read_length, "; Total reads: ", _read_depth, "; Error rate: ", _error_rate, "." );
    LOG( "[genomeMaker::SequencerSim::start()] Read count calculated as: ", _read_count );
    LOG( "[genomeMaker::SequencerSim::start()] Writing to file: '", _writer.getFileName() , "'." );
    std::cout << "...Starting..." << std::endl;


    //TODO add throbber or progress bar
    //Sequencer simulation


    size_t line_size = 71; //per line max char write for cleanliness
    _randomiser.setPoolRange( 0, 10 ); //TODO calculate and set real pool range -> try to fit read_length into chunk?

    //TODO take out chunk and do random reads * depth
    std::vector<char> v;
    while( _reader.read( v, _read_length ) ) {
        for( auto i : v ) {
            std::cout << i << ", ";
        }
        std::cout << std::endl;
    }

    unsigned count { 0 };
    while( count < _read_depth ) {
        //_reader.read()?


        count++;
    }

    //TODO error % variable needed

    std::stringstream ss;





    // begin with '>' and description
    // on new line the read
    // * and blank line at the end of each read


    return false;
}

/**
 * Gets the set read character size
 * @return Read length
 */
size_t genomeMaker::SequencerSim::getReadLength() const {
    return _read_length;
}

/**
 * Gets the set total number of reads
 * @return Total reads
 */
size_t genomeMaker::SequencerSim::getReadDepth() const {
    return _read_depth;
}

/**
 * Gets the set error rate
 * @return Error rate
 */
double genomeMaker::SequencerSim::getErrorRate() const {
    return _error_rate;
}

/**
 * Gets the number of reads
 * @return Read count
 */
uint64_t genomeMaker::SequencerSim::getReadCount() const {
    return _read_count;
}

/**
 * Calculates the read count
 * @param genome_byte_size Genome size in bytes
 * @param read_length      Read length in number of chars
 * @return Read depth
 */
uint64_t genomeMaker::SequencerSim::calcReadCount( const std::streampos &genome_size,
                                                   const size_t &read_length,
                                                   const size_t &read_depth ) const
{
    //depth = read_count * read_length / genome_size
    return read_depth * genome_size / _read_length;
}

