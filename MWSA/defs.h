/**
    Weighted Suffix Array
    Copyright (C) 2017 Panagiotis.Charalampopoulos, Costas.Iliopoulos, Chang.Liu, Solon.Pissis
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
#ifndef DEFS_H 
#define DEFS_H


#include <string>
#include <list>
#include <forward_list>

#include <sdsl/rmq_support.hpp>

using namespace std;
using namespace sdsl;

int LCParray ( unsigned char * text, size_t n, int * SA, int * ISA, int * LCP );
void usage ( void );
int read ( string filename, double *** text, int& n );
void SA_LCP_index (vector<vector<double>> text, const char * sq, int N, int n, double z, int * SA, int * LCP);
void union_find_resort( int * SA, int * LCP, int *ME, int n );
void maximalSF ( double ** text, string & sq, int N, int n, double z, int * ME );
void WeightedIndex ( int n, int N, int * SA, int * LCP, int * ME, vector<int> & WSA, vector<int>& WLCP );

#endif