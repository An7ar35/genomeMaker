#include "SequencerSim.h"

/**
 * Constructor
 * @param reader           EADlib File Reader
 * @param writer           EADlib File Writer
 * @param read_randomiser  Read position randomiser
 * @param error_randomiser Error randomiser
 */
genomeMaker::SequencerSim::SequencerSim( eadlib::io::FileReader &reader,
                                         eadlib::io::FileWriter &writer,
                                         genomeMaker::Randomiser &read_randomiser,
                                         genomeMaker::Randomiser &error_randomiser ) :
    _reader( reader ),
    _writer( writer ),
    _read_randomiser( read_randomiser ),
    _error_randomiser( error_randomiser )
{}

/**
 * Destructor
 */
genomeMaker::SequencerSim::~SequencerSim() {}

/**
 * Starts sequence read simulation
 * @param read_length Number of characters per reads
 * @param read_depth  Total number of simulated reads
 * @param error_rate  Error rate of the simulator on the reads (0 to 1)
 * @return Success
 */
bool genomeMaker::SequencerSim::start( const size_t &read_length,
                                       const size_t &read_depth,
                                       const double &error_rate ) {
    //Error control
    if( read_length < 1 || read_length > 1000 ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::start( ", read_length, ", ", read_depth, ", ", error_rate, " )] "
            "Invalid read length of '", read_length, "'." );
        return false;
    }
    if( read_depth < 1 ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::start( ", read_length, ", ", read_depth, ", ", error_rate, " )] "
            "Invalid read depth of '", read_depth, "'." );
        return false;
    }
    if( error_rate < 0 || error_rate > 1 ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::start( ", read_length, ", ", read_depth, ", ", error_rate, " )] "
            "Invalid error rate of '", error_rate, "'." );
        return false;
    }
    if( _reader.getFileName() == _writer.getFileName() ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::start( ", read_length, ", ", read_depth, ", ", error_rate, " )] "
            "Input and Output files are the same ('", _reader.getFileName(), "')!" );
        return false;
    }
    if( !_reader.isOpen() && !_reader.open() ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::start( ", read_length, ", ", read_depth, ", ", error_rate, " )] "
                       "There was a problem opening the genome file." );
        return false;
    }
    if( !_writer.isOpen() && !_writer.open() ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::start( ", read_length, ", ", read_depth, ", ", error_rate, " )] "
                       "There was a problem creating the sequencer file." );
        return false;
    }
    //Calculating total number of reads to do on genome
    uint64_t reads_total = calcReadCount( _reader.size(), read_length, read_depth );
    std::cout << "-> Calculated the number of reads at: " << reads_total << std::endl;
    //Logging stats
    LOG( "[genomeMaker::SequencerSim::start()] Reading from file: '", _reader.getFileName() , "'" );
    LOG( "[genomeMaker::SequencerSim::start()] Read length......: ", read_length );
    LOG( "[genomeMaker::SequencerSim::start()] Total reads......: ", read_depth );
    LOG( "[genomeMaker::SequencerSim::start()] Error rate.......: ", error_rate );
    LOG( "[genomeMaker::SequencerSim::start()] Calculated #reads: ", reads_total );
    LOG( "[genomeMaker::SequencerSim::start()] Writing to file..: '", _writer.getFileName() , "'" );
    //Starting sequencing..
    std::cout << "...Starting..." << std::endl;
    return sequenceGenome( read_length,
                           read_depth,
                           error_rate,
                           reads_total );
}

/**
 * Calculates the read count
 * @param genome_size Genome size in bytes
 * @param read_length Read length in number of chars
 * @param read_depth  Depth of reads
 * @return Total number of reads to do
 */
uint64_t genomeMaker::SequencerSim::calcReadCount( const std::streampos &genome_size,
                                                   const size_t &read_length,
                                                   const size_t &read_depth ) const
{
    //from eq: depth = read_count * read_length / genome_size
    return read_depth * genome_size / read_length;
}

/**
 * Calculates the maximum start index for a read on the buffer
 * @param current_buffer_size Size of the current buffer
 * @param next_buffer_size    Size of the next buffer
 * @param read_length         Length of a read
 * @return Maximum start index for a read
 * @throws std::out_of_range when the current buffer size is smaller than a read length
 */
size_t genomeMaker::SequencerSim::calcMaxIndex( const std::streamsize &current_buffer_size,
                                                const std::streamsize &next_buffer_size,
                                                const size_t &read_length ) const {
    //Error control
    if( read_length > current_buffer_size ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::calcMaxIndex( ", current_buffer_size, ", ", next_buffer_size, ", ", read_length, " )] "
            "Buffer size is too small for a full length read to happen." );
        throw std::out_of_range( "Current buffer is too small for a read!" );
    }
    if( next_buffer_size > read_length - 1 ) {
        return current_buffer_size > 0 ? (size_t) current_buffer_size - 1 : 0;
    } else {
        return current_buffer_size - read_length + next_buffer_size;
    }
}

/**
 * Run the sequencer simulation on the provided genome file
 * @param read_length Length of reads
 * @param read_depth  Depth of the reads
 * @param error_rate  Error rate on reads
 * @param reads_total Total number of reads to do on genome
 * @return Success
 */
bool genomeMaker::SequencerSim::sequenceGenome( const size_t &read_length,
                                                const size_t &read_depth,
                                                const double &error_rate,
                                                const uint64_t &reads_total ) {
    //Setting things up
    uint64_t       reads_completed  { 0 };
    const size_t   chunk_size       { read_length * 4 };
    const uint64_t genome_size      { _reader.size() > 0 ? (uint64_t) _reader.size() : 0 };
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Size of unit of work from file.: ", chunk_size );
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Genome size (#chars)...........: ", genome_size );


    //TODO not sure about this block...
    const uint64_t genome_chunks    { ( genome_size / chunk_size ) };
    u_int64_t      processed_chunks { 0 };

    const uint64_t reads_per_chunk { reads_total / ( genome_size / chunk_size ) }; //TODO work out number of reads for the chunk!!*
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Number of whole units..........: ", genome_chunks );
    //LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Size of remainder left (#chars): ", chunks_remainder );
    LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Number of reads per unit ......: ", reads_per_chunk );

    Buffers buffer;
    //TODO *sort out even distribution of reads (count) on chunks inc remainders
    //TODO integrate error rate (might be an idea to send it to chunk after calculating the range from the total number of reads?
    //_error_randomiser.setPoolRange( 0, 0 );

    buffer._current_size = _reader.read( *buffer._current, chunk_size );
    bool data_remaining_flag { true };
    do {
        if( buffer._current_size < 1 || buffer._current_size < read_length) { //Got nothing from read || not enough to do a full length read
            data_remaining_flag = false;
        } else {
            processed_chunks++;
            LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Processing chunk #", processed_chunks , "/", genome_chunks );
            buffer._next_size = _reader.read( *buffer._next, chunk_size );
            if( buffer._current_size < chunk_size ) { //EOF reached during 'current' read
                if( !sequenceGenomeChunk( read_length, reads_per_chunk, reads_completed, buffer ) ) {
                    LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenome( ", read_length, ", ", read_depth, ", ", error_rate, ", ", reads_total, " )] "
                        "Problem occurred whilst processing genome chunk #", processed_chunks, "/", genome_chunks, ". (EOF chunk)" );
                    return false;
                }
                data_remaining_flag = false;
            } else if( buffer._next_size < chunk_size ) { //EOF reached during 'next' read
                LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] EOF reached whilst caching 'next' buffer. Merging buffers..." );
                buffer._current->insert( buffer._current->end(), buffer._next->begin(), buffer._next->end() ); //concatenating 'next' into 'current'
                buffer._current_size += buffer._next_size;
                buffer._next_size = 0;
                if( !sequenceGenomeChunk( read_length, reads_per_chunk, reads_completed, buffer ) ) {
                    LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenome( ", read_length, ", ", read_depth, ", ", error_rate, ", ", reads_total, " )] "
                        "Problem occurred whilst processing genome chunk #", processed_chunks, "/", genome_chunks, ". (EOF merged chunk)" );
                    return false;
                }
                data_remaining_flag = false;
            } else { //Both 'current' and 'next' buffers are fully filled
                if( !sequenceGenomeChunk( read_length, reads_per_chunk, reads_completed, buffer ) ) {
                    LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenome( ", read_length, ", ", read_depth, ", ", error_rate, ", ", reads_total, " )] "
                        "Problem occurred whilst processing genome chunk ", processed_chunks, "/", genome_chunks, ". (STD chunk)" );
                    return false;
                }
                buffer.swapBuffers();
            }
        }
    } while( data_remaining_flag );
}

/**
 * Sequences a genome chunk
 * @param read_length           Length of reads
 * @param read_count            Number of reads to do on the chunk
 * @param total_reads_completed Total number of reads done on the whole genome
 * @param buffer                Chunk of the genome to sequence
 * @param buffer_data_size      Size of data to sequence from the genome chunk
 * @return Success
 */
bool genomeMaker::SequencerSim::sequenceGenomeChunk( const size_t &read_length,
                                                     const uint64_t &read_count,
                                                     uint64_t &total_reads_completed,
                                                     Buffers &buffer ) {
    try {
        //so if we have enough on the next buffer to have a full read starting at back() of current we are golden
        //if not then we need to adjust the max start index
        size_t max_begin_index = calcMaxIndex( buffer._current_size, buffer._next_size, read_length );
        _read_randomiser.setPoolRange( 0, max_begin_index ); //so that we get full read_length reads only
        LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenomeChunk(..)] Current buffer size..: ", buffer._current_size );
        LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenomeChunk(..)] Next buffer size.....: ", buffer._next_size );
        LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenomeChunk(..)] Reads to do on chunk.: ", read_count );
        LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenomeChunk(..)] Pool range set to....: 0-", max_begin_index );
        //Sequencing...
        uint64_t reads_done = { 0 };
        while( reads_done < read_count ) {
            //Getting read from buffer
            size_t start_index = _read_randomiser.getRand();
            std::stringstream ss;
            ss << ">read#" << total_reads_completed << "\n";
            for( size_t i = 0; i < read_length; i++ ) {
                if( i > 0 && i % _LINE_SIZE == 0 ) {
                    ss << "\n";
                }
                //TODO inject errors (error % variable needed)
                if( start_index + i < buffer._current_size ) {
                    ss << buffer._current->at( start_index + i );
                } else {
                    ss << buffer._next->at( start_index + i - buffer._current_size );
                }
            }
            ss << "\n\n";
            //Writing read to sequencer file
            if( !_writer.write( ss.str())) {
                LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenomeChunk(..)] Error occurred whilst writing read ",
                           reads_done, "/", read_count, " of genome chunk to file '", _writer.getFileName(), "'." );
                std::cerr << "Error: could not write read #" << total_reads_completed << " to sequencer file: " << std::endl;
                std::cerr << ss.str();
                std::cerr << "Aborting..." << std::endl;
                return false;
            }
            reads_done++;
            total_reads_completed++;
        }
        return true;
    } catch ( std::out_of_range e ) {
        return false;
    }
}
