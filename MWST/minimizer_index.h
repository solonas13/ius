#pragma once

#include<string>
#include<vector>
#include<iostream>
#include "PST.h"

class MinimizerIndex {
	std::string alph;
	int N;
	int Nz;
	std::vector<std::vector<double>> fP;

	PropertySuffixTree * forward_index;
	PropertySuffixTree * reverse_index;

	friend std::istream & operator >> (std::istream& input, MinimizerIndex &M);
	bool isValid(vector<int> const &iP, int begin, double z) const;
	bool is_valid(string const& p, int pos, double z) const;

public:
	MinimizerIndex(): alph(), fP(), forward_index(NULL), reverse_index(NULL){}
	~MinimizerIndex(){
		if(forward_index) delete forward_index;
		if(reverse_index) delete reverse_index;
	}
	void build_index(double z, int ell);
	std::vector<int> occurrences(std::string const &P, int ell, double z, ostream& result) const;
};
