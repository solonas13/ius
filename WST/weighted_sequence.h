/**
    Weighted Index
    Copyright (C) 2017 Carl Barton, Tomasz Kociumaka, Chang Liu, Solon P. Pissis and Jakub Radoszewski.
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
#pragma once

#include<string>
#include<vector>
#include<iostream>
#include "PST.h"


const double EPS = 1e-7;

class WeightedSequence {
    std::string alph;
    std::vector<std::vector<double>> P;
    PropertySuffixTree const* weighted_index;
	size_t len;
    
    friend std::istream & operator >> (std::istream& input, WeightedSequence &X);

public:
    WeightedSequence(): alph(), P(), weighted_index(NULL){}
    ~WeightedSequence() {
        if (weighted_index) delete weighted_index;
    }
    
    std::size_t length() const { return len; }
    std::size_t sigma() const { return alph.size(); }
    std::vector<double> const& probability(size_t i) const { return P[i]; }
    char symbol(size_t a) const { return alph[a]; }
    
    void build_index(double z, bool quiet, std::ostream& out);
    bool contains(std::string const& P) const;
    std::vector<int> occurrences(std::string const &P) const;
    

};

