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
#include <unordered_map>
#include <deque>
#include <boost/functional/hash.hpp>

#include "defs.h"
#include "sa.h"

#include <divsufsort.h>
#include <sdsl/rmq_support.hpp>

using namespace std;
using namespace sdsl;

void right_compacted_trie ( unordered_set<int > &anchors, int  * SA, int  * LCP, int  n, int  * RSA, int  * RLCP, int  g )
{
	int  ii = 0; //this is the index over RSA[o..g-1] and the RLCP[0..g-1], where g is the number of anchors
	int  minLCP = n; //this is to maint ain the minimum LCP in a range over LCP[i..j]: stores the LCP of SA[i] and SA[j]

	for( int  i = 0; i < n; i++ ) // in lex order
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
  			// cout<< i << "RSA[i]: "<< RSA[ii] <<" RLCP[i]: "<<RLCP[ii]<<"\n"; 
			// getchar();
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
void left_compacted_trie ( unordered_set<int > &anchors, int  * SA, int  * LCP, int  n, int  * RSA, int  * RLCP, int  g )
{
	int  ii = 0;
	int  minLCP = n;
	for( int  i = 0; i < n; i++ ) // in lex order
	{
		/* If the ith lex suffix is an anchor then add it to the compacted trie (encoded in arrays RSA and RLCP) */
		auto it = anchors.find( ( n - 1) - SA[i] );
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
	if(ii < anchors.size()){
		while(ii != anchors.size()){
			RSA[ii] = n;
			ii++;
		}
	}
}

// int  fast_RMQ ( int  * arr, int  L, int  R, int  n, std::unordered_map<pair<int ,int >, int , boost::hash<pair<int ,int > >> &rmq )
// {
	// if ( R - L > 1 ) // int ernal nodes
	// {
		// int  M = (L+R)/2;
		// int  a = fast_RMQ ( arr, L, M, n, rmq ); //Recurse on the left
		// int  b = fast_RMQ ( arr, M, R, n, rmq ); //Recurse on the right
		// int  value = std::min(a, b); 	     //This is the minimum of arr[L+1..M]

		// pair<int ,int > p(L+1, R);
		// pair<pair<int ,int >,int > np(p, value);
		// rmq.insert(np);
		// return value;
	// }
	// else  // Base case: leaf nodes
	// {
		// int  value;
		// if ( R >= n ) value = 0;
		// else value = std::min (arr[L+1], arr[R]);
		// pair<int ,int > p(L+1, R);
		// pair<pair<int ,int >,int > np(p, value);
		// rmq.insert(np);
		// return value;
	// }
// }


int  LCParray ( unsigned char * text, size_t n, int  * SA, int  * ISA, int  * LCP )
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

void extention ( vector<vector<double>>& text, string& s, string& alph, vector<int>& le, vector<int>& re, double z ){
	int n = text.size();
	int N = s.size();
	unordered_map<char, int> A;
	for(int i = 0; i < alph.size(); i++){
		A[alph[i]] = i;
	}
	
	vector<double> _pi;
	for(int i = 0; i < N; i++){
		_pi.push_back( text[i%n][A[s[i]]]);
	}	
	
	int l = 0;
	double cum_pi = 1;
	
	re.assign(N,0);	
	for (auto j = 0; j < z; j++ ){
		for(auto i = 0; i < n; i++){
			int si = i + j*n;
			if( i == 0 || re[si-1] < 1 ){
				l = 0;
				cum_pi = 1;
				for( l = 0; l < n-i-1; l++){
					if(cum_pi * _pi[si+l] * z >= 1){
						cum_pi *= _pi[si+l];
					}else{
						break;
					}
				}
				re[si] = l-1;
			}else{
				l = re[si-1];
				cum_pi /= _pi[si-1];
				if ( cum_pi * z < 1 ){
					re[si] = l -1;
				}else{
					for ( l = l; l < n-i-1; l++ ){
						if(cum_pi * _pi[si+l] * z >= 1){
							cum_pi *= _pi[si+l];
						}else{
							break;
						}
					}
					re[si] = l-1;
				}
			}
		}
	}
	
	le.assign(N,0);
	reverse(_pi.begin(), _pi.end());
	for (auto j = 0; j < z; j++ ){
		for(auto i = 0; i < n; i++){
			int si = i + j*n;
			if( i == 0 || le[si-1] < 1 ){
				l = 0;
				cum_pi = 1;
				for( l = 0; l < n-i; l++){
					if(cum_pi * _pi[si+l] * z >= 1){
						cum_pi *= _pi[si+l];
					}else{
						break;
					}
				}
				le[si] = l-1;
			}else{
				l = le[si-1];
				cum_pi /= _pi[si-1];
				if ( cum_pi < 1/z )
					le[si] = l -1;
				else
				{
					for ( l = l; l < n-i; l++ )
					{
						if(cum_pi * _pi[si+l] * z >= 1){
							cum_pi *= _pi[si+l];
						}else{
							break;
						}
					}
					le[si] = l-1;
				}
			}
		}	
	}
	reverse(le.begin(), le.end());
}

class UF    {
	int *id, cnt, *sz;
	public:
	// Create an empty union find data structure with N isolated sets.
	UF(int N)   {
		cnt = N;
		id = new int[N];
		sz = new int[N];
		for(int i=0; i<N; i++)	{
			id[i] = i;
			sz[i] = 1;
		}
	}
	~UF()	{
		delete [] id;
		delete [] sz;
	}
	// Return the id of component corresponding to object p.
	int find(int p)	{
		int root = p;
		while (root != id[root])
			root = id[root];
		while (p != root) {
			int newp = id[p];
			id[p] = root;
			p = newp;
		}
		return root;
	}
	// Replace sets containing x and y with their union.
	void merge(int x, int y)	{
		int i = find(x);
		int j = find(y);
		if (i == j) return;

		if ( i < j ){
			id[j] = i;
		}	
		else	{
			id[i] = j;
		}
		cnt--;
	}
	// Are objects x and y in the same set?
	bool connected(int x, int y)    {
		return find(x) == find(y);
	}
	// Return the number of disjoint sets.
	int count() {
		return cnt;
	}
};

void union_find_resort( int * SA, int * LCP, vector<int>& property, int n )
{
	UF uf(n);
	int * ME = new int [property.size()];
	for(int i = 0; i < property.size(); i++){
		ME[i] = property[i]+1;
	}
	
	vector< deque <int> > pSA (n);
	vector< int > pLCP (n, 0);
	vector< int > iSA (property.size(), 0 );
	map<int, vector<int> > lcp_map;
	map<int, vector<int> > me_map;
	int max_lcp = 0;
	int max_me = 0;
	for ( int i = 0; i < n; i++ )
	{
		iSA[SA[i]] = i;
		lcp_map[LCP[i]].push_back(i);
		if ( max_lcp < LCP[i] )
			max_lcp = LCP[i];
		me_map[ME[SA[i]]].push_back(i);
		if ( max_me < ME[SA[i]] )
			max_me = ME[SA[i]];
	}

	for ( int l = max( max_lcp, max_me ); l >= 0; l-- )
	{
		if ( !lcp_map[l].empty() )
		{
			for ( auto it = lcp_map[l].begin(); it != lcp_map[l].end(); it ++ )
			{
				if( *it > 0 )
				{
					uf.merge( *it-1, *it );
				}
			}
		}
		if ( !me_map[l].empty() )
		{
			for ( auto it = me_map[l].begin(); it != me_map[l].end(); it++ )
			{
				int f = uf.find( *it );
				pSA[f].push_front ( SA[*it] );
			}
		}
	}
	int counter = 0;
	int j = 0;
	
	for ( int i = 0; i < n; i++ )
	{
		if ( counter > 0 && i > 0 && !pSA[i].empty() )
		{
			int k1 = SA[counter-1];
			int k2 = pSA[i][0];
			if ( iSA[k1] > iSA[k2] )
			{
				pLCP[counter] = ME[k1];
			}
			else
			{
				int rmq = LCP[++j];
				for ( ; j <= i; j++ )
					rmq = min ( rmq, LCP[j] );
				pLCP[counter] = min ( rmq, min( ME[k1], ME[k2] ) );
			}
		}					
		for ( auto it = pSA[i].begin(); it != pSA[i].end(); it++ )
		{
			if ( it != pSA[i].begin() )
				pLCP[counter] = ME[*(it-1)];
			SA[counter++] = *it;
		}
		if ( !pSA[i].empty() )	j = i;
	}
	for ( int i = 0; i < n; i++ )
	{
		LCP[i] = pLCP[i];
	}
			
	delete[] ME;
	map<int, vector<int>>().swap(lcp_map);
	map<int, vector<int>>().swap(me_map);
}
