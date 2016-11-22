//
// Created by alwyn on 4/9/16.
//

#include "Randomiser.h"

/**
 * Constructor
 * Note: default pool range is 0-1 (coin flip)
 */
genomeMaker::Randomiser::Randomiser() {
    setPoolRange( 0, 1 );
}

/**
 * Constructor
 * @param range_from Lower bound of the pool range
 * @param range_to   Upper bound of the pool range
 */
genomeMaker::Randomiser::Randomiser( const uint64_t &range_from, const uint64_t &range_to ) {
    if( !setPoolRange( range_from, range_to ) ) {
        LOG_ERROR( "[genomeMaker::Randomiser( ", range_from, ", ", range_to, " )] Problem setting pool range. Defaulting to 0-1 (coin flip)." );
        setPoolRange( 0, 1 );
    }
}

/**
 * Copy-Constructor
 * @param randomiser Randomiser to copy
 */
genomeMaker::Randomiser::Randomiser( const Randomiser &randomiser ) :
    _lower_bound( randomiser._lower_bound ),
    _upper_bound( randomiser._upper_bound ),
    _rng( randomiser._rng ),
    _distribution( randomiser._distribution )
{}

/**
 * Move-Constructor
 * @param randomiser Randomiser to move over
 */
genomeMaker::Randomiser::Randomiser( Randomiser &&randomiser ) :
    _lower_bound( randomiser._lower_bound ),
    _upper_bound( randomiser._upper_bound ),
    _rng( std::move( randomiser._rng ) ),
    _distribution( std::move( randomiser._distribution ) )
{}

/**
 * Copy-assignement operator
 * @param rhs Randomiser to copy over
 * @return Randomiser
 */
genomeMaker::Randomiser & genomeMaker::Randomiser::operator =( const Randomiser &rhs ) {
    _lower_bound = rhs._lower_bound;
    _upper_bound = rhs._upper_bound;
    _rng = rhs._rng;
    _distribution = rhs._distribution;
    return *this;
}

/**
 * Sets the randomiser to a new pool size
 * @param range_from Lower bound of the pool range
 * @param range_to   Upper bound of the pool range
 * @return Success
 */
bool genomeMaker::Randomiser::setPoolRange( const uint64_t &range_from, const uint64_t &range_to ) {
    if( ( range_to - range_from ) < 1 ) {
        LOG_ERROR( "[genomeMaker::Randomiser::setPoolSize( ", range_from, ", ", range_to, " )] Pool size specified is too small (", range_to - range_from, ")." );
        return false;
    }
    _lower_bound = range_from;
    _upper_bound = range_to;
    _rng.seed( uint64_t() );
    _distribution = std::uniform_int_distribution<uint64_t>( range_from, range_to );
    return true;
}

/**
 * Gets the set lower bound of the Randomiser
 * @return Lower bound
 */
uint64_t genomeMaker::Randomiser::getLowerBound() {
    return _lower_bound;
}

/**
 * Gets the set upper bound of the Randomiser
 * @return Upper bound
 */
uint64_t genomeMaker::Randomiser::getUpperBound() {
    return  _upper_bound;
}

/**
 * Gets a random number from the pool
 * @return Random number from pool
 */
unsigned long genomeMaker::Randomiser::getRand() {
    return _distribution( _rng );
}