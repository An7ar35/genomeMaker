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
    _error_randomiser( error_randomiser ),
    _total_reads_completed( 0 ),
    _total_read_errors( 0 )
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
    uint64_t error_bound = calcErrorUpperBound( reads_total, error_rate );
    _error_randomiser.setPoolRange( error_bound, reads_total );
    std::cout << "-> Calculated the number of reads at: ~" << reads_total << std::endl;
    //Logging stats
    LOG( "[genomeMaker::SequencerSim::start(..)] Reading from file.: '", _reader.getFileName() , "'" );
    LOG( "[genomeMaker::SequencerSim::start(..)] Read length.......: ", read_length );
    LOG( "[genomeMaker::SequencerSim::start(..)] Depth of reads....: ", read_depth );
    LOG( "[genomeMaker::SequencerSim::start(..)] Error rate........: ", error_rate );
    LOG( "[genomeMaker::SequencerSim::start(..)] Calculated #reads.: ~", reads_total );
    LOG( "[genomeMaker::SequencerSim::start(..)] Calculated #errors: ~", error_bound, "/", reads_total );
    LOG( "[genomeMaker::SequencerSim::start(..)] Writing to file...: '", _writer.getFileName() , "'" );
    //Starting sequencing..
    _total_reads_completed = 0;
    _total_read_errors     = 0;
    std::cout << "...Starting..." << std::endl;
    return sequenceGenome( read_length, read_depth, reads_total, error_bound );
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
 * Calculates the number of read errors over the total number of reads based on the rate given
 * @param reads_total Total number of reads
 * @param error_rate  Error rate on the reads
 * @return Upper bound number for the eroneous reads
 */
uint64_t genomeMaker::SequencerSim::calcErrorUpperBound( const uint64_t &reads_total, const double &error_rate ) const {

    //return reads_total * error_rate; //TODO terrible!
    return 0;
}

/**
 * Calculates the number of reads to do on a chunk
 * @param genome_size Size of the genome (#chars)
 * @param genome_chunks Number of complete chunks the genome is broken into
 * @param chunk_size Size of chunk
 * @return Number of reads on the chunk
 */
uint64_t genomeMaker::SequencerSim::calcChunkReads( const uint64_t &genome_size,
                                                    const uint64_t &reads_total,
                                                    const uint64_t &genome_chunks,
                                                    const size_t &chunk_size ) const {
    uint64_t reads = ( chunk_size * 100 / genome_size ) * reads_total / 100; //even-distribution of reads
    if( reads > 0 ) {
        LOG_DEBUG( "[genomeMaker::SequencerSim::calcChunkReads( ", genome_size, ", ", reads_total, ", ", genome_chunks, ", ", chunk_size, " )] "
            "Even-distribution of reads for chunk size'", chunk_size, "': ", reads );
        return reads;
    }
    uint64_t reads_alt = reads_total / genome_chunks; //rough distribution of reads
    if( reads_alt > 0 ) {
        LOG_DEBUG( "[genomeMaker::SequencerSim::calcChunkReads( ", genome_size, ", ", reads_total, ", ", genome_chunks, ", ", chunk_size, " )] "
            "Rough distribution of reads for chunk size '", chunk_size, "': ", reads_alt );
        return reads_alt;
    }
    LOG_ERROR( "[genomeMaker::SequencerSim::calcChunkReads( ", genome_size, ", ", reads_total, ", ", genome_chunks, ", ", chunk_size, " )] "
        "Could not calculate a number of reads for chunk." );
    return 0;
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
 * @param error_total Total number of errors to inject into reads
 * @param reads_total Total number of reads to do on genome
 * @return Success
 */
bool genomeMaker::SequencerSim::sequenceGenome( const size_t &read_length,
                                                const size_t &read_depth,
                                                const uint64_t &reads_total,
                                                const uint64_t &erroneous_reads ) {
    //Setting things up
    const size_t   chunk_size       { read_length * 4 };
    const uint64_t genome_size      { _reader.size() > 0 ? (uint64_t) _reader.size() : 0 };
    const uint64_t genome_chunks    { genome_size / chunk_size };
    u_int64_t      processed_chunks { 0 };
    LOG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Genome size (#chars)...........: ", genome_size );
    LOG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Size of genome chunks from file: ", chunk_size );
    LOG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Number of chunks to process....: ", genome_chunks );
    LOG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Size of remainder for chunks...: ", genome_size % chunk_size );

    //Calculating and error checking number of reads per full sized genome chunks
    uint64_t reads_per_chunk = calcChunkReads( genome_size, reads_total, genome_chunks, chunk_size );
    if( reads_per_chunk < 1 ) {
        LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenome(..)] Number of reads calculated ('", reads_per_chunk, "') too low." );
        std::cerr << "Error: Number of reads calculated based on arguments is too low for the size of the genome." << std::endl;
        return false;
    }
    LOG( "[genomeMaker::SequencerSim::sequenceGenome(..)] #reads per whole chunks........: ", reads_per_chunk );

    eadlib::cli::ProgressBar progress( genome_size, 70 );
    progress.printPercentBar( std::cout, 0 );

    Buffers buffer;
    buffer._current_size = _reader.read( *buffer._current, chunk_size );

    bool data_remaining_flag { true };
    do {
        progress.printPercentBar( std::cout, 0 );
        if( buffer._current_size < 1 || buffer._current_size < read_length) { //Got nothing from read || not enough to do a full length read
            data_remaining_flag = false;
        } else {
            processed_chunks++;
            LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Processing chunk #", processed_chunks , "/", genome_chunks );
            buffer._next_size = _reader.read( *buffer._next, chunk_size );
            if( buffer._current_size < chunk_size ) { //EOF reached during 'current' read
                //This block kept for safety but should never trigger as when the cached 'next' buffer hits EOF it gets merged with the current.
                LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Inside the section where the current buffer read hits EOF on the genome!" );
                if( !sequenceGenomeChunk( read_length, reads_per_chunk, erroneous_reads, buffer )) {
                    LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenome( ", read_length, ", ", read_depth, ", ", reads_total, " )] "
                        "Problem occurred whilst processing genome chunk #", processed_chunks, "/", genome_chunks, ". (EOF chunk)" );
                    return false;
                }
                data_remaining_flag = false;
            } else if( buffer._next_size < chunk_size ) { //EOF reached during 'next' read
                LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] EOF reached whilst caching 'next' buffer." );
                LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Merging buffers... ", buffer._current_size, " + ", buffer._next_size );
                //concatenating 'next' buffer into 'current'
                buffer._current->insert( buffer._current->end(), buffer._next->begin(), buffer._next->end() );
                buffer._current_size += buffer._next_size;
                buffer._next_size = 0;
                //getting number of reads for merged chunks
                size_t   merged_size     = chunk_size + ( genome_size % chunk_size );
                uint64_t reads_on_merged = calcChunkReads( genome_size, reads_total, genome_chunks, merged_size );
                if( reads_on_merged < 1 ) {
                    LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenome( ", read_length, ", ", read_depth, ", ", reads_total, " )] "
                        "could not get a number of reads for genome chunk #", processed_chunks, "/", genome_chunks, ". (EOF merged chunk)" );
                    return false;
                }
                LOG_DEBUG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Number of reads on merged chunk: ", reads_on_merged );
                if( !sequenceGenomeChunk( read_length, reads_on_merged, erroneous_reads, buffer )) {
                    LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenome( ", read_length, ", ", read_depth, ", ", reads_total, " )] "
                        "Problem occurred whilst processing genome chunk #", processed_chunks, "/", genome_chunks, ". (EOF merged chunk)" );
                    return false;
                }
                progress += reads_on_merged;
                data_remaining_flag = false;
            } else { //Both 'current' and 'next' buffers are fully filled
                if( !sequenceGenomeChunk( read_length, reads_per_chunk, erroneous_reads, buffer )) {
                    LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenome( ", read_length, ", ", read_depth, ", ", reads_total, " )] "
                        "Problem occurred whilst processing genome chunk ", processed_chunks, "/", genome_chunks, ". (STD chunk)" );
                    return false;
                }
                buffer.swapBuffers();
                progress += reads_per_chunk;
            }
        }
    } while( data_remaining_flag );

    progress.complete().printPercentBar( std::cout, 0 );
    LOG( "[genomeMaker::SequencerSim::sequenceGenome(..)] Reads completed: ", _total_reads_completed );
    std::cout << "\n-> Total number of reads taken: " << _total_reads_completed << std::endl;
    return true;
}

/**
 * Sequences a genome chunk
 * @param read_length           Length of reads
 * @param read_count            Number of reads to do on the chunk
 * @param total_reads_completed Total number of reads done on the whole genome
 * @param total_errors_injected Total number of errors injected in all the reads
 * @param buffer                Chunk of the genome to sequence
 * @return Success
 */
bool genomeMaker::SequencerSim::sequenceGenomeChunk( const size_t &read_length,
                                                     const uint64_t &read_count,
                                                     const uint64_t &erroneous_reads,
                                                     Buffers &buffer ) {
    //TODO use the error randomiser to check if error read or not
    //TODO then create random error based on read size
    try {
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
            ss << ">read#" << _total_reads_completed << "\n";
            for( size_t i = 0; i < read_length; i++ ) {
                if( i > 0 && i % _LINE_SIZE == 0 ) {
                    ss << "\n";
                }
                //TODO inject errors (error % variable needed)
                if( start_index + i < buffer._current_size ) {
                    ss << buffer._current->at( start_index + i );
                } else { //read overlaps with the 'next' buffer
                    ss << buffer._next->at( start_index + i - buffer._current_size );
                }
            }
            ss << "\n\n";
            //Writing read to sequencer file
            if( !_writer.write( ss.str())) {
                LOG_ERROR( "[genomeMaker::SequencerSim::sequenceGenomeChunk(..)] Error occurred whilst writing read ",
                           reads_done, "/", read_count, " of genome chunk to file '", _writer.getFileName(), "'." );
                std::cerr << "Error: could not write read #" << _total_reads_completed << " to sequencer file: " << std::endl;
                std::cerr << ss.str();
                std::cerr << "Aborting..." << std::endl;
                return false;
            }
            reads_done++;
            _total_reads_completed++;
        }
        return true;
    } catch ( std::out_of_range e ) {
        return false;
    }
}
