#ifndef UNIT_TESTS_READER_H
#ifndef UNIT_TESTS_READER_H
#define UNIT_TESTS_READER_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MOCK_Randomiser.h"

TEST( Creator_Tests, create_DNA ) {
    using ::testing::Return;
    using ::testing::_;
    using ::testing::ByRef;
    //ON_CALL( r, getRand() ).WillByDefault( Return( 1 ) );
    //ON_CALL( r, setPoolRange(_,_) ).WillByDefault( Return( true ) );
    //auto creator = genomeMaker::GenomeCreator( r, "test_file" );
    //creator.create_DNA();
}

#endif //UNIT_TESTS_READER_H