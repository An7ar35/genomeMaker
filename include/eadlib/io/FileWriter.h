/**
   Component Name:  EADlib.io.FileWriter
   Language:        C++14

   License: GNUv2 Public License
   (c) Copyright E. A. Davison 2016

   Author: E. A. Davison

   Description: File writer (plain text)
   Dependencies: EADlib.Logger
**/

#ifndef EADLIB_FILEWRITER_H
#define EADLIB_FILEWRITER_H

#include <fstream>
#include <memory>
#include "../logger/Logger.h"

namespace eadlib {
    namespace io {
        class FileWriter {
          public:
            FileWriter( const std::string &file_name );
            FileWriter( const FileWriter &writer ) = delete;
            FileWriter( FileWriter &&writer );
            ~FileWriter();

            bool open( const bool &overwrite_flag = false );
            void close();

            template<class T> bool write( const T &value );
            bool flush();

            bool isOpen();
            std::string getFileName();
          private:
            std::string _file_name;
            std::unique_ptr<std::ofstream> _output_stream;
        };

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // FileWriter class public method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         * @param file_name File name
         */
        inline FileWriter::FileWriter( const std::string &file_name ) :
            _file_name( file_name ),
            _output_stream( new std::ofstream )
        {}

        /**
         * Move-Constructor
         * @param writer FileWriter to move over
         */
        inline FileWriter::FileWriter( FileWriter &&writer ) :
            _file_name( writer._file_name ),
            _output_stream( writer._output_stream.release() )
        {}

        /**
         * Destructor
         */
        inline FileWriter::~FileWriter() {
            if( _output_stream ) {
                _output_stream->close();
            }
        }

        /**
         * Opens the output stream to the file
         * |-Default behaviour appends from the end of the file.
         * |-When overwrite flag is set the previous file content is discarded.
         * @param overwrite_flag Overwrite flag (default=false)
         * @return Success
         */
        inline bool FileWriter::open( const bool &overwrite_flag ) {
            if( _output_stream->is_open() ) {
                LOG_ERROR( "[eadlib::io::FileWriter::open( ", _file_name, " )] "
                    "Output stream to file '", _file_name, "' is already opened." );
                return false;
            }
            if( overwrite_flag ) {
                _output_stream->open( _file_name.c_str(), std::ofstream::out | std::ofstream::trunc );
            } else {
                _output_stream->open( _file_name.c_str(), std::ofstream::out | std::ofstream::ate | std::ofstream::app );
            }
            if( !_output_stream->is_open() ) {
                LOG_ERROR( "[eadlib::io::FileWriter::open( ", _file_name, " )] Unable to open stream to file." );
                return false;
            }
            return true;
        }

        /**
         * Closes the output stream to the file
         */
        inline void FileWriter::close() {
            if( _output_stream->is_open() ) {
                _output_stream->close();
            }
            if( !_output_stream->is_open() ) {
                _output_stream->clear();
            }
        }

        /**
         * Writes a value to the file
         * @param value Value to write
         */
        template<class T> bool FileWriter::write( const T &value ) {
            ( *_output_stream ) << value;
            if( _output_stream->bad() || _output_stream->fail() ) {
                LOG_ERROR( "[eadlib::io::FileWriter::write( ", value, " )] Problem writing to file '", _file_name, "': ",
                           strerror(errno) );
                return false;
            }
            return true;
        }

        /**
         * Flushes the output stream
         * @return Success
         */
        inline bool FileWriter::flush() {
            if( !_output_stream->is_open() ) {
                LOG_ERROR( "[eadlib::io::FileWriter::flush()] Trying to flush an unopened stream to '", _file_name );
                return false;
            }
            _output_stream->flush();
            if( _output_stream->bad() || _output_stream->fail() ) {
                LOG_ERROR( "[eadlib::io::FileWriter::flush()] Problem flushing stream to file '", _file_name, "': ",
                           strerror(errno) );
                return false;
            }
            return true;
        }

        /**
         * Gets the open status of the file stream
         * @return Open state
         */
        inline bool FileWriter::isOpen() {
            return _output_stream->is_open();
        }

        /**
         * Gets the file name of the writer
         * @return File name
         */
        inline std::string FileWriter::getFileName() {
            return _file_name;
        }
    }
}

#endif //EADLIB_FILEWRITER_H
