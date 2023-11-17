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
#include "property_string.h"

using namespace std;


class PropertySuffixTree {
    typedef string::const_iterator position;
    struct stNode {
        stNode* suf_link;
        unordered_map<char, stNode*> children;
        position begin, end; 
        vector<int> labels;
        
        stNode(position const &begin, position const &end);
        ~stNode();
        
        stNode* add_leaf(position const &begin, position const &end);
        
        void print(ostream &out, int d) const;
        
        stNode* trim(bool is_root = false);
        void list(vector<int> &l) const;
		
		void delete_tree(stNode* r);
    };
    
    struct Locus {
        stNode* node;
        position begin, end;
        
        stNode* descendant() const;
        bool is_explicit() const;
        void fast_forward();
        void suf_link();
        void forward(position limit);
        stNode* make_explicit();
        
        bool operator < (Locus const &other) const;
        
        Locus() {}
        Locus(stNode* node, position const &begin, position const &end) : node(node), begin(begin), end(end) {}
    };
    
        
    stNode* root;
    string const text;
    Locus find(string const &P) const;
    
    void build_suffix_tree();
    void process_property(vector<int> const& pi);
    friend std::ostream& operator<< (std::ostream &out, PropertySuffixTree const &st);
    
public:
    PropertySuffixTree(PropertyString const& S);
    
    bool contains(string const &P) const;
    vector<int> occurrences(string const &P) const;
    ~PropertySuffixTree();
};
