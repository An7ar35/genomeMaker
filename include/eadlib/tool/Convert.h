/**
   Component Name:  EADlib.tool.Convert
   Language:        C++14

   License: GNUv2 Public License
   (c) Copyright E. A. Davison 2016

   Author: E. A. Davison

   Description: Type conversion class (int/double/string/...)
   Dependencies: EADlib.math (for eadlib::math::getPrecision(..) )
**/

#ifndef EADLIB_CONVERT_H
#define EADLIB_CONVERT_H

#include <iostream>
#include <sstream>
#include <typeinfo>

#include "../logger/Logger.h"
#include "../math/math.h"

namespace eadlib {
    namespace tool {
        struct Convert {
          public:
            /**
             * Converts T to a string
             * @param value T value
             * @return The value as a string
             */
            template<typename T> static std::string to_string( T const &value ) {
                return To_String<T>::convert( value );
            }

            /**
             * Converts string to T
             * @param value String value
             * @return The value as a T type
             * @throws std::bad_cast when calue can't be translated to type T
             */
            template<typename T> static T string_to_type( std::string const &value ) {
                return StringToType<T>::convert( value );
            }

          private:
            template<typename T> struct To_String {
                /**
                 * Converts T type value to a string
                 * @param value T value
                 * @return Value as a string
                 */
                static std::string convert( T const &value ) {
                    std::stringstream ss;
                    ss << value;
                    return ss.str();
                }
            };

            template<typename T> struct StringToType {
                /**
                 * Converts string to T
                 * @param value String value
                 * @return Value as type T
                 * @throws std::bad_cast when value can't be tranlasted to type T
                 */
                static T convert( std::string const &value ) {
                    std::stringstream ss { value };
                    T convertedValue { };
                    if( !( ss >> convertedValue )) {
                        LOG_ERROR( "[eadlib::tool::Convert::string_to_type(", value, ")] Not a valid ",
                                   static_cast<std::string>( typeid( T ).name()), " received." );
                        throw std::bad_cast();
                    }
                    if( value.size() != ss.str().size() ) {
                        LOG_ERROR( "[eadlib::tool::Convert::string_to_type(", value, ")] Not a valid ",
                                   static_cast<std::string>( typeid( T ).name()), " received." );
                        throw std::bad_cast();
                    }
                    return convertedValue;
                }
            };
        };

        /**
         * To_String Specialisations
         */
        template<> struct Convert::To_String<double> {
            static std::string convert( double const &value ) {
                std::stringstream ss;
                ss.precision( eadlib::math::getPrecision( value ) );
                ss << value;
                return ss.str();
            }
        };

        template<> struct Convert::To_String<long double> {
            static std::string convert( long double const &value ) {
                std::stringstream ss;
                ss.precision( eadlib::math::getPrecision( value ) );
                ss << value;
                return ss.str();
            }
        };

        template<> struct Convert::To_String<float> {
            static std::string convert( float const &value ) {
                std::stringstream ss;
                ss.precision( eadlib::math::getPrecision( value ) );
                ss << value;
                return ss.str();
            }
        };

        /**
         * StringToType Specialisations
         */
        template<> struct Convert::StringToType<std::string> {
            static std::string convert( std::string const &value ) {
                return value;
            }
        };
    }
}



#endif //EADLIB_CONVERT_H
