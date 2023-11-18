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
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <map>
#include "property_string.h"
#include "heavy_string.h"

using namespace std;

class PropertySuffixTree {
    typedef HeavyString::const_iterator position;
    struct stNode {
        stNode* suf_link;
        map<char, stNode*> children;
        position begin, end;
        list<size_t> minimizers;
        
       stNode(position const &begin, position const &end);
        
        stNode* add_leaf(position const &begin, position const &end);
        void split_edge(PropertySuffixTree::position const &split);
        void list_minimizers(vector<size_t>& l) const;
    };
            
    stNode* root;
    
    void build_suffix_tree(list<pair<size_t,size_t>> &min_substrings);
    friend std::ostream& operator<< (std::ostream &out, PropertySuffixTree const &st);
    stNode* find(string const &P);
    
public:
    HeavyString text;

   PropertySuffixTree(HeavyString const& H, list<pair<size_t,size_t>> &min_substrings);
   
    vector<pair<int,double>> occurrences(string const &s);
	double naive_check(string const & pat, int p_begin, int t_begin, int length, int c);
	void dfs();
    ~PropertySuffixTree();
};
