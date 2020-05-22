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

#ifndef MULTI_BAM_READER_H_
#define MULTI_BAM_READER_H_

#include <string>
#include <vector>
#include <exception>

#include <pthread.h>

#include "api/BamAux.h"
#include "api/BamReader.h"
#include "api/BamAlignment.h"

#define MIN_ISIZE 100
#define MAX_ISIZE 1000000

using namespace BamTools;

class MultiBamReaderException : public std::exception
{
    std::string _what;

public:
    MultiBamReaderException( const std::string &what ) : _what(what) {}
    MultiBamReaderException( const char* what ) : _what( std::string(what) ) {}

    ~MultiBamReaderException() throw() {}

    virtual const char* what() const throw(){ return _what.c_str(); }
};


//! class that can handle multiple bam files of different libraries aligned on the same assembly
class MultiBamReader
{
private:
	bool _is_open;								// whether every bam file has been opened successfully

    std::vector< BamReader* > _bam_readers; 	// pointers to BAM readers
    std::vector< BamAlignment > _bam_aligns; 	// Next alignment to be processed for each reader
    std::vector< bool > _valid_aligns;			// Whether an alignment is valid (to be processed)

    std::vector< pthread_mutex_t > _bam_mutex;	// mutexes associated to each BAM reader

    std::vector< int32_t > _minInsert;			// min insert size to compute mean/std
    std::vector< int32_t > _maxInsert;			// max insert size to compute mean/std

    std::vector< double > _isize_mean;			// mean insert size for the libraries
    std::vector< double > _isize_std;			// standard deviation of insert sizes for the libraries
    std::vector< uint64_t > _isize_count;

    uint64_t _asm_size;							// assembly size
    std::vector< uint64_t > _reads_len;			// sum of libraries' reads length
    std::vector< double > _coverage;			// libraries' mean coverage

public:
    MultiBamReader();
    ~MultiBamReader();

    bool Open( const std::vector< std::string > &filenames );
    bool Open( const std::string &filename );
    void Close();

    inline uint32_t size() const { return (this->_bam_readers).size(); }
	inline BamReader& at( const size_t &index ) const { return *(this->_bam_readers.at(index)); }
    inline BamReader& operator[]( const size_t &index ) const { return *(this->_bam_readers[index]); }

	void setMinMaxInsertSizes( const std::vector<int32_t> &minInsert, const std::vector<int32_t> &maxInsert );

    BamReader* getBamReader( uint32_t idx );
    double getISizeMean( uint32_t idx );
    double getISizeStd( uint32_t idx );
    uint64_t getISizeNum( uint32_t idx );

    double getCoverage( uint32_t idx );
    double getMeanCoverage();
    double getGlobCoverage();

    void lockBamReader( uint32_t idx );
    void unlockBamReader( uint32_t idx );

    bool Rewind();
    bool Jump( uint32_t refID, uint32_t position = 0 );
    bool SetRegion ( const uint32_t &leftRefID, const uint32_t &leftPosition, const uint32_t &rightRefID, const uint32_t &rightPosition );

    bool computeStatistics();

    bool GetNextAlignment( BamAlignment &align, bool update_stats = false );
    const RefVector& GetReferenceData() const;

    void writeStatsToFile( const std::string &filename ) const;
    uint32_t readStatsFromFile( const std::string &filename );
};

#endif /* MULTI_BAM_READER_H_ */
