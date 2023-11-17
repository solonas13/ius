/**
 *    Weighted Index
 *    Copyright (C) 2017 Carl Barton, Tomasz Kociumaka, Chang Liu, Solon P. Pissis and Jakub Radoszewski.
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <map>
#include <queue>
#include "estimation.h"

using namespace std;


struct Node;

struct TokenRequest {
    char const lead;
    Node* const source;
    TokenRequest(char lead, Node* source): lead(lead), source(source) {};
};

struct Token {
    Node* location;
    double odds;
    int depth;
    PropertyString &S;
    
    Token(double z, PropertyString &S): location(NULL), odds(z), depth(0), S(S) {};
    char move(char h);
};

struct Node {
    Node* parent;                   // parent node of this node
    double pp;                      // the probability of the character on the edge to the parent
    vector<Node*> children;         // all children nodes of this node
    int tokens;                     // number of processed tokens
    vector<TokenRequest> requests;  // token requests
    
    Node(): parent(NULL), pp(1), children(), tokens(0), requests() {};
    Node(Node* parent, double pp): parent(parent), pp(pp), children(), tokens(0), requests() {};
    
    void copy(Node* original, double p, char a);
    ~Node();
};



void Node::copy(Node* original, double p, char a) {
    int t = floor(p+EPS);
    for (auto c : original->children) {
        double pc = p*c->pp;
        int tc = floor(pc+EPS);
        t -= tc;
        if (tc > 0) {
            Node *newchild = new Node(this, c->pp);
            this->children.push_back(newchild);
            newchild->copy(c, pc, a);
        }
    }
    for (int i = 0; i < t; ++i) {
        original->requests.push_back(TokenRequest(a, this));
    }
}

Node::~Node() {
	parent = NULL;
	vector<Node*>().swap(children);
	vector<TokenRequest>().swap(requests);
}


char Token::move(char h) {
    ++depth;
    location->tokens--;
    while(true) {
        if (!location->requests.empty()) {
            auto req = location->requests.back();
            location->requests.pop_back();
            location = req.source;
            location->tokens++;
            return req.lead;
        } 
        int t = floor(odds+EPS);
        for (auto c : location->children) {
            t -= floor(odds*c->pp + EPS);
        }
        if (t > location->tokens) {
            location->tokens++;
            return h;
        }
        odds /= location->pp;
        --depth;
        location = location->parent;
    }
}

int heavy(vector<double> const& symbol) {
    int r = 0;
    for (size_t i = 0; i < symbol.size(); ++i) {
        if (symbol[i] > symbol[r]) r = i;
    }
    return r;
}

void deleteTree(Node *root){
	if(root == NULL)
		return;
	
	queue<Node*> q;
	
	q.push(root);
	while(!q.empty()){
		Node * node = q.front();
		q.pop();
		
		if(!node->children.empty()){
			for(auto c : node->children){
				q.push(c);
			}
		}
		
		delete node;
	}
}

Estimation::Estimation(std::vector<std::vector<double>> const& probability, std::string const& sigma, double z) {
    int n = probability.size();
    int flz = floor(z);
    S = vector<PropertyString>(flz, PropertyString(n));
    Node *root = new Node;
    vector<Token> tokens;
    for (auto & s : S) {
        tokens.emplace_back(Token(z, s));
    }
    for (auto &t : tokens) {
        t.location = root;
        root->tokens++;
    }
    for (int i = n-1; i >= 0; --i) {
        vector<double> const& symbol = probability[i];
        size_t h = heavy(symbol);
        Node *hroot = root;
        root = new Node;
        hroot->parent = root;
        hroot->pp = symbol[h];
        root->children.push_back(hroot);
        
        for (size_t a = 0; a < sigma.size(); ++a) {
            double p = symbol[a]*z;
            int t = floor(p+EPS);
            if (a != h && t > 0) {
                Node* aroot = new Node(root, symbol[a]);
                root->children.push_back(aroot);
                aroot->copy(hroot, p, a);
            }
        }
        for (Token &t : tokens) {
            t.odds *= symbol[h];
            int a = t.move(h);
            t.odds *= symbol[a]/symbol[h];
            t.S[i] = sigma[a];
            t.S.pi(i) = t.depth;
        }
    }
    deleteTree( root );
}


std::ostream& operator <<(std::ostream &out, Estimation const &E) {
    for (auto const &p : E.S) {
        out << p;
    }
    return out;
}
