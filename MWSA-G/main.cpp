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

#include <iostream>
#include <string>
#include <chrono>
#include <list>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <malloc.h>

#include "defs.h"
#include "heavy_string.h"
#include "input.h"
#include "estimation.h"
#include "property_string.h"
#include "krfp.h"
#include "minimizers.h"
#include "sa.h"
#include "grid.h"
#include "pattern_matching.h"

#include <divsufsort.h>
#include <sdsl/rmq_support.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

typedef grid_point point;
typedef grid_query query;

using namespace std;
using namespace std::chrono;
using get_time = chrono::steady_clock;

bool sort_sa(const pair<int,int> &a,const pair<int,int> &b)
{
       return a.first<b.first;
}

bool is_valid(vector<vector<double>>& text, string& alph, string& p, int   pos, double z){
		unordered_map<char, int> mapping;
		pos = pos%(text.size());
		if(pos + p.size() > text.size()) return false;
		for(int i = 0; i < alph.size(); i++){
			mapping[alph[i]] = i;
		}
		double cum_prob = 1;
		for(int i = 0; i < p.size(); i++){
			cum_prob *= text[pos+i][mapping[p[i]]];
		}
		return (cum_prob*z >= 1)?true:false;
}

int main (int argc, char ** argv )
{
    Settings st = decode_switches(argc, argv);
	istream& text_file = st.text.is_open()?st.text:cin;
	ostream& output_file = st.output.is_open()?st.output:cout;
	ofstream result;
	
	auto begin = get_time::now();
	struct mallinfo2 mi;
    mi = mallinfo2();
	double begin_ram = mi.hblkhd + mi.uordblks;
	
	double z = st.z;
	int ell = st.ell;

	string alphabet;
	vector<vector<double>> text;
	karp_rabin_hashing::init();

	int   N;
	text_file >> N;
	text_file >> alphabet;
	int   A = alphabet.size();
	for (int   i = 0; i < N; ++i) {
        double sum = 0;
        vector<double> symbol(A, 0);
        for (int   j = 0; j < A; ++j) {
            text_file >> symbol[j];
            sum += symbol[j];
        }
        if (abs(sum-1) > EPS) {
            cerr << "Probabilities at position " << i << " do not sum up to 1" << endl;
            throw 1;
        }
        text.emplace_back(symbol);
    }
	cout << "finish reading" << endl;
	
	int k = ceil(4 * log2(ell) / log2(alphabet.size()));
	int w = ell - k + 1;
	
	cout << "index begin" << endl;
	Estimation fS(text, alphabet, z);
	
	string zstrs;
	vector<int> mini_pos;
	
	int   ii = 0;
	for(PropertyString const & s : fS.strings()){
		zstrs += s.string();
		unordered_set<uint64_t> M;
		string temp_s = s.string();
		compute_minimizers(temp_s, w, k, M);
		for(auto it : M)
				mini_pos.push_back(it + ii*N);
		ii++;
	}
	
	int   Nz = zstrs.size();
	string rev_zstrs(zstrs.rbegin(), zstrs.rend());
	
	vector<int> le;
	vector<int> re;
	extention(text, zstrs, alphabet, le, re, z);
	
	mini_pos.erase(remove_if(mini_pos.begin(), mini_pos.end(), [&](int i) { return re[i] < k; }), mini_pos.end());
	unordered_set<int> f_mini_pos(mini_pos.begin(), mini_pos.end());
	int g = f_mini_pos.size();

	HeavyString fH(text, zstrs, alphabet, f_mini_pos, le, re, true);
	
	fS.clear();
	vector<vector<double>>().swap(text);
		
	int   * forward_SA		= new int   [Nz];
	int   * forward_LCP	= new int   [Nz];
	int   * reverse_SA		= new int   [Nz];
	int   * reverse_LCP	= new int   [Nz];
	
	unsigned char * seq = (unsigned char *)zstrs.c_str();
	divsufsort( seq, forward_SA,  Nz );
	int  * iSA = new int   [Nz];
	for(int  i = 0; i < Nz; i++){
		iSA[forward_SA[i]] = i;
	}
	LCParray( seq, Nz, forward_SA, iSA, forward_LCP );
	
	seq = (unsigned char *)rev_zstrs.c_str();
	divsufsort( seq, reverse_SA,  Nz );
	
	for(int   i = 0; i < Nz; i++){
		iSA[reverse_SA[i]] = i;
	}
	LCParray( seq, Nz, reverse_SA, iSA, reverse_LCP );
	delete[] iSA;

	int   * RSA	 = new int   [g];
	int   * RLCP = new int   [g];
	int   * LSA	 = new int   [g];
	int   * LLCP = new int   [g];
	
	right_compacted_trie ( f_mini_pos, forward_SA, forward_LCP, Nz, RSA, RLCP, g );
	left_compacted_trie ( f_mini_pos, reverse_SA, reverse_LCP, Nz, LSA, LLCP, g );

	vector<int> le_r(le.rbegin(), le.rend());	
	union_find_resort ( RSA, RLCP, re, g );
	union_find_resort ( LSA, LLCP, le_r, g );	

	delete[] forward_SA;
	delete[] forward_LCP;
	delete[] reverse_SA;
	delete[] reverse_LCP;
	string().swap(zstrs);
	string().swap(rev_zstrs);	
	vector<int>().swap(mini_pos);
	unordered_set<int>().swap(f_mini_pos);
		
	vector<int> tmp_llcp(LLCP, LLCP+g);
	vector<int> tmp_rlcp(RLCP, RLCP+g);	
	rmq_succinct_sct<> lrmq ( &tmp_llcp );
	rmq_succinct_sct<> rrmq ( &tmp_rlcp );

	vector<int>().swap(tmp_llcp);
	vector<int>().swap(tmp_rlcp);
	vector<int>().swap(le);
	vector<int>().swap(re);
	vector<int>().swap(le_r);

	vector<pair<int  ,int  >> l;
	vector<pair<int  ,int  >> r;
	for ( int   i = 0; i < g; i++ )
  	{
  		l.push_back( make_pair( Nz-1-LSA[i], i) );
 		r.push_back( make_pair( RSA[i], i ) );	
	}
	
 	sort(l.begin(),l.end(),sort_sa);
	sort(r.begin(),r.end(),sort_sa);

  	std::vector<point> points;
  	  	
  	for ( int   i = 0; i < g; i++ )
  	{
 
		point to_insert;
		to_insert.row = l.at(i).second+1;
		to_insert.col = r.at(i).second+1;
		
		to_insert.level = 0;
		to_insert.label = l.at(i).first;
		points.push_back(to_insert); 

  	}
  	  	
  	//constructing grid with bd-anchors
  	grid construct;
  	construct.build( points, 0 );
  	
	vector<pair<int,int>>().swap(l);
	vector<pair<int,int>>().swap(r);
	vector<point>().swap(points);

		
	mi = mallinfo2();
	
	double end_ram = mi.hblkhd + mi.uordblks;
	auto end = get_time::now();
	auto diff2 = end - begin;
	
	output_file << "CT "<< chrono::duration_cast<chrono::milliseconds>(diff2).count()<< endl;	
	output_file << "IS " << (end_ram-begin_ram)/1000000 << endl;

	if(!st.patterns.empty()){
		size_t total_occ_no = 0;

		string pfile = st.patterns;

		ifstream file(pfile, std::ios_base::in | std::ios_base::binary);
		boost::iostreams::filtering_istream patterns;
		patterns.push(boost::iostreams::gzip_decompressor());
		patterns.push(file);	
		begin = get_time::now();
		for (string pattern; getline(patterns, pattern); ){
			if(pattern.size() < k) continue;
			// output_file << pattern << ":"; 
			size_t j = pattern_minimizers(pattern, k);
			string left_pattern = pattern.substr(0, j+1);
			

			reverse(left_pattern.begin(), left_pattern.end());
			pair<int64_t ,int64_t> left_interval = rev_pattern_matching ( left_pattern, fH, LSA, LLCP, lrmq, (int64_t)g ); 
			string right_pattern = pattern.substr(j);			
			pair<int64_t ,int64_t> right_interval = pattern_matching ( right_pattern, fH, RSA, RLCP, rrmq, (int64_t)g );

			if ( left_interval.first <= left_interval.second  && right_interval.first <= right_interval.second )
			{			
				//Finding rectangle containing bd-anchors in grid
				grid_query rectangle;
				rectangle.row1 = left_interval.first+1;
				rectangle.row2 = left_interval.second+1;
				rectangle.col1 = right_interval.first+1;
				rectangle.col2 = right_interval.second+1;			
				
				vector<size_t> result;
				construct.search_2d(rectangle, result);
			
				set<int64_t> valid_res;

				for(size_t i = 0; i < result.size(); i++){
					if(valid_res.count( (RSA[result.at(i)-1]-j)%N) ) continue;
					if(fH.get_pi(RSA[result.at(i)-1], RSA[result.at(i)-1]-j, pattern.size() ) * z >= 1){
						valid_res.insert((RSA[result.at(i)-1]-j)%N);
					}
				}
				total_occ_no += valid_res.size();
					// for(auto i : valid_res)
						// output_file<< i << " ";				
			}
				// output_file << endl;	
		}
		end = get_time::now();
		auto diff3 = end - begin;
		output_file << "PMT "<< chrono::duration_cast<chrono::milliseconds>(diff3).count()<<"\n" << "OCCS " << total_occ_no << endl;

	}

	return 0;
}

