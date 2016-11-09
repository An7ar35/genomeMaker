#include "SequencerSim.h"

/**
 * Constructor
 * @param reader          EADlib File Reader
 * @param writer          EADlib File Writer
 * @param read_length     Number of characters per reads
 * @param number_of_reads Total number of simulated reads
 * @param error_rate      Error rate of the simulator on the reads (0 to 1)
 */
genomeMaker::SequencerSim::SequencerSim( eadlib::io::FileReader &reader, eadlib::io::FileWriter &writer,
                                         const size_t &read_length, const size_t &number_of_reads, const double &error_rate ) :
    _reader( reader ),
    _writer( writer ),
    _read_length( read_length ),
    _total_reads( number_of_reads ),
    _error_rate( error_rate )
{
    //Error checks
    if( _read_length < 1 || _read_length > 1000 ) {
        _read_length = 260;
        LOG_ERROR( "[genomeMaker::SequencerSim( <Reader>, <Writer>, ", read_length, ", ", number_of_reads, ", ", error_rate, " )] "
            "Invalid read length of ", read_length, ". Defaulting to '", _read_length, "'." );
    }
    if( _total_reads < 1 ) {
        _total_reads = 1;
        LOG_ERROR( "[genomeMaker::SequencerSim( <Reader>, <Writer>, ", read_length, ", ", number_of_reads, ", ", error_rate, " )] "
            "Invalid number of reads. Defaulting to '", _total_reads, "'." );
    }
    if( _error_rate < 0 || _error_rate > 1 ) {
        _error_rate = 0;
        LOG_ERROR( "[genomeMaker::SequencerSim( <Reader>, <Writer>, ", read_length, ", ", number_of_reads, ", ", error_rate, " )] "
            "Invalid error rate of ", error_rate, ". Defaulting to '", _error_rate, "'." );
    }
    //TODO make sure to output default values to the cli --help info
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
    LOG( "[genomeMaker::SequencerSim::start()] Read length: ", _read_length, "; Total reads: ", _total_reads, "; Error rate: ", _error_rate, "." );
    LOG( "[genomeMaker::SequencerSim::start()] Writing to file: '", _writer.getFileName() , "'." );
    //TODO output stats to screen?
    std::cout << "..Starting..." << std::endl;
    //Sequencer simulation


    unsigned line_char_length = 71; //per line max char write for cleanliness
    auto randomiser = Randomiser(); //TODO set pool range

    std::vector<char> v;
    while( _reader.read( v, 3 ) ) {
        for( auto i : v ) {
            std::cout << i << ", ";
        }
        std::cout << std::endl;
    }

    unsigned count { 0 };
    while( count < _total_reads ) {
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
size_t genomeMaker::SequencerSim::getTotalReads() const {
    return _total_reads;
}

/**
 * Gets the set error rate
 * @return Error rate
 */
double genomeMaker::SequencerSim::getErrorRate() const {
    return _error_rate;
}

