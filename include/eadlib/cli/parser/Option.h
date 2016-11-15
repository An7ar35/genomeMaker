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
            bool setValue( const std::string &value );
            void addValueRegex( const std::regex &regex, const std::string &fail_msg );
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
                    _value( "" )
                {};
                bool _flag;
                std::regex _regex;
                std::string _value;
                std::string _error_msg;
            };
            std::string _name;
            std::string _alt;
            std::string _description;
            bool _req_flag;
            std::vector<Value> _values;
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
            return os;
        }

        /**
         * Sets a new value in order
         * @param value Value to add
         * @return Result of regex validation
         */
        inline bool Option::setValue( const std::string &value ) {
            size_t index = _values_set;
            if( index >= _values.size() ) {
                std::cerr << "Trying to set a value '" << value << "' on cli Parser option '" << _name << "' that was not expected." << std::endl;
                return false;
            }
            _values.at( index )._value = value;
            _values_set++;
            if( !std::regex_match( _values.at( index )._value, _values.at( index )._regex ) ) {
                std::cerr << "Invalid value '" << value << "': " << _values.at( index )._error_msg << std::endl;
                return false;
            }
            _values.at( index )._flag = true;
            return true;
        }

        /**
         * Adds a new value regex in order
         * @param regex Value regex
         * @return Success of regex
         */
        inline void Option::addValueRegex( const std::regex &regex, const std::string &fail_msg ) {
            _values.emplace_back( Value( regex, fail_msg ) );
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
         * @return Value flags
         */
        inline std::vector<bool> Option::getValueFlags() const {
            std::vector<bool> v;
            for( auto it = _values.begin(); it != _values.end(); ++it ) {
                v.emplace_back( it->_flag );
            }
            return v;
        }

        /**
         * Gets all values from Option in order
         * @return Values in order
         */
        inline std::vector<std::string> Option::getValues() const {
            std::vector<std::string> v;
            for( auto it = _values.begin(); it != _values.end(); ++it ) {
                v.emplace_back( it->_value );
            }
            return v;
        }
    }
}

#endif //EADLIB_OPTION_H
