#ifndef EADLIB_OPTION_H
#define EADLIB_OPTION_H

#include <iostream>
#include <regex>
#include <vector>

namespace eadlib {
    namespace cli {
        class Option {
          public:
            Option( const std::string &name, const std::string &alt,
                    const std::string &description, const bool &required );
            ~Option();
            friend std::ostream &operator <<( std::ostream &os, const Option &option );
            bool setValue( const size_t &index, const std::string &value );
            void addValueRegex( const std::regex &regex, const std::string &fail_msg );
            void addValueRegex( const std::regex &regex, const std::string &fail_msg, const std::string &default_value );
            size_t valuesSet() const;
            size_t valuesExpected() const;
            std::vector<bool> getValueFlags() const;
            std::vector<std::string> getValues() const;
          private:
            struct Value {
                Value( const std::regex &regex, const std::string &fail_msg ) :
                    _flag( false ),
                    _regex( regex ),
                    _error_msg( fail_msg ),
                    _value( "" ),
                    _has_default( false ),
                    _default( "" )
                {};
                Value( const std::regex &regex, const std::string &fail_msg, const std::string &default_value ) :
                    _flag( false ),
                    _regex( regex ),
                    _error_msg( fail_msg ),
                    _value( "" ),
                    _has_default( true ),
                    _default( default_value )
                {};
                bool _flag;
                std::regex _regex;
                std::string _value;
                bool _has_default;
                std::string _default;
                std::string _error_msg;
            };
            std::string _name;
            std::string _alt;
            std::string _description;
            bool _req_flag;
            std::vector<Value> _values;
            bool _has_defaults;
            size_t _values_set;
        };

        /**
         * Constructor
         * @param name        Name of option (short version)
         * @param alt         Name of option (long version)
         * @param description Description of option
         * @param required    Required option flag
         */
        inline Option::Option( const std::string &name, const std::string &alt,
                               const std::string &description, const bool &required ) :
            _name( name ),
            _alt( alt ),
            _description( description ),
            _req_flag( required ),
            _has_defaults( false ),
            _values_set( 0 )
        {}

        /**
         * Destructor
         */
        inline Option::~Option() {}

        /**
         * Output stream
         * @param os     Output stream
         * @param option Option
         * @return Output stream
         */
        inline std::ostream & operator <<( std::ostream &os, const Option &option ) {
            os << option._name
               << "\t" << option._alt
               << "\t" << option._description;
            if( option._req_flag ) {
                os << "\t\t\t[REQUIRED]";
            }
            if( option._has_defaults ) {
                os << "\t[DEFAULT='";
                for( auto it = option._values.begin(); it != option._values.end(); ++it ) {
                    if( it->_has_default ) {
                        os << it->_default << "'";
                    }
                    if( std::next( it ) != option._values.end() ) {
                        os << ", ";
                    }
                }
                os << "]";
            }
            return os;
        }

        /**
         * Sets a new value
         * @param index Index of value to set
         * @param value Value to set
         * @return Result of regex validation
         */
        inline bool Option::setValue( const size_t &index, const std::string &value ) {
            if( _values.empty() ) {
                std::cerr << "Trying to set a value '" << value << "' on cli Parser option '"
                          << _name << "' that was not expected." << std::endl;
                return false;
            }
            if( index >= _values.size() ) {
                std::cerr << "Out of bound index: trying to set a value '" << value << "' in index '" << index
                          << "/" << _values.size() << "' on cli Parser option '" << _name << "'." << std::endl;
                return false;
            }
            _values.at( index )._value = value;
            if( !( _values.at( index )._has_default ) ) {
                _values_set++;
            }
            if( !std::regex_match( _values.at( index )._value, _values.at( index )._regex ) ) {
                std::cerr << "Invalid value '" << value << "': " << _values.at( index )._error_msg << std::endl;
                return false;
            }
            _values.at( index )._flag = true;
            return true;
        }

        /**
         * Adds a new value regex in order
         * @param regex    Value regex
         * @param fail_msg Message for regex fail
         */
        inline void Option::addValueRegex( const std::regex &regex, const std::string &fail_msg ) {
            _values.emplace_back( Value( regex, fail_msg ) );
        }

        /**
         * Adds a new value regex in order
         * @param regex         Value regex
         * @param fail_msg      Message for regex fail
         * @param default_value Default value
         * @throws std::invalid_argument when default value given fails the regex
         */
        inline void Option::addValueRegex( const std::regex &regex, const std::string &fail_msg,
                                           const std::string &default_value ) {

            if( !std::regex_match( default_value, regex ) ) {
                std::cerr << "Invalid default value '" << default_value << "' passed for cli parser option '"
                          << _name << "/" << _alt << "' (regex fail)." << std::endl;
                throw std::invalid_argument( "Default value does not pass regex check given for option." );
            }
            _values_set++;
            _has_defaults = true;
            _values.emplace_back( Value( regex, fail_msg, default_value ) );
        }

        /**
         * Gets the number of values stored in the option
         * @return Number of values
         */
        inline size_t Option::valuesSet() const {
            return _values_set;
        }

        /**
         * Gets the number of values expected for this option
         * @return Number of expected values
         */
        inline size_t Option::valuesExpected() const {
            return _values.size();
        }

        /**
         * Gets all the value flags from Option in order
         * Note: checks for both value flag and default flag
         * @return Value flags
         */
        inline std::vector<bool> Option::getValueFlags() const {
            std::vector<bool> v;
            for( auto it = _values.begin(); it != _values.end(); ++it ) {
                v.emplace_back( it->_flag || it->_has_default );
            }
            return v;
        }

        /**
         * Gets all values from Option in order
         * Note: If the value flag is not set in a Value then the default is used
         * @return Values in order
         */
        inline std::vector<std::string> Option::getValues() const {
            std::vector<std::string> v;
            for( auto it = _values.begin(); it != _values.end(); ++it ) {
                if( it->_flag ) {
                    v.emplace_back( it->_value );
                } else {
                    v.emplace_back( it->_default );
                }
            }
            return v;
        }
    }
}

#endif //EADLIB_OPTION_H
