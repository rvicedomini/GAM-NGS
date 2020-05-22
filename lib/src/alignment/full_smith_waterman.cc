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

#include <list>
#include <stdexcept>
#include <iostream>

#include "alignment/full_smith_waterman.hpp"

FullSmithWaterman::FullSmithWaterman() :
        _match_score(MATCH_SCORE),
        _mismatch_score(MISMATCH_SCORE),
        _gap_score(GAP_SCORE)
{}

FullSmithWaterman::FullSmithWaterman(
        const ScoreType& match_score,
        const ScoreType& mismatch_score,
        const ScoreType& gap_score) :
        _match_score(match_score), _mismatch_score(mismatch_score),
        _gap_score(gap_score)
{}

MyAlignment
FullSmithWaterman::find_alignment(
        const Contig& a,
        size_type begin_a,
        size_type end_a,
        const Contig& b,
        size_type begin_b,
        size_type end_b ) const
{
    size_type x_size = end_b - begin_b + 2;
    size_type y_size = end_a - begin_a + 2;

    ScoreType* sw_matrix[x_size];
    for( size_type i = 0; i < x_size; i++ ) sw_matrix[i] = new ScoreType[y_size];

    for( size_type i = 0; i < x_size; i++ ) sw_matrix[i][0] = 0;
    for( size_type j = 0; j < y_size; j++ ) sw_matrix[0][j] = 0;

    // fill SmithWaterman matrix
    for( size_type i = 1; i < x_size; i++ )
    {
        for( size_type j = 1; j < y_size; j++ )
        {
            ScoreType diag_score = sw_matrix[i-1][j-1] + (a.at(begin_a + j-1) == b.at(begin_b + i-1) ? _match_score : _mismatch_score );
            ScoreType up_score = sw_matrix[i-1][j] + _gap_score;
            ScoreType left_score = sw_matrix[i][j-1] + _gap_score;

            sw_matrix[i][j] = std::max( std::max(diag_score,up_score), left_score );
        }
    }

    // find max score
    size_type max_i = x_size-1;
    size_type max_j = y_size-1;
    ScoreType max_score = sw_matrix[max_i][max_j];

    for( size_type j = 1; j < y_size; j++ )
    {
        if( sw_matrix[x_size-1][j] > max_score )
        {
            max_score = sw_matrix[x_size-1][j];
            max_i = x_size-1;
            max_j = j;
        }
    }

    for( size_type i = 1; i < x_size; i++ )
    {
        if( sw_matrix[i][y_size-1] > max_score )
        {
            max_score = sw_matrix[i][y_size-1];
            max_i = i;
            max_j = y_size-1;
        }
    }

    std::list< AlignmentAlphabet > edit_string;
	uint64_t num_of_matches = 0;

    // traceback to find alignment
    size_type i = max_i;
    size_type j = max_j;
    while( i > 0 && j > 0 )
    {
        ScoreType diag_score = sw_matrix[i-1][j-1] + (a.at(begin_a + j-1) == b.at(begin_b + i-1) ? _match_score : _mismatch_score );
        ScoreType up_score = sw_matrix[i-1][j] + _gap_score;
        //ScoreType left_score = sw_matrix[i][j-1] + _gap_score;

        if( sw_matrix[i][j] == diag_score )
        {
			if( a.at(begin_a + j-1) == b.at(begin_b + i-1) )
			{
				edit_string.push_front(MATCH);
				num_of_matches++;
			}
			else
			{
				edit_string.push_front(MISMATCH);
			}
			//edit_string.push_front( a.at(begin_a + j-1) == b.at(begin_b + i-1) ? MATCH : MISMATCH );
            i--;
            j--;
        }
        else if( sw_matrix[i][j] == up_score )
        {
            edit_string.push_front(GAP_A);
            i--;
        }
        else // sw_matrix[i][j] == left_score
        {
            edit_string.push_front(GAP_B);
            j--;
        }
    }

    for( size_type i=0; i < x_size; i++ ) delete[] sw_matrix[i];

	// identity of the sequences aligned
	double homology = (edit_string.size() == 0) ? 0 : double(num_of_matches * 100) / double(edit_string.size());

    MyAlignment sw_alignment( j, i, a.size(), b.size(), max_score, homology, edit_string );
    return sw_alignment;
}