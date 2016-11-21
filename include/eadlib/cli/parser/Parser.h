/**
 * \class eadlib::cli::Parser
 *
 * CLI parser system to process program arguments.
 * The Parser uses regular expression to check argument values passed to it.
 * Any value that fail its regular expression check during the parsing will trigger
 * its associated error message and the parsing method with return false.
 * If all values check out then true is returned and anything parsed can be queried.
 *
 * \note If the regex is malformed an std::regex_error exception will be raise on
 *       ````option(..)```` by the std::regex() constructor.
 *
 * \author       E. A. Davison
 * \copyright    E. A. Davison 2016
 * \license      GNUv2 Public License
**/

#ifndef EADLIB_CLI_PARSER_H
#define EADLIB_CLI_PARSER_H

#include <iostream>
#include <unordered_map>
#include <list>
#include "Option.h"

//TODO printInfo() -> organise size of columns based on largest item inside of each
//TODO printInfo() -> option to move the required and default flag/desc to next line?

namespace eadlib {
    namespace cli {
        class Parser {
          public:
            struct ValueCheck {
                std::regex regex;
                std::string fail_msg;
                std::string default_value = "";
            };
            Parser();
            ~Parser();
            void option( const std::string &cat,
                         const std::string &name,
                         const std::string &alt,
                         const std::string &desc,
                         const bool &required,
                         std::list<ValueCheck> value_regexs );
            void addTitleLine( const std::string &title );
            void addDescriptionLine( const std::string &usage );
            void addExampleLine( const std::string &example );
            bool parse( const int &argc, char *argv[] );
            void printInfo() const;
            std::vector<bool> getValueFlags( const std::string &option_name ) const;
            std::vector<std::string> getValues( const std::string &option_name ) const;
            size_t size() const;
            bool isEmpty() const;

          private:
            typedef std::unordered_map<std::string, size_t> IndexMap_t;
            typedef std::unordered_map<std::string, std::list<size_t>> MultiIndexMap_t;
            std::vector<Option>                    _options;
            IndexMap_t                             _name_map;
            IndexMap_t                             _alt_map;
            MultiIndexMap_t                        _category_map;
            std::vector<MultiIndexMap_t::iterator> _category_order;
            std::string                            _program_title;
            std::vector<std::string>               _usage_description;
            std::vector<std::string>               _usage_examples;
        };

        /**
         * Constructor
         */
        inline Parser::Parser() :
            _program_title( "" )
        {}

        /**
         * Destructor
         */
        inline Parser::~Parser() {}

        /**
         * Adds option to the parser
         * @param category     Option category
         * @param name         Option name (e.g. 'n')
         * @param alternative  Option alternative name (e.g.: 'name')
         * @param description  Option description
         * @param required     Required flag - adds an anotation in the description of the option
         * @param value_regexs List of { regular expression, error message, default value } for values expected in order
         */
        inline void Parser::option( const std::string &category,
                                    const std::string &name,
                                    const std::string &alternative,
                                    const std::string &description,
                                    const bool &required,
                                    std::list<ValueCheck> value_regexs ) {
            //creating/adding index entries for the option
            auto insert_result = _category_map.insert( typename MultiIndexMap_t::value_type( category,
                                                                                             std::list<size_t>() ) );
            if( insert_result.second ) { //if category was added
                _category_order.emplace_back( insert_result.first ); //add the MultiIndexMap_t iterator
            }
            _category_map.at( category ).emplace_back( _options.size() );
            _name_map.insert( typename IndexMap_t::value_type( name, _options.size() ) );
            _alt_map.insert( typename IndexMap_t::value_type( alternative, _options.size() ) );
            //creating the option and adding the expected values if any
            auto option = Option( name, alternative, description, required );
            for( auto regex : value_regexs ) {
                if( regex.default_value.empty() ) {
                    option.addValueRegex( regex.regex, regex.fail_msg );
                } else {
                    option.addValueRegex( regex.regex, regex.fail_msg, regex.default_value );
                }
            }
            _options.emplace_back( option );
        }

        /**
         * Adds a title line to the Parser text
         * @param title Title (program name and version typically)
         */
        inline void Parser::addTitleLine( const std::string &title ) {
            _program_title = title;
        }
        /**
         * Adds a description block to the Parser info
         * @param usage Text to be printed in the description block
         */
        inline void Parser::addDescriptionLine( const std::string &usage ) {
            _usage_description.emplace_back( usage );
        }

        /**
         * Adds an example block to the Parser info
         * @param example Text to be printed in the example block
         */
        inline void Parser::addExampleLine( const std::string &example ) {
            _usage_examples.emplace_back( example );
        }

        /**
         * Parses and checks cli arguments
         * @param argc
         * @param argv
         * @return Success
         */
        inline bool Parser::parse( const int &argc, char **argv ) {
            if( argc < 2 ) {
                printInfo();
                return false;
            } else {
                std::vector<std::string> arguments;
                for( int i = 1; i < argc; i++ ) {
                    arguments.emplace_back( argv[ i ] );
                }
                //Processing all args
                std::vector<std::string>::iterator it = arguments.begin();
                while( it != arguments.end() ) {
                    if( _name_map.find( *it ) != _name_map.end()) { //found as std name
                        size_t index = _name_map.at( *it );
                        std::string name = *it;
                        size_t count { 0 };
                        while( count < _options.at( index ).valuesExpected() ) {
                            it++;
                            if( !_options.at( index ).setValue( count, *it ) ) {
                                std::cerr << "Value '" << *it << "' for Option '"
                                          << name << "' is not valid." << std::endl;
                                return false;
                            }
                            count++;
                        }
                    } else if( _alt_map.find( *it ) != _alt_map.end()) { //found as alternative name
                        size_t index = _alt_map.at( *it );
                        std::string name = *it;
                        size_t count { 0 };
                        while( count < _options.at( index ).valuesExpected() ) {
                            it++;
                            if( !_options.at( index ).setValue( count, *it ) ) {
                                std::cerr << "Value '" << *it << "' for Option '"
                                          << name << "' is not valid." << std::endl;
                                return false;
                            }
                            count++;
                        }
                    } else { //Not a valid option
                        std::cerr << "Argument '" << *it << "' is not recognised." << std::endl;
                        return false;
                    }
                    it++;
                }
            }
            return true;
        }

        /**
         * Prints Parser info with all the options added
         */
        inline void Parser::printInfo() const {
            if( !_program_title.empty() ) {
                std::cout << _program_title << std::endl;
            }
            if( !_usage_description.empty() ) {
                std::cout << "-|Description|-" << std::endl;
                for( auto desc : _usage_description ) {
                    std::cout << desc << std::endl;
                }
                std::cout << std::endl;
            }
            for( auto cat : _category_order ) {
                std::cout << "-|" << cat->first << "|-" << std::endl;
                for( auto i : cat->second ) {
                    std::cout << "  " << _options.at( i ) << std::endl;
                }
                std::cout << std::endl;
            }
            if( !_usage_examples.empty() ) {
                std::cout << "-|Example(s)|-" << std::endl;
                for( auto eg : _usage_examples ) {
                    std::cout << eg << std::endl;
                }
                std::cout << std::endl;
            }
        }

        /**
         * Gets the list of value flags for an option (i.e. whether a value is set or not OR a default exists)
         * @param option_name Option name
         * @return List of flags
         * @throws std::out_of_range when option name given is not found or there are no values for it
         */
        inline std::vector<bool> Parser::getValueFlags( const std::string &option_name ) const {
            if( _name_map.find( option_name ) != _name_map.end() ) {
                size_t index = _name_map.at( option_name );
                if( _options.at( index ).valuesExpected() > 0 ) {
                    return _options.at( index ).getValueFlags();
                } else {
                    std::cerr << "Option '" << option_name << "' has no expected values." << std::endl;
                    throw std::out_of_range( "[eadlib::cli::Parser::getValueFlags( std::string )] "
                                                 "Option has no expected values." );
                }
            } else if( _alt_map.find( option_name ) != _alt_map.end() ) {
                size_t index = _alt_map.at( option_name );
                if( _options.at( index ).valuesExpected() > 0 ) {
                    return _options.at( index ).getValueFlags();
                } else {
                    std::cerr << "Option '" << option_name << "' has no expected values." << std::endl;
                    throw std::out_of_range( "[eadlib::cli::Parser::getValueFlags( std::string )] "
                                                 "Option has no expected values." );
                }
            } else {
                std::cerr << "Option '" << option_name << "' does not exist." << std::endl;
                throw std::out_of_range( "[eadlib::cli::Parser::getValueFlags( std::string )] "
                                             "Name of option given does not exist." );
            }
        }

        /**
         * Gets the list of values for an option
         * @param option_name Option name
         * @return List of values
         * @throws std::out_of_range when option name given is not found or there are no values for it
         */
        inline std::vector<std::string> Parser::getValues( const std::string &option_name ) const {
            if( _name_map.find( option_name ) != _name_map.end() ) {
                size_t index = _name_map.at( option_name );
                if( _options.at( index ).valuesSet() > 0 ) {
                    return _options.at( index ).getValues();
                } else {
                    std::cerr << "Values not found for Option '" << option_name << "'." << std::endl;
                    throw std::out_of_range( "[eadlib::cli::Parser::getValues( std::string )] "
                                                 "Option has expected values but nothing was found." );
                }
            } else if( _alt_map.find( option_name ) != _alt_map.end() ) {
                size_t index = _alt_map.at( option_name );
                if( _options.at( index ).valuesSet() > 0 ) {
                    return _options.at( index ).getValues();
                } else {
                    std::cerr << "Values not found for Option '" << option_name << "'." << std::endl;
                    throw std::out_of_range( "[eadlib::cli::Parser::getValues( std::string )] "
                                                 "Option has expected values but nothing was found." );
                }
            } else {
                std::cerr << "Option '" << option_name << "' does not exist." << std::endl;
                throw std::out_of_range( "[eadlib::cli::Parser::getValues( std::string )] "
                                             "Name of option given does not exist." );
            }
        }

        /**
         * Gets the number of options in the parser
         * @return Number of options
         */
        inline size_t Parser::size() const {
            return _options.size();
        }

        /**
         * Gets the empty state of the parser
         * @return No options state
         */
        inline bool Parser::isEmpty() const {
            return _options.empty();
        }
    }
}

#endif //EADLIB_CLI_PARSER_H