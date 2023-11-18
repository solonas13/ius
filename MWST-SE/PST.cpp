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

#include <iostream>
#include <string>
#include <stack>
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <map>
#include "PST.h"
#include "heavy_string.h"


using namespace std;

PropertySuffixTree::stNode::stNode(PropertySuffixTree::position const &begin, PropertySuffixTree::position const &end): suf_link(NULL), children(), begin(begin), end(end) {}

/*
PropertySuffixTree::stNode::~stNode() {
    for (auto const &p : children) delete p.second;
	vector<int>().swap(labels);
	map<char, stNode*>().swap(children);
}
* */


PropertySuffixTree::stNode* PropertySuffixTree::stNode::add_leaf(PropertySuffixTree::position const &begin, PropertySuffixTree::position const &end) {
      if (begin == end) return this;
      stNode* leaf = new stNode(begin, end);
      children[*begin] = leaf;
      return leaf;
}

void PropertySuffixTree::stNode::split_edge(PropertySuffixTree::position const &split) {
      stNode* new_node = new stNode(split, end);
      end=split;
      new_node->children=children;
      new_node->minimizers=minimizers;
      children.clear();
      children[*split] = new_node;
      minimizers.clear();
}


//new build_suffix_tree
void PropertySuffixTree::build_suffix_tree(list<pair<size_t,size_t>>& min_substrings) {
    root = new stNode(text.end(), text.end()+1);
    pair<size_t,size_t> curr_string,next_string;
    int curr_depth,depth1;
    stNode *curr_node;
    int lcp;
    stack<stNode*> ancestors;
	curr_node=root;
	curr_string=make_pair(0,0); 
	curr_depth=-1; //root has a pseudo-parent at depth -1 which is never accessed
	while(min_substrings.size()>0){
		next_string=min_substrings.front();
		min_substrings.pop_front();
		lcp=text.LCP(curr_string,next_string);
		while(curr_depth>=lcp){		//moving up the tree to the node that will be the parent of the new leaf
			curr_node=ancestors.top();
			ancestors.pop();
			curr_depth -= curr_node->end - curr_node->begin;
		}
		if(lcp!=curr_depth+curr_node->end - curr_node->begin){ // making implict node explicit by spliting the edge
			curr_node->split_edge(curr_node->begin+lcp-curr_depth);
		}
		if(lcp==next_string.second-next_string.first){
			curr_node->minimizers.push_back(next_string.first);
			continue;
		}
		ancestors.push(curr_node);
		curr_node=curr_node->add_leaf(text.begin()+next_string.first+lcp,text.begin()+next_string.second);
		curr_node->minimizers.push_back(next_string.first);
		curr_depth=lcp;
		curr_string=next_string;
	}
}




PropertySuffixTree::PropertySuffixTree(HeavyString const& H, list<pair<size_t,size_t>> &min_substrings) {
	text = H;
	build_suffix_tree(min_substrings);
}





PropertySuffixTree::stNode* PropertySuffixTree::find(string const &P){
	   int m=P.length(),depth=0;
	   stNode* curr_node=root;
	   while(depth<m){
		   if(curr_node->children.find(P[depth])!=curr_node->children.end()){
			   curr_node=curr_node->children[P[depth]];
		   }else{//there is no child starting with the next letter
			   return NULL;
		   }
		   if(m-depth<=curr_node->end-curr_node->begin){//P should end at this node, or in the middle of the edge leading to it
			   if(P.substr(depth,m-depth)==text.substr(curr_node->begin-text.begin(),m-depth)){
				   return curr_node;
			   }else{//but it tried to exit the edge
				   return NULL;
			   }
		   }else if(P.substr(depth,curr_node->end-curr_node->begin)!=text.substr(curr_node->begin-text.begin(),curr_node->end-curr_node->begin)){
			   return NULL; //P tried to exit the edge which should have been read in full
		   }
		   depth+=curr_node->end-curr_node->begin;
	   }
		return curr_node; //this will be called only for an empty string P anyway
   }


void PropertySuffixTree::stNode::list_minimizers(vector<size_t>& l) const {
    l.insert(l.end(), minimizers.begin(), minimizers.end());
    for (auto const &p : children) p.second->list_minimizers(l);
}


vector<pair<int,double>> PropertySuffixTree::occurrences(string const& P){
	vector<pair<int,double>> res;	
	PropertySuffixTree::stNode* node = find(P);
	if(node!=NULL){
		vector<size_t> l;
		node->list_minimizers(l);
		for(vector<size_t>::iterator el=l.begin();el!=l.end();++el){
			res.push_back(make_pair((int)((*el)%text.n),text.substr_weight(*el,P.length())));
		}
	}
	return res;
}

//printout of the final structure (not used in the code)
void PropertySuffixTree::dfs() {
    stack<stNode*> s;
    s.push(root);
    while (!s.empty()) {
        stNode* curr = s.top();
        s.pop();
		
		if(curr != root){
			cout << text.substr(curr->begin - text.begin(),curr->end-curr->begin);
				for(list<size_t>::iterator minit=curr->minimizers.begin();minit!=curr->minimizers.end();++minit){
					cout <<"("<<*minit<<")";
				}
			
			cout <<endl;
		}
		
        for (map<char, stNode*>::reverse_iterator child=curr->children.rbegin();child!=curr->children.rend();++child) {
            s.push(child->second);
        }
    }
}
