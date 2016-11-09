#ifndef EADLIB_MATH_H
#define EADLIB_MATH_H

#include <cmath>

namespace eadlib {
    namespace math {
        /**
         * Gets the precision of a double
         * @param value Value to get precision from
         * @return Precision
         */
        inline size_t getPrecision( const double &value ) {
            size_t i { 0 };
            double temp { value };
            while( temp >= 1 ) {
                temp *= 0.1;
                i++;
            }
            temp = value;
            size_t j { 0 };
            while( temp > trunc( temp ) && temp < ceil( temp ) ) {
                temp *= 10;
                j++;
            }
            return i + j;
        }
    }
}

#endif //EADLIB_MATH_H
