#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_set>

#include "defs.h"

#include <divsufsort.h>
#include <sdsl/rmq_support.hpp>
#include <boost/functional/hash.hpp>


using namespace std;
using namespace sdsl;

void right_compacted_trie ( unordered_set<int > &anchors, int  * SA, int  * LCP, int  n, int  * RSA, int  * RLCP, int  g );
void left_compacted_trie ( unordered_set<int > &anchors, int * SA, int  * LCP, int  n, int  * RSA, int  * RLCP, int  g );
int  LCParray ( unsigned char * text, size_t n, int  * SA, int  * ISA, int  * LCP );
void extention ( vector<vector<double>>& text, string& s, string& alph, vector<int>& le, vector<int>& re, double z );
void union_find_resort( int * SA, int * LCP, vector<int>& property, int n );



