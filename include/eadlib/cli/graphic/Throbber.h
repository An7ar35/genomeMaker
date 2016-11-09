/**
   Component Name:  EADlib.cli.Throbber
   Language:        C++14

   License: GNUv2 Public License
   (c) Copyright E. A. Davison 2016

   Author: E. A. Davison

   Description: CLI progress throbber
   Dependencies:
**/

#ifndef EADLIB_THROBBER_H
#define EADLIB_THROBBER_H

#include <iostream>
#include <initializer_list>

namespace eadlib {
    namespace cli {
        class Throbber {
          public:
            Throbber();
            Throbber( std::initializer_list<char> &list );
            ~Throbber();
            friend std::ostream & operator <<( std::ostream &os, const Throbber &throbber );
            const Throbber & operator ++();
            const Throbber operator ++( int );
          private:
            std::vector<char> _elements;
            size_t _position;
        };

        //--------------------------------------------------------------------------------------------------------------------------------------------
        // Throbber class public method implementations
        //--------------------------------------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         */
        inline Throbber::Throbber() :
            _position( 0 )
        {
            _elements = { '|', '/', '-', '\\' };
        }

        /**
         * Constructor
         * @param list Initializer_list of char for animation
         */
        inline Throbber::Throbber( std::initializer_list<char> &list ) :
            _position( 0 )
        {
            if( list.size() < 2 ) { //defaulting
                Throbber();
            } else {
                for( auto it = list.begin(); it != list.end(); ++it ) {
                    _elements.emplace_back( *it );
                }
            }
        }

        /**
         * Destructor
         */
        inline Throbber::~Throbber() {
        }

        /**
         * Output stream operator
         * @param os       Output stream
         * @param throbber Throbber object
         * @return Output stream
         */
        inline std::ostream & operator <<( std::ostream &os, const Throbber &throbber ) {
            os << "\b" << throbber._elements.at( throbber._position ) << std::flush;
            return os;
        }

        /**
         * Increments the animation of the throbber
         * @return Throbber object post-increment
         */
        inline const Throbber& Throbber::operator ++() {
            if( !_elements.empty() ) {
                _position == ( _elements.size() - 1 ) ? _position = 0
                                                      : _position++;
            }
            return *this;
        }

        /**
        * Increments the animation of the throbber
        * @return Throbber object pre-increment
        */
        inline const Throbber Throbber::operator ++( int ) {
            Throbber original = *this;
            ++( *this );
            return original;
        }
    }
}

#endif //EADLIB_THROBBER_H
