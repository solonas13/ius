#ifndef MST_SET_H
#define MST_SET_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <list>
#include "heavy_string.h"
#include "PST.h"


using namespace std;

class MinimizerIndex{
	map<char, int> amap;
	int N;
	vector<vector<double>> fP;
	
	string H;
	vector<double> pi_prefix;
	
	friend std::istream & operator >> (std::istream& input, MinimizerIndex &M);
	int pruning(int first_pos,double p1);
	PropertySuffixTree * forward_index;
	double naive_check(string P, int start_pos);
	
public:
	string alph;
	void build_index(double z,int ell);

	vector<int> occurrences(string const &P, int ell, double z, ostream& result);
};

#endif //MST_SET_H  
