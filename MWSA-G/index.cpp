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

#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_set>

#include "defs.h"

#include <divsufsort.h>
#include <sdsl/rmq_support.hpp>

using namespace std;
using namespace sdsl;

int LCParray ( unsigned char * text, size_t n, int * SA, int * ISA, int * LCP )
{
        size_t i=0, j=0;

        LCP[0] = 0;
        for ( i = 0; i < n; i++ ) // compute LCP[ISA[i]]
                if ( ISA[i] != 0 )
                {
                        if ( i == 0) j = 0;
                        else j = (LCP[ISA[i-1]] >= 2) ? LCP[ISA[i-1]]-1 : 0;
                        while ( text[i+j] == text[SA[ISA[i]-1]+j] )
                                j++;
                        LCP[ISA[i]] = j;
                }

        return ( 1 );
}

void SA_LCP_index ( vector<vector<double>> text, const char * sq, int N, int n, double z, int * SA, int * LCP )
{
	int i = 0;
	int j = 0;
	// compute SA:
	unsigned char * psq = new unsigned char [N];
	for ( i = 0; i < N; i++ )
		psq[i] = sq[i];
	if ( divsufsort ( psq, SA, N ) != 0 )
	{
		fprintf ( stderr, " Error: Cannot allocate memory.\n" );
		exit ( EXIT_FAILURE );
	}

	int * iSA = new int [N];
	// compute inverseSA:
	for ( i = 0; i < N; i++ )
	{
		iSA[SA[i]] = i;
	}
	
	// compute LCP:
	LCP[0] = 0;
	int l = 0;
	for ( i = 0; i < N; i++ )
	{
		if ( iSA[i] != 0 )
		{
			if ( i == 0 ) l = 0;
			else l = ( LCP[iSA[i-1]]>=2 ) ? LCP[iSA[i-1]]-1 : 0;
			while ( sq[i+l] == sq[SA[iSA[i]-1]+l] )
				l ++;
			LCP[iSA[i]] = l;
		}
	}
	delete[] psq;
	delete[] iSA;
}

struct compare
{
	int * SA;
	compare ( int * sa ):
		SA(sa){}
	bool operator() ( int a, int b )
	{
		return SA[a] < SA[b];
	}
};

// A utility function to get maximum value in arr[]
int getMax( vector<int> & arr, int n )
{
    int mx = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > mx)
            mx = arr[i];
    return mx;
}
 
// A function to do counting sort of arr[] according to
// the digit represented by exp.
void countSort(vector<int> & arr, vector<int>& p, int n, int exp)
{
    int * output = new int [n]; // output array
	int * position = new int [n];
    int i, count[10] = {0};
 
    // Store count of occurrences in count[]
    for (i = 0; i < n; i++)
        count[ (arr[i]/exp)%10 ]++;
 
    // Change count[i] so that count[i] now contains actual
    //  position of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];
 
    // Build the output array
    for (i = n - 1; i >= 0; i--)
    {
        output[count[ (arr[i]/exp)%10 ] - 1] = arr[i];
		position[count[(arr[i]/exp)%10]-1] = p[i];
        count[ (arr[i]/exp)%10 ]--;
    }
 
    // Copy the output array to arr[], so that arr[] now
    // contains sorted numbers according to current digit
    for (i = 0; i < n; i++)
	{
        arr[i] = output[i];
		p[i] = position[i];
	}
}
 
// The main function to that sorts arr[] of size n using 
// Radix Sort
void radixsort(vector<int> & v, vector<int>& p )
{
    // Find the maximum number to know number of digits
    int m = getMax(v, v.size());
 
    // Do counting sort for every digit. Note that instead
    // of passing digit number, exp is passed. exp is 10^i
    // where i is current digit number
    for (int exp = 1; m/exp > 0; exp *= 10)
        countSort(v, p, v.size(), exp);
}

void print ( vector<int>& x )
{
	for ( auto i = x.begin(); i != x.end(); i++ )
		cout << *i << ' ';
	cout << endl;
}

void WeightedIndex ( int n, int N, int * SA, int * LCP, int * ME, vector<int> & WSA, vector<int>& WLCP )
{
	int i = 0, j = 0, k = 0;
	vector<int> position_check;
	position_check.reserve ( N );
	while ( ME[SA[i]] == 0 )
	{
		i++;
	}
	position_check.push_back ( SA[i] );
	for ( i = i + 1; i < N; i++ )
	{
		if ( position_check.empty() )
		{
			position_check.push_back ( SA[i] );
			k = i;
		}
		else
		{
			if ( LCP[i] == ME[SA[i-1]] && ME[SA[i-1]] == ME[SA[i]] )
			{
				position_check.push_back ( SA[i] );
			}
			else
			{
				vector<int> position;
				vector<int> remainder;
				position.reserve ( position_check.size() );
				remainder.reserve ( position_check.size() );
				for ( j = 0; j < position_check.size(); j++ )
				{
					remainder.push_back( position_check[j]%n );
					position.push_back( j );
				}
				radixsort ( remainder, position );
				WSA.push_back( position_check[position[0]] );
				WLCP.push_back ( LCP[k] );
				int lcp_counter = 0;
				for ( j = 1; j < position_check.size(); j++ )
				{
					if ( remainder[j] != remainder[j-1] )
					{
						WSA.push_back ( position_check[position[j]] );
						lcp_counter ++;
					}
				}
				while ( lcp_counter )
				{
					WLCP.push_back ( LCP[i-1] );
					lcp_counter--;
				}
				position_check.clear();
				position_check.push_back ( SA[i] );
				k = i;
			}
		}
	}
	vector<int> position;
	vector<int> remainder;
	position.reserve ( position_check.size() );
	remainder.reserve ( position_check.size() );
	for ( j = 0; j < position_check.size(); j++ )
	{
		remainder.push_back( position_check[j]%n );
		position.push_back( j );
	}
	radixsort ( remainder, position );
	WSA.push_back( position_check[position[0]] );
	WLCP.push_back ( LCP[i-position_check.size()] );
	int lcp_counter = 0;
	for ( j = 1; j < position_check.size(); j++ )
	{
		if ( remainder[j] != remainder[j-1] )
		{
			WSA.push_back ( position_check[position[j]] );
			lcp_counter ++;
		}
	}
	while ( lcp_counter )
	{
		WLCP.push_back ( LCP[N-1] );
		lcp_counter--;
	}
}

void right_compacted_trie ( unordered_set<int> &anchors, int * SA, int * LCP, int n, int * RSA, int * RLCP, int g )
{
	int ii = 0; //this is the index over RSA[o..g-1] and the RLCP[0..g-1], where g is the number of anchors
	int minLCP = n; //this is to maintain the minimum LCP in a range over LCP[i..j]: stores the LCP of SA[i] and SA[j]

	for( int i = 0; i < n; i++ ) // in lex order
	{
		/* If the ith lex suffix is an anchor then add it to the compacted trie (encoded in arrays RSA and RLCP) */
		auto it = anchors.find( SA[i] );
		if( it != anchors.end() )
		{
			RSA[ii] = SA[i];		// store this suffix
			if ( ii == 0 )	RLCP[ii] = 0; 	// if it is the first time the RLCP = LCP = 0
			else
			{
				if ( SA[i-1] == RSA[ii-1] )     // if the immediately prior suffix was added
					RLCP[ii] = LCP[i];	// then the LCP value is the correct one for RLCP
				else
					RLCP[ii] = std::min(minLCP, LCP[i]);	//otherwise, we should take the minimum in the range
			}
  			//cout<<"RSA[i]: "<< RSA[ii] <<" RLCP[i]: "<<RLCP[ii]<<"\n"; getchar();
			minLCP = n; // set this to something high to get the _FIRST_ next minimum value, because a new range STARTS
			ii++;
		}
		else /* Do not add it but remember the minLCP seen so far in a range*/
		{
			if ( LCP[i] < minLCP )
				minLCP = LCP[i];
		}
	}
}

/* Constructs the left compacted trie given the anchors and the SA of the whole string in O(n) time */
void left_compacted_trie ( unordered_set<int> &anchors, int * SA, int * LCP, int n, int * RSA, int * RLCP, int g )
{
	int ii = 0;
	int minLCP = n;
	for( int i = 0; i < n; i++ ) // in lex order
	{
		/* If the ith lex suffix is an anchor then add it to the compacted trie (encoded in arrays RSA and RLCP) */
		auto it = anchors.find( ( n - 1 ) - SA[i] );
		if( it != anchors.end() )
		{
			RSA[ii] = SA[i];		// store this suffix
			if ( ii == 0 )	RLCP[ii] = 0; 	// if it is the first time the RLCP = LCP = 0
			else
			{
				if ( SA[i-1] == RSA[ii-1] ) // if the immediately prior suffix was added
					RLCP[ii] = LCP[i];	//then the LCP value is the correct one for RLCP
				else
					RLCP[ii] = std::min(minLCP, LCP[i]);	//otherwise, we should take the minimum in the range
			}

			//cout<<"LSA[i]: "<< RSA[ii] <<" LLCP[i]: "<< RLCP[ii]<<"\n"; getchar();
			minLCP = n; //set this to something high to get the FIRST next minimum value
			ii++;
		}
		else /* Do not add it but remember the minLCP seen so far in a range*/
		{
			if ( LCP[i] < minLCP )
				minLCP = LCP[i];
		}
	}
}

