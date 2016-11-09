/**
   Component Name:  EADlib.io.FileStats
   Language:        C++14

   License: GNUv2 Public License
   (c) Copyright E. A. Davison 2016

   Author: E. A. Davison

   Description: Fetches file description from unix file systems [POSIX]
                BEWARE: Do not rely upon within race conditions.
                (i.e. when queried object may be changed by other processes/threads in the meantime)
   Dependencies: EADlib.Logger
**/

#ifndef EADLIB_FILESTATS_H
#define EADLIB_FILESTATS_H

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <chrono>

#include "../logger/Logger.h"

namespace eadlib {
    namespace io {
        class FileStats {
          public:
            FileStats( const std::string &file_name );
            ~FileStats();
            friend std::ostream &operator <<( std::ostream &os, const FileStats &file_stats );
            bool getStats();
            //Properties
            bool isValid() const;
            off_t getSize() const;
            std::string getType() const;
            bool isReadable() const;
            bool isWritable() const;
            bool isExecutable() const;
            bool isSocket() const;
            bool isSymLink() const;
            bool isDirectory() const;
            bool isFile() const;
            //IDs
            uid_t getUserID() const;
            std::string getUserName() const;
            gid_t getGroupID() const;
            std::string getGroupName() const;
            dev_t getDeviceID() const;
            //Times stamps
            std::time_t getLastAccess() const;
            std::string getLastAccessStr() const;
            std::time_t getLastModification() const;
            std::string getLastModificationStr() const;
            std::time_t getLastStatusChange() const;
            std::string getLastStatusChangeStr() const;
          private:
            std::string getErrorDescription( const int &error_number ) const;
            bool _valid_flag;
            std::string _file_name;
            struct stat _file_stats;
        };
        //--------------------------------------------------------------------------------------------------------------------------------------------
        // FileStats class public method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         * @param file_name File name
         */
        inline FileStats::FileStats( const std::string &file_name ) :
            _valid_flag( false ),
            _file_name( file_name )
        {
            if( getStats() ) {
                _valid_flag = true;
            } else {
                LOG_ERROR( "[eadlib::io::posix::FileStats( ", file_name, "' )] Problem getting stats.");
            }
        }

        /**
         * Destructor
         */
        inline FileStats::~FileStats() {}

        /**
         * Stream out operator
         * @param os Output stream
         * @param fs FileStats object
         * @return Output stream
         */
        inline std::ostream & operator <<( std::ostream &os, const FileStats &fs ) {
            if( fs._valid_flag ) {
                os << "File..............: " << fs._file_name << "\n";
                os << "Type..............: " << fs.getType() << "\n";
                os << "I-Node #..........: " << fs._file_stats.st_ino << "\n";
                os << "Mode..............: " << fs._file_stats.st_mode << "\n";
                os << "Link count........: " << fs._file_stats.st_nlink << "\n";
                os << "Owner.............: " << fs.getUserName() << " [" << fs._file_stats.st_uid << "]\n";
                os << "Group.............: " << fs.getGroupName() << " [" << fs._file_stats.st_gid << "]\n";
                os << "Size..............: " << fs.getSize() << ( fs.getSize() > 1 ? " bytes" : " byte" ) << "\n";
                os << "I/O Block size....: " << fs._file_stats.st_blksize << "\n";
                os << "Blocks allocated..: " << fs._file_stats.st_blocks << "\n";
                os << "File permissions..: ";
                os << (S_ISDIR( fs._file_stats.st_mode ) ? "d" : "-" );
                os << ( fs._file_stats.st_mode & S_IRUSR ? "r" : "-" );
                os << ( fs._file_stats.st_mode & S_IWUSR ? "w" : "-" );
                os << ( fs._file_stats.st_mode & S_IXUSR ? "x" : "-" );
                os << ( fs._file_stats.st_mode & S_IRGRP ? "r" : "-" );
                os << ( fs._file_stats.st_mode & S_IWGRP ? "w" : "-" );
                os << ( fs._file_stats.st_mode & S_IXGRP ? "x" : "-" );
                os << ( fs._file_stats.st_mode & S_IROTH ? "r" : "-" );
                os << ( fs._file_stats.st_mode & S_IWOTH ? "w" : "-" );
                os << ( fs._file_stats.st_mode & S_IXOTH ? "x" : "-" ) << "\n";
                os << "Device ID.........: " << fs.getDeviceID() << "\n";
                os << "Last status change: " << fs.getLastStatusChangeStr() << "\n";
                os << "Last access.......: " << fs.getLastAccessStr() << "\n";
                os << "Last modifications: " << fs.getLastModificationStr();
            } else {
                os << "Object '" << fs._file_name << "' not found.";
            }
            return os;
        }

        /**
         * Refreshes the data of the file
         * @return Success accessing the file
         */
        inline bool FileStats::getStats() {
            if(  stat( _file_name.c_str(), &_file_stats ) < 0 ) {
                _valid_flag = false;
                LOG_ERROR( "[eadlib::io::posix::FileStats::getStats()] Error accessing '", _file_name, "': ",
                           getErrorDescription( errno ) );
            } else {
                _valid_flag = true;
            }
            return _valid_flag;
        }

        /**
         * Gets the validity state of the object
         * @return Object validity
         */
        inline bool FileStats::isValid() const {
            return _valid_flag;
        }

        /**
         * Gets the size in bytes of the file
         * @return Size in bytes
         */
        inline off_t FileStats::getSize() const {
            if( isValid() ) {
                return _file_stats.st_size;
            } else {
                return -1;
            }
        }

        /**
         * Gets the type of object
         * @return Type
         */
        inline std::string FileStats::getType() const {
            switch( _file_stats.st_mode & S_IFMT ) {
                case S_IFREG:
                    return "Regular file";
                case S_IFDIR:
                    return "Directory";
                case S_IFCHR:
                    return "Character device";
                case S_IFBLK:
                    return "Block device";
                case S_IFIFO:
                    return "FIFO (named pipe)";
                case S_IFLNK:
                    return "Symbolic link";
                case S_IFSOCK:
                    return "Socket";
                default:
                    return "Unknown";
            }
        }

        /**
         * Gets the Readability state
         * @return Readable state
         */
        inline bool FileStats::isReadable() const {
            return ( _file_stats.st_mode & S_IRUSR ) > 0;
        }

        /**
         * Gets the Writability state
         * @return Writable state
         */
        inline bool FileStats::isWritable() const {
            return ( _file_stats.st_mode & S_IWUSR ) > 0;
        }

        /**
         * Gets the Executability state
         * @return Executable state
         */
        inline bool FileStats::isExecutable() const {
            return ( _file_stats.st_mode & S_IXUSR ) > 0;
        }

        /**
         * Checks if object is a socket
         * @return Socket state
         */
        inline bool FileStats::isSocket() const {
            return ( _file_stats.st_mode & S_IFSOCK ) > 0;
        }

        /**
         * Checks if object is a symbolic link
         * @return Symbolic link state
         */
        inline bool FileStats::isSymLink() const {
            return ( _file_stats.st_mode & S_IFLNK ) > 0;
        }

        /**
         * Checks if object is a directory
         * @return Directory state
         */
        inline bool FileStats::isDirectory() const {
            return ( _file_stats.st_mode & S_IFDIR ) > 0;
        }

        /**
         * Checks if object is a file
         * @return File state
         */
        inline bool FileStats::isFile() const {
            return ( _file_stats.st_mode & S_IFREG ) > 0;
        }

        /**
         * Gets the User ID
         * @return User ID register
         */
        inline uid_t FileStats::getUserID() const {
            return _file_stats.st_uid;
        }

        /**
         * Gets the Username
         * @return Username
         */
        inline std::string FileStats::getUserName() const {
            std::string username { "" };
            if( isValid() ) {
                register struct passwd *pw = getpwuid( getUserID() );
                if( pw ) {
                    username = std::string( pw->pw_name );
                }
            }
            return username;
        }

        /**
         * Gets the Group ID
         * @return Group ID register
         */
        inline gid_t FileStats::getGroupID() const {
            return _file_stats.st_gid;
        }

        /**
         * Gets the Groupname
         * @return Groupname
         */
        inline std::string FileStats::getGroupName() const {
            std::string groupname { "" };
            if( isValid() ) {
                register struct group *gp = getgrgid( getGroupID());
                if( gp ) {
                    groupname = std::string( gp->gr_name );
                }
            }
            return groupname;
        }

        /**
         * Gets the Device ID
         * @return Device ID register
         */
        inline dev_t FileStats::getDeviceID() const {
            return _file_stats.st_dev;
        }

        /**
         * Gets the time point for the last access
         * @return Last access time point
         */
        inline std::time_t FileStats::getLastAccess() const {
            return std::time_t( _file_stats.st_atim.tv_sec );
        }

        /**
         * Gets the time point for the last access as a string
         * @return Last access time string
         */
        inline std::string FileStats::getLastAccessStr() const {
            std::time_t time = getLastAccess();
            std::string time_string = std::ctime( &time );
            time_string.pop_back();
            return time_string;
        }

        /**
         * Gets the time point for the last modification
         * @return Last modification time point
         */
        inline std::time_t FileStats::getLastModification() const {
            return std::time_t( _file_stats.st_mtim.tv_sec );
        }

        /**
         * Gets the time point for the last modification as a string
         * @return Last modification time string
         */
        inline std::string FileStats::getLastModificationStr() const {
            std::time_t time = getLastModification();
            std::string time_string = std::ctime( &time );
            time_string.pop_back();
            return time_string;
        }

        /**
         * Gets the time point for the last status change
         * @return Last status change time point
         */
        inline std::time_t FileStats::getLastStatusChange() const {
            return std::time_t( _file_stats.st_ctim.tv_sec );
        }

        /**
         * Gets the time point for the last status change as a string
         * @return Last statis change time string
         */
        inline std::string FileStats::getLastStatusChangeStr() const {
            std::time_t time = getLastStatusChange();
            std::string time_string = std::ctime( &time );
            time_string.pop_back();
            return time_string;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // FileStats class private method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Gets the description associated with an error number
         * @param error_number Error number
         * @return Description
         */
        inline std::string FileStats::getErrorDescription( const int &error_number ) const {
            switch( error_number ) {
                case EACCES:
                    return "Search permission is denied for one of the directories in the path prefix of path.";
                case EBADF:
                    return "'filedes' is bad.";
                case EFAULT:
                    return "Bad address.";
                case ELOOP:
                    return "Too many symbolic links encountered while traversing the path.";
                case ENAMETOOLONG:
                    return "File name too long.";
                case ENOENT:
                    return "A component of the path path does not exist, or the path is an empty string.";
                case ENOMEM:
                    return "Out of memory (i.e. kernel memory).";
                case ENOTDIR:
                    return "A component of the path is not a directory.";
                default:
                    return "Unknown error.";
            }
        }
    }
}

#endif //EADLIB_FILESTATS_H
