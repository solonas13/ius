#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>

#include "defs.h"

using namespace std;

int read ( string filename, double *** text, int& n )
{
	ifstream input ( filename );
	if ( !input.good() ) {
		cout << "Error: Cannot open file " << filename << "\n";
		return 0;
	}
	int length;
	input >> length;
	n = length;
	string alpha;
	input >> alpha;

	double ** dna = new double * [n];
	for ( int i = 0; i < n; i++ ){
		dna[i] = new double [4];
		for ( int j = 0; j < 4; j++ ){
			input >> dna[i][j];
		}
	}
	(*text) = dna;
	
	return 1;
}
