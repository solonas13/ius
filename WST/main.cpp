/**
 *    Weighted Index
 *    Copyright (C) 2017 Carl Barton, Tomasz Kociumaka, Chang Liu, Solon P. Pissis and Jakub Radoszewski.
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
#include <fstream>
#include <string>
#include <set>
#include <cstring>
#include <chrono>
#include <ctime>
#include <malloc.h>

#include "input.h"
#include "weighted_sequence.h"
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>

using namespace std;
using namespace std::chrono;
using get_time = std::chrono::steady_clock;

int main (int argc, char ** argv ) {
    	Settings st = decode_switches(argc, argv);
	istream& text = st.text.is_open()?st.text:cin;
    	string& pfile_prefix = st.patterns;
    	ostream& output = st.output.is_open()?st.output:cout;

	auto begin = get_time::now();
	struct mallinfo2 mi;
    	mi = mallinfo2();
	double begin_ram = mi.hblkhd + mi.uordblks;

	WeightedSequence W;
    	text >> W;
    	W.build_index(st.z, st.quiet, output);
		
	mi = mallinfo2();
	double end_ram = mi.hblkhd + mi.uordblks;

	
	auto end = get_time::now();
	auto diff = end - begin;
	output << "CT " << chrono::duration_cast<chrono::milliseconds>(diff).count() << endl;
	output << "IS " << (end_ram-begin_ram)/1000000 << endl;
		
	string pfile_suffix[7] = {"p32.txt.gz","p64.txt.gz","p128.txt.gz","p256.txt.gz","p512.txt.gz","p1024.txt.gz","p2048.txt.gz"};
	for(string ps : pfile_suffix){
		string pfile = pfile_prefix + ps;
		ifstream file(pfile, std::ios_base::in | std::ios_base::binary);
		boost::iostreams::filtering_istream patterns;
		patterns.push(boost::iostreams::gzip_decompressor());
		patterns.push(file);		
		auto begin1 = get_time::now();

		int total_occ_no = 0;
		for (string str; getline(patterns, str); ){
			vector<int> occs = W.occurrences(str);
			total_occ_no += occs.size();
		}
		auto end1 = get_time::now();
		auto diff1 = end1 - begin1;
		output << "PMT "<< chrono::duration_cast<chrono::milliseconds>(diff1).count()<<"\n" << "OCCS " << total_occ_no << endl;
	}

    return 0;
}

