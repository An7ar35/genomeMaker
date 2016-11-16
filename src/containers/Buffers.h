#ifndef GENOMEMAKER_BUFFERS_H
#define GENOMEMAKER_BUFFERS_H

#include <vector>
#include <memory>
#include <fstream>

namespace genomeMaker {
    struct Buffers {
        Buffers() :
            _current_size( -1 ),
            _current( new std::vector<char>() ),
            _next_size( -1 ),
            _next( new std::vector<char>() )
        {}
        void swapBuffers() {
            std::swap( _current, _next );
            std::streamsize temp = _current_size;
            _current_size = _next_size;
            _next_size = temp;
        }
        std::unique_ptr<std::vector<char>> _current;
        std::unique_ptr<std::vector<char>> _next;
        std::streamsize _current_size;
        std::streamsize _next_size;
    };
}

#endif //GENOMEMAKER_BUFFERS_H
