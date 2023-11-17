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
#include <queue>
#include "PST.h"

using namespace std;

PropertySuffixTree::stNode::stNode(PropertySuffixTree::position const &begin, PropertySuffixTree::position const &end): suf_link(NULL), children(), begin(begin), end(end) {}

PropertySuffixTree::stNode::~stNode() {
    for (auto const &p : children) delete p.second;
}

PropertySuffixTree::stNode* PropertySuffixTree::stNode::add_leaf(PropertySuffixTree::position const &begin, PropertySuffixTree::position const &end) {
      if (begin == end) return this;
      stNode* leaf = new stNode(begin, end);
      children[*begin] = leaf;
      return leaf;
}

void PropertySuffixTree::stNode::list(vector<int>& l) const {
    l.insert(l.end(), labels.begin(), labels.end());
    for (auto const &p : children) p.second->list(l);
}


void PropertySuffixTree::stNode::print(ostream &out, int d) const {
    for (int i = 0; i < d; ++i) out << " ";
    if (d >= 0) for (auto it = begin; it != end; ++it) out << *it;
    d += end-begin;
    for (auto const &l : labels) out << " " << l;
    out << endl;
    for (auto const &ch : children) ch.second->print(out, d);
}

PropertySuffixTree::stNode* PropertySuffixTree::stNode::trim(bool is_root) {
    if (is_root) {
        suf_link->children.clear();
        delete suf_link;
    }
    suf_link = NULL;
    for (auto it = children.begin(); it != children.end(); ) {
        it->second = it->second->trim();
        if (it->second == NULL) {
            it = children.erase(it);
        } else ++it;
    }
    if (!labels.empty() || children.size() > 1 || is_root) return this;
    else if (children.size() == 1) {
        stNode* son = children.begin()->second;
        son->begin -= (end-begin);
        children.clear();
        delete this;
        return son;
    } else {
        delete this;
        return NULL;
    }
}


PropertySuffixTree::stNode * PropertySuffixTree::Locus::descendant() const{
    if (is_explicit()) return node;
    else return node->children[*begin];
}

void PropertySuffixTree::Locus::fast_forward(){
    while(!is_explicit()) {
        stNode* w = node->children[*begin];
        if (w->end - w->begin <= end - begin) {
            begin += (w->end - w->begin);
            node = w;
        } else break;
    } 
}


PropertySuffixTree::stNode* PropertySuffixTree::Locus::make_explicit() {
    fast_forward();
    if (is_explicit()) return node;
    stNode *x = node->children[*begin];
    PropertySuffixTree::position mid = x->begin + (end - begin);
    stNode* midNode = new stNode(x->begin, mid);
    node->children[*begin] = midNode;
    midNode->children[*mid] = x;
    x->begin = mid;
    return midNode;
}


void PropertySuffixTree::Locus::suf_link() {
    node = node->suf_link;
}

void PropertySuffixTree::Locus::forward(PropertySuffixTree::position limit) {
    while (end != limit) {
        if (is_explicit()) {
            if (node->children.find(*begin) != node->children.end()) ++end;
            else return;
        } else {
            stNode* x = node->children[*begin];
            PropertySuffixTree::position it = x->begin + (end - begin);
            while(it != x->end && end != limit) {
                if (*end != *it) return;
                ++it;
                ++end;
            }
            if (it == x->end) {
                node = x;
                begin = end;
            }
        }
    }
}

bool PropertySuffixTree::Locus::is_explicit() const {
    return begin == end;
}

bool PropertySuffixTree::Locus::operator < (PropertySuffixTree::Locus const &other) const {
    return end-begin > other.end - other.begin;
}


void PropertySuffixTree::build_suffix_tree() {
    root = new stNode(text.end()-1, text.end());
    stNode* top = new stNode(text.begin(), text.begin());
    root->suf_link = top;
    for (char c : text) top->children[c] = root;
    
    Locus cur(root, text.begin(), text.begin());
    stNode* prev = NULL, *prev_leaf = NULL;

    for (size_t i = 0; i < text.length(); ++i) {
        cur.forward(text.end());
        stNode* w = cur.make_explicit();
        stNode* leaf = w->add_leaf(cur.end, text.end());
        
        if (prev != NULL) prev->suf_link = w;
        if (prev_leaf != NULL) prev_leaf->suf_link = leaf;
        prev_leaf = leaf;
        
        cur.suf_link();
        cur.fast_forward();
        if (cur.is_explicit()) {
            w->suf_link = cur.node;
            prev = NULL;
        } else {
            prev = w;
        }    
    }
}

void PropertySuffixTree::process_property(const vector<int>& pi) {
    vector<pair<Locus, int>> requests;
    Locus cur = Locus(root, text.begin(), text.begin());
    for (size_t i = 0; i < text.length(); ++i) {
        cur.end = text.begin() + i + pi[i];
        cur.fast_forward();
        requests.emplace_back(make_pair(cur, i));
        cur.node = cur.node->suf_link;
    }
    sort(requests.begin(), requests.end());
    for (auto const &r : requests) {
        auto loc = r.first;
        int label = r.second;
        loc.make_explicit();
        loc.fast_forward();
        loc.node->labels.push_back(label);
    }
    root->trim(true);
}



PropertySuffixTree::PropertySuffixTree(PropertyString const& S): text(S.string()) {
    build_suffix_tree();
    process_property(S.property());
}

PropertySuffixTree::Locus PropertySuffixTree::find(string const &P) const {
    PropertySuffixTree::Locus cur(root, P.begin(), P.begin());
    cur.forward(P.end());
    return cur;
}

bool PropertySuffixTree::contains(string const &P) const {
    return (find(P).end == P.end());
}

vector<int> PropertySuffixTree::occurrences(string const& P) const {
    PropertySuffixTree::Locus l = find(P);
    vector<int> res;
    if (l.end != P.end()) return res;
    stNode* top = l.descendant();
    top->list(res);
    return res;
}


PropertySuffixTree::~PropertySuffixTree() {
    delete root->suf_link;
    delete root;
}


std::ostream& operator<< (std::ostream &out, PropertySuffixTree const &st) {
    st.root->print(out, -1);
    return out;
}
