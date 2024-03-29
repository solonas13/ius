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

#include "input.h"
#include "estimation.h"
#include "property_string.h"

using namespace std;
using namespace std::chrono;
using get_time = chrono::steady_clock;

int main (int argc, char ** argv )
{
    Settings st = decode_switches(argc, argv);
	string output = st.output;
	string alphabet;
	size_t N;
	double z = st.z;
	vector<vector<double>> text;
	
	ifstream text_file(st.text);
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
		if (abs(sum-1) > EPS) {
			cerr << " Error: Probabilities at position " << i << " do not sum up to 1!" << endl;
			throw 1;
		}
		text.emplace_back(symbol);
	}
	cout << "Finish reading the input" << endl;
	cout << N << endl;
	cout << alphabet << endl;
	cout << text.size() << endl;
	
	Estimation fS(text, alphabet, z);
	cout << "The z-strings are constructed" << endl;
	PropertyString fT;
	
	for(PropertyString const & s : fS.strings()){
		fT += s;
	}
	
	fS.clear();
	string zstrs = fT.string();
	size_t Nz = fT.string().size();

	int num_of_pattern = Nz/200;
	if ( num_of_pattern <= 0 ) { cerr << " Error: n * z is too small to generate long patterns! " << endl; throw 1;}
	ofstream pattern64(output+"p64.txt");
	ofstream pattern128(output+"p128.txt");
	ofstream pattern256(output+"p256.txt");
	ofstream pattern512(output+"p512.txt");
	ofstream pattern1024(output+"p1024.txt");
	
	srand(time(NULL));
	size_t p = 0;
	int pos = Nz - 2000; 
	if ( pos <= 0 ) { cerr << " Error: n * z is too small to generate long patterns! " << endl; throw 1; }
	for(int i = 0; i < num_of_pattern; i++){
		p = rand()%(pos);
		pattern64 << zstrs.substr(p,64) << endl;
		p = rand()%(pos);
		pattern128 << zstrs.substr(p,128) << endl;
		p = rand()%(pos);
		pattern256 << zstrs.substr(p,256) << endl;
		p = rand()%(pos);
		pattern512 << zstrs.substr(p,512) << endl;
		p = rand()%(pos);
		pattern1024 << zstrs.substr(p,1024) << endl;
	}
	
	return 0;
}

