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

#ifndef CONTIG_CODE_
#define CONTIG_CODE_

#include "assembly/contig.hpp"
#include "alignment/alignment.hpp"

#include<sstream>
#include<ios>
#include <stdexcept>

size_t
Contig::resize(const size_t& size)
{
(this->_sequence).resize(size);
//(this->_quality).resize(size);

return this->size();
}

Contig::Contig(): _name(""), _sequence(0) {} //, _quality(0) {}

Contig::Contig(const Contig& orig): _name(orig._name),
                            _sequence(orig._sequence) {}
                            //_quality(orig._quality) {}

Contig::Contig(const std::string &name): _name(name),
                            _sequence(0) {} //, _quality(0) {}

Contig::Contig(const std::string &name, const SeqType &sequence,
                                        const QualSeqType &quality):
                                    _name(name), _sequence(sequence) {}
                                                 // _quality(quality)
//{
//  if (_quality.size() != _sequence.size()) {
//    std::stringstream s;
//    s << "Sequence size does not match quality "<<
//                                "sequence size for \""<< name <<"\".";
//    throw std::logic_error(s.str());
//  }
//}

Contig::Contig(const std::string &name, const SeqType &sequence):
               _name(name), _sequence(sequence) {} //, _quality(sequence.size())
//{
//  for (QualSeqType::iterator i=this->_quality.begin();
//                             i!=this->_quality.end(); i++) {
//    *i=100;
//  }
//}

Contig::Contig(const std::string &name, const size_t& size):
       _name(name), _sequence(size) {} //, _quality(size) {}

Contig::Contig(const size_t& size):
       _name(), _sequence(size) {} //, _quality(size) {}

const std::string &
Contig::name() const { return this->_name; }

const Contig&
Contig::set_name(const std::string& new_name)
{
  this->_name=new_name;

  return *this;
}

size_t
Contig::size() const
{ return (this->_sequence).size(); }

const Contig&
Contig::operator=(const Contig& orig)
{
  this->_name=orig._name;
  this->_sequence=orig._sequence;
  //this->_quality=orig._quality;

  return *this;
}

bool
Contig::operator==(const Contig& ctg) const
{
   if (_sequence!=ctg._sequence)
     return false;

   // if (_quality!=ctg._quality) return false;

   return true;
}

bool
Contig::operator!=(const Contig& ctg) const
{
  return !(*this==ctg);
}

const Nucleotide&
Contig::operator[](const size_t& index) const
{
  return this->_sequence[index];
}

Nucleotide&
Contig::operator[](const size_t& index)
{
  return this->_sequence[index];
}

const Nucleotide&
Contig::at(const size_t& index) const
{
  return this->_sequence.at(index);
}

Nucleotide&
Contig::at(const size_t& index)
{
  return this->_sequence.at(index);
}

//const QualType&
//Contig::qual(const size_t& index) const
//{
//  return this->_quality.at(index);
//}
//
//QualType&
//Contig::qual(const size_t& index)
//{
//  return this->_quality.at(index);
//}

SeqType
Contig::sequence(const size_t& index) const
{
  return sequence(index, size()-index);
}

SeqType
Contig::sequence(const size_t& index, const size_t& length) const
{
  if (index+length>size()) {
    throw std::domain_error("The contig has not so many bases.");
  }

  SeqType out_seq(length);

  for (size_t i=index; i<index+length; i++) {
    out_seq.at(i)=this->_sequence.at(i);
  }

  return out_seq;
}

Contig &
reverse(Contig& ctg)
{
  //QualSeqType quality(ctg.size());
  if( ctg.size() == 0 ) return ctg;

  unsigned int i = 0, j = ctg.size()-1;
  Nucleotide temp;

  ctg.set_name( "Reversed "+ctg.name() );
  while( i < j )
  {
      temp = ctg.at(i);
      ctg.at(i) = ctg.at(j);
      ctg.at(j) = temp;
      i++;
      j--;
  }

  return ctg;
  //return Contig(name,sequence); //return Contig(name,sequence,quality);
}

Contig &
complement(Contig& ctg)
{
  //SeqType sequence(ctg.size());
  //QualSeqType quality(ctg.size());

  for (unsigned int i=0; i< ctg.size(); i++) {
    ctg.at(i)=complement(ctg.at(i));
    //quality.at(i)=ctg.qual(i);
  }

  return ctg;
  //return Contig(ctg.name(),sequence); //return Contig(name,sequence,quality);
}

Contig &
reverse_complement(Contig& ctg)
{
  return reverse(complement(ctg));
}

Contig
chop_borders(const Contig& ctg,
             const size_t& preserve_from,
             const size_t& preserve_until)
{
  if ((ctg.size()<=preserve_from)||(preserve_until<preserve_from)) {
    throw std::domain_error("I cannot cut more bases than the contig's ones.");
  }

  size_t max_pos=std::min(preserve_until+1,ctg.size());

  SeqType new_seq(max_pos-preserve_from);
  //QualSeqType new_qual(max_pos-preserve_from);

  for (size_t i=preserve_from; i<max_pos; i++) {
    new_seq.at(i-preserve_from)=ctg.at(i);
    //new_qual.at(i-preserve_from)=ctg.qual(i);
  }

  return Contig(ctg.name(),new_seq); //return Contig(ctg.name(),new_seq,new_qual);
}

Contig
chop_begin(const Contig& ctg, const size_t& preserve_from)
{
  return chop_borders(ctg, preserve_from, ctg.size());
}

Contig
chop_end(const Contig& ctg, const size_t& preserve_until)
{
  return chop_borders(ctg, 0, preserve_until);
}

/*Contig
merge(const Contig& a, const Contig& b,
                         const Alignment& alignment)
{
  SeqType new_seq;
  //QualSeqType new_qual;

  if (alignment.a_position_in_b()>0) {
    size_t a_in_b(alignment.a_position_in_b());
    size_t a_end=a_in_b+a.size();
    size_t new_size=std::max(a_end,b.size());
    new_seq=SeqType(new_size);
    //new_qual=QualSeqType(new_size);

    for (size_t i=0; i<new_size; i++) {
      if ((i<a_in_b)||(i>=a_end)) {
        new_seq.at(i)=b.at(i);
        //new_qual.at(i)=b.qual(i);
      } else {
        new_seq.at(i)=a.at(i-a_in_b);
        //new_qual.at(i)=a.qual(i-a_in_b);
      }
    }
  } else {
    size_t b_in_a(alignment.b_position_in_a());
    size_t b_end=b_in_a+b.size();
    size_t new_size=std::max(b_end,a.size());
    new_seq=SeqType(new_size);
    //new_qual=QualSeqType(new_size);

    for (size_t i=0; i<new_size; i++) {
      if ((i<b_in_a)||(i>=b_end)) {
        new_seq.at(i)=a.at(i);
        //new_qual.at(i)=a.qual(i);
      } else {
        new_seq.at(i)=b.at(i-b_in_a);
        //new_qual.at(i)=b.qual(i-b_in_a);
      }
    }
  }

  return Contig(a.name(),new_seq); //return Contig(a.name(),new_seq,new_qual);
}*/

#endif // CONTIG_CODE_
