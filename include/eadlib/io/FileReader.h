/**
   Component Name:  EADlib.io.FileReader
   Language:        C++14

   License: GNUv2 Public License
   (c) Copyright E. A. Davison 2016

   Author: E. A. Davison

   Description: File reader (plain text)
   Dependencies: EADlib.Logger
**/

#ifndef EADLIB_FILEREADER_H
#define EADLIB_FILEREADER_H

#include <fstream>
#include <memory>
#include <errno.h>
#include "../logger/Logger.h"

namespace eadlib {
    namespace io {
        class FileReader {
          public:
            FileReader( const std::string &file_name );
            FileReader( const FileReader &reader ) = delete;
            FileReader( FileReader &&reader );
            ~FileReader();

            bool open();
            void close();
            bool reset();

            std::streamsize read( std::vector<char> &buffer, const size_t &block_size );
            std::streamsize readLine( std::vector<char> &buffer );

            bool isOpen() const;
            bool isDone() const;
            std::streampos size() const;
            std::streampos getPosition() const;
            std::string getFileName() const;

          private:
            bool reachedEOF( const std::streampos &pos ) const;
            std::string _file_name;
            std::unique_ptr<std::ifstream> _input_stream;
            std::streampos _size;
            std::streampos _cursor;
            bool _completed_read;
        };

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // FileReader class public method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         * @param file_name File name
         */
        inline FileReader::FileReader( const std::string &file_name ) :
            _file_name( file_name ),
            _input_stream( new std::ifstream ),
            _size( -1 ),
            _cursor( -1 ),
            _completed_read( false )
        {}

        /**
         * Move-Constructor
         * @param reader FileReader to move over
         */
        inline FileReader::FileReader( FileReader &&reader ) :
            _file_name( reader._file_name ),
            _input_stream( reader._input_stream.release() ),
            _size( reader._size ),
            _cursor( reader._cursor ),
            _completed_read( false )
        {}

        /**
         * Destructor
         */
        inline FileReader::~FileReader() {
            if( _input_stream ) {
                _input_stream->close();
            }
        }

        /**
         * Opens the input stream to the file
         * @return Success
         */
        inline bool FileReader::open() {
            _input_stream->open( _file_name.c_str() );
            if( !_input_stream->is_open() ) {
                LOG_FATAL( "[eadlib::io::FileReader::open( ", _file_name, " )] "
                    "Unable to open stream to file: ", strerror( errno ) );
                return false;
            }
            _input_stream->seekg( 0, _input_stream->end );
            _size = _input_stream->tellg();
            _input_stream->seekg( 0, _input_stream->beg );
            _input_stream->unsetf( std::ios_base::skipws );
            if( _size < 1 ) {
                LOG_ERROR( "[eadlib::io::FileReader::open( ", _file_name, " )] "
                    "File empty or couldn't get size: ", strerror( errno ) );
                _completed_read = true;
            }
            return true;
        }

        /**
         * Closes the input stream to the file
         */
        inline void FileReader::close() {
            if( _input_stream->is_open()) {
                _input_stream->close();
            }
            if( !_input_stream->is_open() ) {
                _input_stream->clear();
                _size = -1;
                _cursor = -1;
            }
        }

        /**
         * Resets cursor position in file back to the beginning
         */
        inline bool FileReader::reset() {
            if( !_input_stream->is_open() ) {
                LOG_ERROR( "[eadlib::io::FileReader::reset()] "
                               "Input stream to '", _file_name, "' is not open." );
                return false;
            }
            _input_stream->clear();
            _input_stream->seekg( 0, _input_stream->end );
            _size = _input_stream->tellg();
            _input_stream->seekg( 0, _input_stream->beg );
            _cursor = -1;
            _completed_read = false;
            if( _input_stream->fail() || _input_stream->bad() ) {
                LOG_ERROR( "[eadlib::io::FileReader::reset()] "
                               "Problem resetting '", _file_name, "' input stream: ", strerror( errno ) );
                return false;
            }
            return true;
        }

        /**
         * Reads a block of text into a buffer
         * @param buffer Text buffer
         * @param block_size Size of the block
         * @return Size of the text read into buffer
         */
        inline std::streamsize FileReader::read( std::vector<char> &buffer, const size_t &block_size ) {
            if( !_input_stream->is_open() ) {
                LOG_ERROR( "[eadlib::io::FileReader::read( <buffer>, ", block_size, " )] "
                    "Input stream to '", _file_name, "' is not open." );
                return -1;
            }
            if( _completed_read ) {
                LOG_ERROR( "[eadlib::io::FileReader::read( <buffer>, ", block_size, " )] "
                    "Read of '", _file_name, "' is already completed. Reset() to read again." );
                return -1;
            }
            if( block_size > buffer.size() ) {
                buffer.resize( block_size, ' ' );
            }std::streamoff s { 0 };
            _input_stream->read( buffer.data(), block_size );
            s = _input_stream->gcount();
            if( reachedEOF( _cursor + s ) ) {
                _completed_read = true;
            }
            _cursor = _input_stream->tellg();

            if( _input_stream->bad() ) {
                LOG_ERROR( "[eadlib::io::FileReader::read( <buffer>, ", block_size, " )] "
                    "Problem occurred reading the input stream of '", _file_name, "': ", strerror( errno ) );
            }
            return s;
        }

        /**
         * Reads into the buffer up to the end of the line ('\n')
         * @param buffer Text buffer
         * @return Size of text read into buffer
         */
        inline std::streamsize FileReader::readLine( std::vector<char> &buffer ) {
            if( !_input_stream->is_open() ) {
                LOG_ERROR( "[eadlib::io::FileReader::readLine()] Input stream to '", _file_name, "' is not open." );
                return -1;
            }
            if( reachedEOF( _cursor ) ) {
                LOG_ERROR( "[eadlib::io::FileReader::readLine()] Reached EOF. No more data." );
                return -1;
            }
            std::streamoff char_count { 0 };
            buffer.clear();
            bool eol_flag { false };
            char character { ' ' };
            while( !eol_flag ) {
                if( _input_stream->get( character ) ) {
                    if( character == '\n' ) {
                        eol_flag = true;
                    } else {
                        buffer.emplace_back( character );
                        char_count++;
                    }
                } else {
                    if( _input_stream->bad() ) {
                        LOG_ERROR( "[eadlib::io::FileReader::readLine()] Problem occurred reading the input stream of '",
                                   _file_name, "' at position '", _cursor + char_count, "': ", strerror( errno ) );
                    }
                    if( _input_stream->eof() ) {
                        eol_flag = true;
                    }
                }
            }
            _completed_read = reachedEOF( _cursor + char_count );
            _cursor = _input_stream->tellg();
            return char_count;
        }

        /**
         * Gets the open status of the file stream
         * @return Open state
         */
        inline bool FileReader::isOpen() const {
            return _input_stream->is_open();
        }

        /**
         * Checks if the entirerity of the file has been read
         * @return Full read completed state
         */
        inline bool FileReader::isDone() const {
            return _completed_read;
        }

        /**
         * Gets the size of the file
         * @return Size of file
         */
        inline std::streampos FileReader::size() const {
            if( !_input_stream->is_open() ) {
                LOG_ERROR( "[eadlib::io::FileReader::size()] Trying to get size from unopened stream." );
            }
            return _size;
        }

        /**
         * Gets the current cursor position in the file
         * @return Cursor position
         */
        inline std::streampos FileReader::getPosition() const {
            if( !_input_stream->is_open() ) {
                LOG_ERROR( "[eadlib::io::FileReader::getPosition()] Trying to get cursor position of unopened stream." );
            }
            return _cursor;
        }

        /**
         * Gets the file name of the reader
         * @return File name
         */
        inline std::string FileReader::getFileName() const {
            return _file_name;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // FileReader class private method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Checks if EOF has been reached
         * @param pos Position cursor
         * @return End of file reached state
         */
        inline bool FileReader::reachedEOF( const std::streampos &pos ) const {
            if( _input_stream->eof() ) {
                if( pos != _size ) {
                    LOG_ERROR( "[eadlib::io::FileReader::reachedEOF( ", pos, " )] "
                        "Reached EOF. Cursor position and recorded size are different (", pos, "/", _size, "). "
                        "File may have been modified by another process." );
                } else {
                    LOG_DEBUG( "[eadlib::io::FileReader::reachedEOF( ", pos, " )] Reached EOF." );
                }
                return true;
            }
            return false;
        }
    }
}

#endif //EADLIB_FILEREADER_H
