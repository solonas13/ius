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
#include <algorithm>
#include <random>

#include "input.h"
#include "estimation.h"
#include "property_string.h"

using namespace std;
using namespace std::chrono;
using get_time = chrono::steady_clock;

void random_replace(vector<double>& oldVec, vector<double>& newVec){

    // Generate a vector of indices from 0 to newVec.size() - 1
    std::vector<int> indices(newVec.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Shuffle the indices
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    // Take the first 4 indices as the positions
    std::vector<int> positions(indices.begin(), indices.begin() + oldVec.size());

    // Copy elements from the old vector to the new vector at the specified positions
    for (int i = 0; i < oldVec.size(); i++) {
        newVec[positions[i]] = oldVec[i];
    }
}

int main (int argc, char ** argv )
{
    Settings st = decode_switches(argc, argv);
	string output = st.output;
	string alphabet;
	size_t N;
	vector<vector<double>> text;
	string english = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	size_t new_ab_size = 20;
	
	ifstream text_file(st.text);
	text_file >> N;
	text_file >> alphabet;
	size_t A = alphabet.size();
	srand(time(NULL));
	int rws_begin = rand()%(N-10010);
	for (size_t i = 0; i < N; ++i) {
		if( i == rws_begin + 10000) break;
		if(i >= rws_begin){
			double sum = 0;
			vector<double> symbol(A, 0);
			vector<double> new_symbol(new_ab_size,0);
			for (size_t j = 0; j < A; ++j) {
				text_file >> symbol[j];
				sum += symbol[j];
			}
			if (abs(sum-1) > EPS) {
				cerr << "Probabilities at position " << i << " do not sum up to 1" << endl;
				throw 1;
			}
			random_replace(symbol, new_symbol);
			
			text.emplace_back(new_symbol);
		}
	}
	cout << "finish read" << endl;
	cout << N << endl;
	cout << alphabet << endl;
	cout << text.size() << endl;
	
	alphabet = english.substr(0, new_ab_size);
	N = text.size();
	ofstream rws_out("rws_a10.in");
	rws_out << 10000 << endl;
	rws_out << english.substr(0,new_ab_size) << endl;

	for(int i = 0; i < 10000; i++){
		for(int j = 0; j < new_ab_size; j++){
			rws_out << text[i][j] << " ";
		}
		rws_out << endl;
	}
	
	for(int z = 16; z < 512; z *= 2){
		Estimation fS(text, english.substr(0,new_ab_size), z);
		cout << "z-strings constructed" << endl;
		PropertyString fT;
		
		for(PropertyString const & s : fS.strings()){
			fT += s;
		}
		

		fS.clear();
		string zstrs = fT.string();
		size_t Nz = fT.string().size();
		string cz = to_string(z);

		int num_of_pattern = Nz/200;
		ofstream pattern32(output+ cz + "p32.txt");
		ofstream pattern64(output+ cz+"p64.txt");
		ofstream pattern128(output+ cz+"p128.txt");
		ofstream pattern256(output+ cz+"p256.txt");
		ofstream pattern512(output+ cz+"p512.txt");
		ofstream pattern1024(output+ cz+"p1024.txt");
		
		srand(time(NULL));
		size_t p = 0;
		for(int i = 0; i < num_of_pattern; i++){
			p = rand()%(Nz-3000);
			pattern32 << zstrs.substr(p,32) << endl;
			p = rand()%(Nz-3000);
			pattern64 << zstrs.substr(p,64) << endl;
			p = rand()%(Nz-3000);
			pattern128 << zstrs.substr(p,128) << endl;
			p = rand()%(Nz-3000);
			pattern256 << zstrs.substr(p,256) << endl;
			p = rand()%(Nz-3000);
			pattern512 << zstrs.substr(p,512) << endl;
			p = rand()%(Nz-3000);
			pattern1024 << zstrs.substr(p,1024) << endl;
		}
	}
		
	
	return 0;
}

