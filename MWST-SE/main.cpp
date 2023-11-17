/**
 *    ius: indexing uncertain strings
 *    Copyright (C) 2023 E. Gabory, C. Liu, G. Loukides, S. P. Pissis, and W. Zuba.
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <iostream>
#include <cstdio>
#include <string>
#include <chrono>
#include <list>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <malloc.h>
#include <cmath>

#include "input.h"
#include "minimizer_index.h"
#include "krfp.h"
#include "heavy_string.h"


#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>


using namespace std;
using get_time = chrono::steady_clock;

int main (int argc, char ** argv )
{
    	Settings st = decode_switches(argc, argv);
	istream& text = st.text.is_open()?st.text:cin;
	ostream& output_file = st.output.is_open()?st.output:cout;
	ofstream result;
	int l=st.ell;
	double z=st.z;
	
	auto begin = get_time::now();
	// struct mallinfo2 mi;
    	// mi = mallinfo2();
	// double begin_ram = mi.hblkhd + mi.uordblks;
	
	karp_rabin_hashing::init();
	MinimizerIndex M;
	text >> M;
	//cout << "finish reading" << endl;
	M.build_index(z,l);
	//cout << "Minimizer Index build finish" << endl;
	//string Pattern="TCTCT";
	//M.occurrences(Pattern,l,z,output_file);
	// mi = mallinfo2();
	
	// double end_ram = mi.hblkhd + mi.uordblks;
	auto end = get_time::now();
	auto diff2 = end - begin;
	output_file << "CT "<< chrono::duration_cast<chrono::milliseconds>(diff2).count()<<endl;	
	// output_file << "CS " << (end_ram-begin_ram)/1000000 << endl;
	
	if(!st.patterns.empty())
	{
		int total_occ = 0;
		begin = get_time::now();		
		ifstream file(st.patterns, std::ios_base::in | std::ios_base::binary);
		boost::iostreams::filtering_istream patterns;
		patterns.push(boost::iostreams::gzip_decompressor());
		patterns.push(file);	
		begin = get_time::now();
		//cout<<patterns.size()<<" patterns to check"<<endl;
		for (string pattern; getline(patterns, pattern); ){
			//cout << pattern << ":"<<endl;
			std::vector<int> occs = M.occurrences(pattern, l, z, output_file);
			//if (occs.empty()) {
			// output_file << "\n";
			//} else {
				// for (auto p : occs) {
					// output_file << p << " ";
				// }
				// output_file << endl;
			//}
			total_occ += occs.size();
		}
		end = get_time::now();
		auto diff = end - begin;
		output_file << "PMT " << chrono::duration_cast<chrono::milliseconds>(diff).count() << "\nOCCS " << total_occ << endl;
	}





	return 0;
}

