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
#include <cmath>
#include <iostream>

#include "defs.h"
#include "node.h"

using namespace std;

extern double ** text;
extern vector <int> heavy_string;
string alpha = "ACGT";

void Node::Copy ( Node * x )
{
	this->coreNode = x;
	this->odds = x->odds;
	this->edge = x->edge;
	this->pos = x->pos;
	for ( int i = 0; i < (int) x->child.size(); i++ )
	{
		Node * child = new Node;
		child->Copy ( x->child[i] );
		child->parent = this;
		this->child.push_back( child );
	}
}

void Node::Update( double p, double z )
{
	int parent_token = floor ( this->odds * z );
	int child_token = 0;
	int num_token;
	for ( int i = 0; i < this->child.size(); i++ )
	{
		this->child[i]->odds *= p;
		if ( this->child[i]->odds < 1/z )
		{
			double tempOdds = this->odds;
			if ( tempOdds * text[this->pos][this->child[i]->edge] < 1/z )
			{
				this->child[i]->deleteNode();
				this->child.erase( this->child.begin()+i );
				i--;
			}
			else
			{
				tempOdds *= text[this->pos][this->child[i]->edge];
			
			for ( int k = this->pos+1; k < this->child[i]->pos; k++ )
			{
				double tempP = tempOdds * text[k][heavy_string[k]];
				if ( tempP < 1/z )
				{
					this->child[i]->pos  = k;
					this->child[i]->odds = tempOdds;
					for ( int ii = 0; ii < this->child[i]->child.size(); ii++ )
					{
						this->child[i]->child[ii]->deleteNode();
					}
					this->child[i]->child.clear();
					break;
				}
				tempOdds = tempP;
			}
			}
		}
		else
		{
			child_token += floor ( z * this->child[i]->odds );
		}
	}
	num_token = parent_token - child_token;
	if ( num_token )
		this->token.resize( num_token );
	for ( unsigned int i = 0; i < this->child.size(); i++ )
	{
		this->child[i]->Update( p, z );
	}
}

int Node::subToken()
{
	int token = -1;
	for ( unsigned int i = 0; i < this->child.size(); i++ )
	{
		if ( this->child[i]->token.size() )
		{
			token = this->child[i]->token[0];
			this->child[i]->token.erase ( this->child[i]->token.begin() );
			return token;
		}
		else
		{
			token = this->child[i]->subToken();
			if ( token != -1 )
				return token;
		}
	}
	return token;
}

void Node::Compact( int e )
{
	if ( this->child.size() == 1 && this->parent )
	{
			this->child[0]->edge = this->edge;
			this->child[0]->parent = this->parent;
			this->parent->child[e] = this->child[0];
			this->parent->child[e]->Compact ( e );
			delete this;
	}
	else
	{
		for ( unsigned int i = 0; i < this->child.size(); i++ )
			this->child[i]->Compact( i );
	}
}

void Node::deleteNode()
{
	if ( this->child.size() == 0 )
	{
		delete this;
	}
	else
	{
		while ( this->child.size() )
		{
			this->child.back()->deleteNode();
			this->child.pop_back();
		}
		delete this;
	}
}
