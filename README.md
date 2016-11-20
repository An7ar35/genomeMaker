# Table of Contents
1. [Description](#description)
2. [How to use](#how-to-use)
   1. [Creating a genome file](#creating-a-genome-file)
   2. [Creating a set of FASTA reads](#creating-a-set-of-FASTA-reads)
   3. [Creating a genome and its reads in one go](#creating-a-genome-and-its-reads-in-one-go)
3. [Logger](#logger)
4. [Platforms Supported](#platforms-supported)

## Description ##

Software to generate randomised content into files from letter sets and then
generate FASTA read files from it. Originally created in order to test 
deBruijn graph construction from FASTA reads and have the ability to compare
the results to original genome data.

The program can be used in the following cases:

1. Create a synthetic genome.
2. Create a simulated sequencer reads file (FASTA)
    from an existing genome file.
3. Create both the synthetic genome file and its
    simulated sequencer reads file in one go.


#### Notes ####
___The software is in the alpha stage of development.___

## How to use ##
### CLI ###
Help with flag descriptions and examples:
~~~~
./genomeMaker
~~~~
Standard usage: 
~~~~
./genomeMaker -<option> <argument>
~~~~

#### Creating a genome file ####
##### Flags #####
~~~~
  -g	-genome	Name of the genome file to create.
  -s	-size	Size of the genome in bytes.
  -t	-type	Type of letter set for genome creation (DNA, RNA).	[DEFAULT='DNA']
~~~~

##### Example #####
To create a synthetic genome file of 100,000,000 bytes (100MB) with the __RNA__ letter set:
~~~~
./genomeMaker -g genome_file -s 100000000 -t rna
~~~~
    
#### Creating a set of FASTA reads ####
Note: Error rate ````-e```` is not yet implemented.
##### Flags #####
~~~~
  -f	-fasta	Name of the FASTA file to create.
  -l	-length	Character length of each reads.	[DEFAULT='260']
  -d	-depth	Depth of reads.
  -e	-error	Error rate of the simulated sequencer (0 <= x <= 1).	[DEFAULT='0']
~~~~

##### Example #####
To create a sequencer file named "_my_reads.fasta_" with the default read length
of 260, error rate of 0.01, depth of 200 and based on a genome file
called "_genome.genome_":
~~~~
./genomeMaker -g my_reads -f reads -d 200 -e 0.01
~~~~
    
#### Creating a genome and its reads in one go ####
#### Flag ####
~~~~
  -p	-pipeline	Create both genome and sequencer files.
~~~~

#### Example ####
To create a complete set of files composed of:
- a genome file called "_my_genome.genome_"
  - size of 100,000 bytes
- a sequencer file "_my_genome.fasta_"
  - read length of 10 characters and
  - depth of 5
~~~~
./genomeMaker -p my_genome -s 100000 -l 10 -d 5
~~~~

#### Logger ####
GenomeMaker comes with a logger but defaults to output to both the screen and the file.

To avoid that just make sure that the output specified in the _log_config.cfg' file only contains:
~~~~
OUTPUT=<log,FILE_OVERWRITE,TERMINAL,MSG>
~~~~
If you run into issues with the software you will need to change that last argument from "__MSG__" to "__TRACE__" in order to produce a comprehensive log during your next execution of genomeMaker.

## Platforms Supported ##
The software is bundled with the components required from the EADlib library.

### Linux ツ ###
- Need a version of GCC with C++14 support (made with GCC 6.2.1)
- CMake 3.5
- [_Optional_] Google Tests libraries (gTest & gMock) to run the unit tests

### Mac OSX ###

- See above.
- Homebrew might help. Untested.
 
### Windows ###

- Nope. Good luck.