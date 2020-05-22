/*
 *  This file is part of GAM-NGS.
 *  Copyright (c) 2011 by Riccardo Vicedomini <rvicedomini@appliedgenomics.org>,
 *  Francesco Vezzi <vezzi@appliedgenomics.org>,
 *  Simone Scalabrin <scalabrin@appliedgenomics.org>,
 *  Lars Arverstad <lars.arvestad@scilifelab.se>,
 *  Alberto Policriti <policriti@appliedgenomics.org>,
 *  Alberto Casagrande <casagrande@appliedgenomics.org>
 *
 *  GAM-NGS is an evolution of a previous work (GAM) done by Alberto Casagrande,
 *  Cristian Del Fabbro, Simone Scalabrin, and Alberto Policriti.
 *  In particular, GAM-NGS has been adapted to work on NGS data sets and it has
 *  been written using GAM's software as starting point. Thus, it shares part of
 *  GAM's source code.
 *
 *  GAM-NGS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GAM-NGS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GAM-NGS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*!
 * \file Block.hpp
 * \brief Definition of Block class.
 * \details This file contains the definition of the class representing a block
 *          between two assemblies.
 */

#ifndef BLOCK_HPP
#define	BLOCK_HPP

#include <list>
#include <map>
#include <set>

#include "api/BamAux.h"
#include "api/BamReader.h"

#include "bam/MultiBamReader.hpp"
#include "assembly/Read.hpp"
#include "assembly/Frame.hpp"
#include "assembly/RefSequence.hpp"

#include "google/sparse_hash_map"
#include "types.hpp"

using namespace BamTools;
using google::sparse_hash_map;

//! Class implementing a block.
class Block
{
    //! Inserts a block into into an output stream.
    friend std::ostream &operator<<( std::ostream &output, const Block &block );

    //! Extracts a block from an input stream.
    friend std::istream &operator>>( std::istream &input, Block &block );

private:
    IntType     _numReads;      //!< Number of reads in the block
    Frame       _masterFrame;   //!< Frame of master assembly
    Frame       _slaveFrame;    //!< Frame of slave assembly

public:
    //! A constructor.
    /*!
     * This method creates an empty block.
     */
    Block();

	//! A constructor.
	/*!
	 * This method creates a singleton (or empty) block
	 */
	Block( Read &mRead, Read &sRead, int minOverlap );

    //! A copy constructor.
    /*!
     * Creates a copy of a block.
     * \param block a Block object.
     */
    Block(const Block &block);

    //! Sets the number of the reads in the block.
    /*!
     * \param nr number of reads.
     */
    void setReadsNumber( IntType nr );

    //! Sets master's frame.
    /*!
     * \param master frame relative to master's assembly.
     */
    void setMasterFrame( Frame master );

    //! Sets slave's frame.
    /*!
     * \param slave frame relative to slave's assembly.
     */
    void setSlaveFrame( Frame slave );

    //! Returns the number of the reads in the block.
    /*!
     * \return the number of reads in the block.
     */
    IntType getReadsNumber() const;

    //! Return the sum of the lengths of all the reads included in the block.
    /*!
     * \return the sum of read lengths of the block.
     */
    UIntType getReadsLen() const;

    //! Returns the frame relative to the master assembly.
    /*!
     * \return const reference to the master's frame.
     */
    const Frame& getMasterFrame() const;

	//! Returns the frame relative to the master assembly.
	/*!
	 * \return const reference to the master's frame.
	 */
	Frame& getMasterFrame();

	int32_t getMasterId() const;

    //! Returns the frame relative to the slave assembly.
    /*!
     * \return const reference to the slave's Frame.
     */
    const Frame& getSlaveFrame() const;

	//! Returns the frame relative to the slave assembly.
	/*!
	 * \return reference to the slave's Frame.
	 */
	Frame& getSlaveFrame();

	int32_t getSlaveId() const;

    //! Returns whether the block is empty or not.
    /*!
     * \return \c true if block is empty, \c false otherwise.
     */
    bool isEmpty();

    //! Clears the block.
    void clear();

    bool overlaps( Read &mRead, Read &sRead, int minOverlap = 0 );

    //! Adds master and slave reads to the block.
    /*!
     * Both reads must share the same contigs of the block.
     * \param mRead a read of the master assembly.
     * \param sRead a read of the slave assembly.
     * \param minOverlap minimum number of bases shared with the block.
     * \return \c true if reads have been added successfully,\c false otherwise.
     */
    bool addReads( Read &mRead, Read &sRead, int minOverlap = 0 );

    //! Removes adjacent (either on master or on slave) blocks whose frames overlaps.
    /*!
     *  Overlapping frames witnesses probable inconsistences due to repeats.
     * \param blocks    vector of blocks.
     * \return vector of blocks filtered.
     */
    static std::vector<Block> filterBlocksByOverlaps( std::list<Block> &blocks );

    static void filterBlocksByCoverage(
		std::list<Block> &blocks,
		const std::set< std::pair<int32_t,int32_t> > &slb,
		double min_cov,
		double t = 0.5 );

	static void filterBlocksByLength(
		std::list<Block> &blocks,
		const RefSequence &masterRef,
		const RefSequence &slaveRef,
		const std::set< std::pair<int32_t,int32_t> > &slb,
		int32_t min_length );

    //! Finds the blocks over two assemblies.
    /*!
     * \param outblocks         (output) vector of blocks found.
     * \param bamReader         BamReader object of the slave assembly.
     * \param minBlockSize      minimum reads required to form a block.
     * \param readsMap_1        hash table of the master's reads (pair 1)
     * \param readsMap_2        hash table of the master's reads (pair 2)
     * \param coverage          vector of coverages of the slave assembly (output)
     * \param assemblyId        slave assembly identifier
     * \return vector of blocks found.
     */
    static void findBlocks(
        std::vector<Block> &outblocks,
        MultiBamReader &bamReader,
        const int minBlockSize,
        sparse_hash_map< std::string, Read > &readsMap_1,
        sparse_hash_map< std::string, Read > &readsMap_2,
        std::vector< std::vector< uint32_t > > &coverage,
        bool noMultFilter = false );

    static void updateCoverages(
        std::vector<Block> &blocks,
        const std::vector< std::vector<uint32_t> > &masterCoverage,
        std::vector< std::vector<uint32_t> > &slaveCoverage );

    //! Returns whether two block share the master contig.
    /*!
     * \param a a block
     * \param b a block
     * \return \c true if \c a and \c b have the same master contig, \c false otherwise.
     */
    static bool shareMasterContig( const Block &a, const Block &b );

    //! Returns whether two block share the slave contig.
    /*!
     * \param a a block
     * \param b a block
     * \return \c true if \c a and \c b have the same slave contig, \c false otherwise.
     */
    static bool shareSlaveContig( const Block &a, const Block &b );

    //! Returns whether two block share both the contigs.
    /*!
     * \param a a block
     * \param b a block
     * \return \c true if \c a and \c b have the same master and slave contigs, \c false otherwise.
     */
    static bool shareContig( const Block &a, const Block &b );

    //! Reads blocks from an input file.
    /*!
     * \param blockFile the name of the input file.
     * \param minBlockSize minimum size (# reads) of the blocks to be loaded.
     * \return a vector containing the blocks read from the file.
     */
	static void loadBlocks( const std::string& blockFile, std::list<Block> &blocks, int minBlockSize = 1 );
    static std::vector<Block> readCoreBlocks( const std::string& blockFile, int minBlockSize = 1 );

    //! Write a vector of blocks into a file.
    /*!
     * \param blockFile the name of the output file.
     * \param blocks a vector of Block objects.
     */
    static void writeBlocks( const std::string& blockFile, std::vector< Block >& blocks  );
    static void writeBlocksVerbose( const std::string &blockFile, std::vector< Block > &blocks, const MultiBamReader &masterBam, const MultiBamReader &slaveBam );

    //! Less-Than operator for the Block class.
    bool operator <(const Block &block) const;

    //! Equality operator for the Block class.
    bool operator ==(const Block &block) const;
};


void getNoBlocksContigs(
	const RefSequence &masterRef,
	const RefSequence &slaveRef,
	const std::list<Block> &blocks,
	boost_bitset_t &masterNBC,
	boost_bitset_t &slaveNBC );

void getNoBlocksAfterFilterContigs(
	const RefSequence &masterRef,
	const RefSequence &slaveRef,
	const std::list<Block> &blocks,
	const boost_bitset_t &masterNBC,
	const boost_bitset_t &slaveNBC,
	boost_bitset_t &masterNBC_AF,
	boost_bitset_t &slaveNBC_AF );

#endif	/* BLOCK_HPP */

