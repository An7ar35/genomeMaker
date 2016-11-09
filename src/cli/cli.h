#ifndef GENOMEMAKER_CLI_H
#define GENOMEMAKER_CLI_H

#include <iostream>
#include <fstream>

#include "eadlib/cli/parser/Parser.h"
#include "eadlib/tool/Convert.h"

#include "../containers/FileOptions.h"

namespace genomeMaker {
    namespace cli {
        void setupOptions( char **argv, const std::string &program_title, eadlib::cli::Parser &parser );
        void loadOptionsIntoContainer( const eadlib::cli::Parser &parser, FileOptions &options );
    }
}

#endif //GENOMEMAKER_CLI_H
