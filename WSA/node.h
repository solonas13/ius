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

#include <vector>
#include <string>
#include <fstream>

using namespace std;

#ifndef NODE_H
#define NODE_H

struct Node
{
		Node * parent;				//parent node of this node
		Node * coreNode;			//corresponding node of this node
		int pos;
		vector < Node* > child;		//all children nodes of this node
		int edge = 0;				//the edge between this node and its parent
		double odds;				//the probability of this node
		vector < int > token;		//the token given to this node
		void Copy( Node * x );		//deep copy of a node
		void Update( double p, double z );	//update this node when add a new letter in the tree
		int subToken();				//get all tokens in the subtree of this node
		void Compact( int e );
		void deleteNode();			//destructor
};

void weighted_index_building( double ** text, int n, double z, string * sq_return );

#endif


