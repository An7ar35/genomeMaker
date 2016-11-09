#ifndef GENOMEMAKER_SEQUENCERSIM_TESTS_H
#define GENOMEMAKER_SEQUENCERSIM_TESTS_H

#include "MOCK_Writer.h"
#include "MOCK_Reader.h"
#include "gtest/gtest.h"
#include "../src/tools/SequencerSim.h"

TEST( SequencerSim_Tests, Constructor ) {
    auto reader = MOCK_Reader();
    auto writer = MOCK_Writer();
    auto sim = genomeMaker::SequencerSim( reader, writer, 10, 100, 0.5 );
    ASSERT_EQ( 10, sim.getReadLength() );
    ASSERT_EQ( 100, sim.getTotalReads() );
    ASSERT_EQ( 0.5, sim.getErrorRate() );
}

TEST( SequencerSim_Tests, getReadLength ) {
    auto reader = MOCK_Reader();
    auto writer = MOCK_Writer();
    auto sim = genomeMaker::SequencerSim( reader, writer, 10, 100, 0.5 );
    ASSERT_EQ( 10, sim.getReadLength() );
    sim = genomeMaker::SequencerSim( reader, writer, 0, 100, 0.5 );
    ASSERT_EQ( 260, sim.getReadLength() ); //defaulting
    sim = genomeMaker::SequencerSim( reader, writer, 1, 100, 0.5 );
    ASSERT_EQ( 1, sim.getReadLength() ); //lower bound
    sim = genomeMaker::SequencerSim( reader, writer, 1000, 100, 0.5 );
    ASSERT_EQ( 1000, sim.getReadLength() ); //upper bound
    sim = genomeMaker::SequencerSim( reader, writer, 1001, 100, 0.5 );
    ASSERT_EQ( 260, sim.getReadLength() ); //defaulting
}

TEST( SequencerSim_Tests, getTotalReads ) {
    auto reader = MOCK_Reader();
    auto writer = MOCK_Writer();
    auto sim = genomeMaker::SequencerSim( reader, writer, 10, 100, 0.5 );
    ASSERT_EQ( 100, sim.getTotalReads() );
    sim = genomeMaker::SequencerSim( reader, writer, 10, 1, 0.5 );
    ASSERT_EQ( 1, sim.getTotalReads() ); //lower bound
    sim = genomeMaker::SequencerSim( reader, writer, 10, 0, 0.5 );
    ASSERT_EQ( 1, sim.getTotalReads() ); //defaulting
}

TEST( SequencerSim_Tests, getErrorRate ) {
    auto reader = MOCK_Reader();
    auto writer = MOCK_Writer();
    auto sim = genomeMaker::SequencerSim( reader, writer, 10, 100, 0.5 );
    ASSERT_EQ( 0.5, sim.getErrorRate() );
    sim = genomeMaker::SequencerSim( reader, writer, 10, 100, -0.1 );
    ASSERT_EQ( 0, sim.getErrorRate() ); //defaulting
    sim = genomeMaker::SequencerSim( reader, writer, 10, 100, 0 );
    ASSERT_EQ( 0, sim.getErrorRate() ); //lower bound
    sim = genomeMaker::SequencerSim( reader, writer, 10, 100, 1 );
    ASSERT_EQ( 1, sim.getErrorRate() ); //upper bound
    sim = genomeMaker::SequencerSim( reader, writer, 10, 100, 1.1 );
    ASSERT_EQ( 0, sim.getErrorRate() ); //defaulting
}

#endif //GENOMEMAKER_SEQUENCERSIM_TESTS_H
