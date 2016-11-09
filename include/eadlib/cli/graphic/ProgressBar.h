/**
   Component Name:  EADlib.cli.ProgressBar
   Language:        C++14

   License: GNUv2 Public License
   (c) Copyright E. A. Davison 2016

   Author: E. A. Davison

   Description: CLI progress bar
   Dependencies:
**/

#ifndef EADLIB_PROGRESSBAR_H
#define EADLIB_PROGRESSBAR_H

#include <iostream>
#include <ios>

namespace eadlib {
    namespace cli {
        class ProgressBar {
          public:
            ProgressBar( const uint64_t &total_steps, const size_t &width );
            ~ProgressBar();
            friend std::ostream & operator <<( std::ostream &os, const ProgressBar &progress_bar );
            const ProgressBar & operator ++();
            const ProgressBar operator ++( int );
            std::ostream & printSimpleBar( std::ostream &out ) const;
            std::ostream & printPercentBar( std::ostream &out, const int &decimal_precision ) const;
            bool isFinished() const;

          private:
            size_t calcPosition() const;
            double calcPercentage() const;
            void refresh();
            //Variables
            uint64_t    _total_steps;
            uint64_t    _current_step;
            size_t      _bar_width;
            size_t      _progress_position;
            double      _percent_completion;
            std::string _progress_bar;
        };

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // ProgressBar class public method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         * @param total_steps Total number of steps
         * @param width       Physical width of the progress bar
         * @param percents    Add percent display with the bar (+5 will be added to width)
         */
        inline ProgressBar::ProgressBar( const uint64_t &total_steps, const size_t &width ) :
            _total_steps( total_steps > 0 ? total_steps - 1 : total_steps ),
            _current_step( 0 ),
            _progress_position( 0 ),
            _percent_completion( 0 ),
            _bar_width( width > 0 ? width - 1 : width ),
            _progress_bar( width, ' ' )
        {
            refresh();
        }

        /**
         * Destructor
         */
        inline ProgressBar::~ProgressBar() {
        }

        /**
         * Output stream operator
         * @param os           Output stream
         * @param progress_bar ProgressBar object
         * @return Output stream
         */
        inline std::ostream & operator <<( std::ostream &os, const ProgressBar &progress_bar ) {
            return progress_bar.printPercentBar( os, 0 );
        }

        /**
         * Increments the current step up
         * @return ProgressBar object post-increment
         */
        inline const ProgressBar & ProgressBar::operator ++() {
            if( !isFinished() ) {
                _current_step++;
                _progress_position  = calcPosition();
                _percent_completion = calcPercentage();
                refresh();
            }
            return *this;
        }

        /**
         * Increments the current step up
         * @return ProgressBar object pre-increment
         */
        inline const ProgressBar ProgressBar::operator ++( int ) {
            ProgressBar original = *this;
            ++( *this );
            return original;
        }

        /**
         * Gets the current progress bar
         * @return Progress bar
         */
        inline std::ostream & ProgressBar::printSimpleBar( std::ostream &out ) const {
            out << "\r" << _progress_bar << std::flush;
            return out;
        }

        /**
         * Gets the current progress bar with percentage
         * @param decimal_precision Number of decimals to the percentage
         * @return Progress bar
         */
        inline std::ostream & ProgressBar::printPercentBar( std::ostream &out, const int &decimal_precision ) const {
            out << "\r" << _progress_bar << " ";
            if( isFinished() ) {
                out << "100\% \b";
                if( decimal_precision > 0 ) {
                    out << ".";
                    for( size_t i = 0; i < decimal_precision; i++ ) {
                        out << "0";
                    }
                }
            } else {
                if( _percent_completion < 10 ) {
                    out << " ";
                }
                if( _percent_completion < 100 ) {
                    out << " ";
                }
                out << std::fixed;
                out << std::setprecision( decimal_precision ) << _percent_completion << "\% \b";
            }
            out << std::flush;
            return out;
        }

        /**
         * Checks finished state of the progress
         * @return Finished state
         */
        inline bool ProgressBar::isFinished() const {
            return _current_step == _total_steps;
        }

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // ProgressBar class private method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Calculates the progress bar cursor position
         * @return Progress bar cursor position
         */
        inline size_t ProgressBar::calcPosition() const {
            return _current_step * _bar_width / _total_steps;
        }

        /**
         * Calculates the current percentage done
         * @return Percentage completed
         */
        inline double ProgressBar::calcPercentage() const {
            return (double) _current_step * 100 / _total_steps;
        }

        /**
         * Refreshes the cached progress bar
         */
        inline void ProgressBar::refresh() {
            if( _bar_width > 0 ) {
                std::stringstream ss;
                ss << "[";
                for( size_t i = 1; i < _progress_position; i++ ) {
                    ss << "=";
                }
                if( _progress_position < _bar_width ) {
                    ss << "|";
                    for( size_t i = _progress_position; i < _bar_width - 1; i++ ) {
                        ss << " ";
                    }
                }
                ss << "]";
                _progress_bar = ss.str();
            }
        }
    }
}

#endif //EADLIB_PROGRESSBAR_H
