#include "SequencerSim.h"

/**
 * Constructor
 * @param reader          EADlib File Reader
 * @param writer          EADlib File Writer
 * @param read_length     Number of characters per reads
 * @param read_depth      Total number of simulated reads
 * @param error_rate      Error rate of the simulator on the reads (0 to 1)
 */
genomeMaker::SequencerSim::SequencerSim( eadlib::io::FileReader &reader,
                                         eadlib::io::FileWriter &writer,
                                         genomeMaker::Randomiser &read_randomiser,
                                         genomeMaker::Randomiser &error_randomiser,
                                         const size_t &read_length,
                                         const size_t &read_depth,
                                         const double &error_rate ) :
    _reader( reader ),
    _writer( writer ),
    _read_randomiser( read_randomiser ),
    _error_randomiser( error_randomiser ),
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
        _read_depth = 10;
        LOG_ERROR( "[genomeMaker::SequencerSim( <Reader>, <Writer>, ", read_length, ", ", read_depth, ", ", error_rate, " )] "
            "Invalid read depth. Defaulting to '", _read_depth, "'." );
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
 * @param read_randomiser  Read position randomiser
 * @param error_randomiser Error randomiser
 * @return Success
 */
bool genomeMaker::SequencerSim::start() {
    //Error control
    if( _reader.getFileName() == _writer.getFileName() ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::start(..)] Input and Output files are the same ('", _reader.getFileName(), "')!" );
        return false;
    }
    //Check/Open the files
    if( !_reader.isOpen() ) {
        if( !_reader.open() ) {
            LOG_ERROR( "[genomeMaker::SequencerSim::start()] There was a problem opening the genome file." );
            std::cerr << "Error: There was a problem opening the genome file for the sequencer." << std::endl;
            return false;
        }
    }
    if( !_writer.isOpen() ) {
        if( !_writer.open() ) {
            LOG_ERROR( "[genomeMaker::SequencerSim::start()] There was a problem creating the sequencer file." );
            std::cerr << "Error: There was a problem creating the sequencer file." << std::endl;
            return false;
        }
    }
    //Logging stats
    LOG( "[genomeMaker::SequencerSim::start()] Reading from file: '", _reader.getFileName() , "'." );
    LOG( "[genomeMaker::SequencerSim::start()] Read length: ", _read_length,
         "; Total reads: ", _read_depth, "; Error rate: ", _error_rate, "." );
    LOG( "[genomeMaker::SequencerSim::start()] Read count calculated as: ", _read_count );
    LOG( "[genomeMaker::SequencerSim::start()] Writing to file: '", _writer.getFileName() , "'." );

    //Starting sequencing..
    std::cout << "...Starting..." << std::endl;
    return sequenceGenome();
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

/**
 * Run the sequencer simulation on the provided genome file
 * @param read_randomiser  Read position randomiser
 * @param error_randomiser Error randomiser
 * @return Success
 */
bool genomeMaker::SequencerSim::sequenceGenome() {
    //Setting things up
    const size_t   work_chunk_size  { _read_length * 4 };
    const uint64_t genome_size      { _reader.size() > 0 ? (uint64_t) _reader.size() : 0 };
    const uint64_t whole_chunks     { genome_size / work_chunk_size };
    const uint64_t chunks_remainder { genome_size % work_chunk_size };
    const uint64_t reads_per_chunk  { _read_count / ( _reader.size() / work_chunk_size ) };
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Size of unit of work from file.: ", work_chunk_size );
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Genome size (#chars)...........: ", genome_size );
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Number of whole units..........: ", whole_chunks );
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Size of remainder left (#chars): ", chunks_remainder );
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Number of reads per unit ......: ", reads_per_chunk );
    std::unique_ptr<std::vector<char>> current_buffer; // = std::make_unique<std::vector<char>>( _read_length, '0' );
    std::unique_ptr<std::vector<char>> next_buffer; // = std::make_unique<std::vector<char>>( _read_length, '0' );

    return 1;

    bool data_remaining_flag { true };
    do {
        std::streamsize current_size = _reader.read( *current_buffer, work_chunk_size );
        std::streamsize next_size = _reader.read( *next_buffer, work_chunk_size );
        if( current_size < 1 ) {
            data_remaining_flag = false;
        } else {
            if( current_size < work_chunk_size ) {
                //End of file reached during 'current' read


            } else if( next_size < work_chunk_size ) {
                //End of file reached during 'next' read: concatenating 'next' into 'current'
                current_buffer->insert( current_buffer->end(), next_buffer->begin(), next_buffer->end() );
                current_size += next_size;
            } else {
                //both 'current' and 'next' buffers filled completely

            }
        }
    } while( data_remaining_flag );

    //[DONE] the size request of the read buffer will need to be the size of the genome chunk to work on
    //[DONE] when reading genome I get the filled size of the returned buffer
    //thus; when the end is reached we can work something out for the reads
    //but might be good to cache the next work chunk before and check its size. (careful on swap operations.. better not)
    //>if that size is smaller than the expected work chunk then combine the two and adapt the different var for the new size
    //also, need to think of what if the first work chunk turns out to be smaller in size than the expected..?



    _read_randomiser.setPoolRange( 0, 10 ); //TODO calculate and set real pool range -> try to fit read_length into chunk?
    _error_randomiser.setPoolRange( 0, 0 );
    //decide work chunk size
    //take a chunk and append overlap part of previous one
    //calculate number of reads for chunk (will need to keep in mind chunk overlaps)
    //

    //Sequencing...
    auto progress_bar = eadlib::cli::ProgressBar( _read_count, 60 );
    uint64_t reads_done = { 0 };
    std::vector<char> read;
    while( reads_done < _read_count ) {
        progress_bar++;
        reads_done++;


        //print read to sequencer file
        if( !_writer.write( formatRead( ">read#" + reads_done, read ) ) ) { //can I avoid formatter and just string the read?
            LOG_ERROR( "[genomeMaker::SequencerSim::start( <Randomiser>, <Randomiser> )] Error occurred whilst writing read ",
                       reads_done, "/", _read_count, " to sequencer file '", _writer.getFileName(), "'." );
            std::cerr << "Error: could not write read #" << reads_done << " to sequencer file: " << std::endl;
            //TODO std::cerr << read that failed with relative start position in genome
            std::cerr << "Aborting..." << std::endl;
            return false;
        }


        progress_bar.printPercentBar( std::cout, 4 );


    }

    //Sequencer simulation





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



    std::cout << std::endl;
    return false;
}

/**
 * Fromats a read for the sequencer output
 * @param name   Name of the read
 * @param read   Read
 * @return Formatted read
 */
std::string genomeMaker::SequencerSim::formatRead( const std::string &name,
                                            const std::vector<char> &read ) {
    static const size_t LINE_SIZE = 71; //per line max char write for cleanliness
    std::stringstream ss;
    ss << ">" << name << "\n";
    for( size_t i = 0; i < read.size(); i++ ) {
        ss << read.at( i );
        if( i % LINE_SIZE == 0 ) {
            ss << "\n";
        }
    }
    ss << "\n\n";
    return ss.str();
}
