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
#include <malloc.h>

#include "node.h"
#include "defs.h"
#include "input.h"
#include "estimation.h"
#include "property_string.h"
#include "pattern_matching.h"

#include <divsufsort.h>
#include <sdsl/rmq_support.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

using namespace std;
using namespace std::chrono;
using get_time = chrono::steady_clock;

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

int main (int argc, char ** argv )
{
    	Settings st = decode_switches(argc, argv);
	istream& text_file = st.text.is_open()?st.text:cin;
	ostream& output_file = st.output.is_open()?st.output:cout;
    	string& pfile_prefix = st.patterns;
	ofstream result;
	
	double z = st.z;
	
	auto begin = get_time::now();
	struct mallinfo2 mi;
    	mi = mallinfo2();
	double begin_ram = mi.hblkhd + mi.uordblks;
	string alphabet;
	vector<vector<double>> text;
	
	size_t N;
	text_file >> N;
	text_file >> alphabet;
	size_t A = alphabet.size();
	for (size_t i = 0; i < N; ++i) {
        double sum = 0;
        vector<double> symbol(A, 0);
        for (size_t j = 0; j < A; ++j) {
            text_file >> symbol[j];
            sum += symbol[j];
        }
        text.emplace_back(symbol);
    	}
	mi = mallinfo2();
	double rmq_space = mi.hblkhd + mi.uordblks;

	Estimation fS(text, alphabet, z);
	PropertyString fT;
	
	for(PropertyString const & s : fS.strings()){
		fT += s;
	}
	
	fS.clear();
	string zstrs = fT.string();
	
	size_t Nz = fT.string().size();

	int * SA	= new int [Nz];
	int * LCP	= new int [Nz];
	vector<int> le;
	vector<int> re;
	extention(text, zstrs, alphabet, le, re, z);
	vector<int>().swap(le);
	fT._pi = re;
	
	begin = get_time::now();
	// maximalSF(text, zstrs, Nz, N, z, ME);
	SA_LCP_index ( text, zstrs.c_str(), Nz, N, z, SA, LCP );
	union_find_resort ( SA, LCP, re.data(), Nz );
	
	vector<int> tmp_lcp ( LCP, LCP+Nz);
	rmq_succinct_sct<> rmq ( &tmp_lcp );
	vector<vector<double>>().swap(text);
	vector<int>().swap(tmp_lcp);
	
	mi = mallinfo2();
		
	double end_ram = mi.hblkhd + mi.uordblks;
	auto end = get_time::now();
	auto diff2 = end - begin;
	output_file << "CT: "<< chrono::duration_cast<chrono::milliseconds>(diff2).count()<<endl;	
	output_file << "IS: " << (end_ram-begin_ram)/1000000 << endl;
	
	string pfile_suffix[7] = {"p32.txt.gz","p64.txt.gz","p128.txt.gz","p256.txt.gz","p512.txt.gz","p1024.txt.gz","p2048.txt.gz"};
	for(string ps : pfile_suffix){	
		string pfile = pfile_prefix + ps;
		begin = get_time::now();
		ifstream file(pfile, std::ios_base::in | std::ios_base::binary);
		boost::iostreams::filtering_istream patterns;
		patterns.push(boost::iostreams::gzip_decompressor());
		patterns.push(file);		
		size_t total_occ_no = 0;
		for (string pattern; getline(patterns, pattern); ){
			int m = pattern.size();
			set<int64_t> valid_res;
			// output_file << pattern << ":";
			pair<int64_t ,int64_t> interval = pattern_matching ( pattern, fT, SA, LCP, rmq, (int64_t)Nz );
			for ( auto i = interval.first; i <= interval.second; i++ ){
				if(re[SA[i]]+1 >= pattern.size()){
					valid_res.insert(SA[i]%N);
				}
			}
			total_occ_no += valid_res.size();
			// for(auto occ : valid_res){
				// output_file << occ << " ";
			// }
			// output_file << endl;
		}
		end = get_time::now();
		auto diff3 = end - begin;
		output_file << pfile << " PMT: "<< chrono::duration_cast<chrono::milliseconds>(diff3).count()<<"\n OCCS " << total_occ_no << endl;
	}	
	
	return 0;
}

