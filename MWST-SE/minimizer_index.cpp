#include <cmath>
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <list>
#include <chrono>
#include <ctime>
#include <functional> 
#include "minimizer_index.h"
#include "minimizers.h"
#include "krfp.h"
#include "heavy_string.h"
#include "PST.h"

using namespace std;
using get_time = chrono::steady_clock;

bool isEqual(double a, double b) {
	double epsilon = 1e-14;
    return std::abs(a - b) <= epsilon * std::max(std::abs(a), std::abs(b));
}


std::istream & operator >> (std::istream& input, MinimizerIndex &M) {
    input >> M.N;
    input >> M.alph;
    int A = M.alph.size();
	M.pi_prefix = vector<double> (M.N,1);
    for (int i = 0; i < M.N; ++i) {
        double sum = 0;
        vector<double> symbol(A, 0);
        for (int j = 0; j < A; ++j) {
            input >> symbol[j];
            sum += symbol[j];
        }
 		int which_max = max_element(symbol.begin(), symbol.end()) - symbol.begin();
		M.H+=(M.alph[which_max]);
		double pi = symbol[which_max];
		if(i == 0){
			M.pi_prefix[i] = log2(pi);
		}else{
			M.pi_prefix[i] = M.pi_prefix[i-1] + log2(pi);
		}       
        
        if (!isEqual(sum,1)) {
            cerr << "Probabilities at position " << i << " do not sum up to 1" << std::endl;
            throw 1;
        }
        M.fP.emplace_back(symbol);
    }
 	return input;
}

int MinimizerIndex::pruning(int first_pos, double p1){ // p1= - log p - log z // finding the longest solid string starting in a minimizer (the part on the heavy path) using binary search
	//pruning is not needed for the structure (if the log z comparison was implemented), but makes the linear comparison (which is faster in practice) of strings faster
	int n=H.size();
	double base;
	if(first_pos==0){
		base=p1;
	}else{
		base=p1+pi_prefix[first_pos-1];
	}
	int beg_pos=first_pos-1,window=1,end_pos; //beg_pos - the furthest position about which we know that it belongs to the solid factor
	while((beg_pos+window<n)&&(pi_prefix[beg_pos+window]>=base)){
		beg_pos+=window;
		window*=2;
	}
	if(beg_pos+window>=n){
		end_pos=n;
	}else{
		end_pos=beg_pos+window;
	}//now we know that beg_pos belongs to the factor and that end_pos does not
	while(beg_pos<end_pos-1){
		if(pi_prefix[(beg_pos+end_pos)/2]-pi_prefix[first_pos]>=p1){
			beg_pos=(beg_pos+end_pos)/2;
		}else{
			end_pos=(beg_pos+end_pos)/2;
		}
	}
	return beg_pos;
}



void MinimizerIndex::build_index(double z, int l){
	int k = ceil(4*log2(l) / log2(alph.size()));
	int n = fP.size();
	list<pair<size_t,size_t>> minimizer_substrings;
	size_t minimizer_count=0;
	
	MinimizerHeap heap(n,l,k);
	
	for(int i = 0; i < alph.size(); i++){
		amap[alph[i]] = i;
	}
	
	//vector<char> S; // We use the copy of S stored in the minimizer heap
	double p = 1.0;
	int a = n-1;
	unordered_set<int> minimizers;
	list<pair<int, char>> diff;
	list<list<pair<int, char>>> global_diff;
	int sig1 = -1;
	int pos1 = n;
	while( a != n ){
		int sig = sig1 + 1;
		if( a >= 0 && sig != alph.size() ){
			if( p != 1 || alph[sig] != H[a]){
				if( p * fP[a][sig] * z < 1 ){
					sig1 = sig;
					continue;
				}else{
					p *= fP[a][sig];
				}
			}else{
				pos1 = a;
			}
			heap.left(alph[sig]);
			if(H[a] != alph[sig]){
				diff.push_front(make_pair(a, alph[sig]));
			}
			if(heap.S.size() >= l){
				double pi_cum = 1;
				if( pos1 <= 0 ){	
					pi_cum = pow(2,pi_prefix[l]);	//if full length S is the heavy string and reach the beginning, directly use pi prefix
				}else{
					pi_cum = p * pow(2, pi_prefix[a+l-1] - pi_prefix[pos1-1]);
				}
				if(pi_cum * z >= 1){
					minimizers.insert(heap.top());
				}
			}
			a = a - 1;
			sig1 = -1;
		}else{
			a = a + 1;//a is set to the position of the letter that is being removed
			if(minimizers.find(a) != minimizers.end()){
				minimizers.erase(a);
				//finding the longest string starting at this minimizer with weight >=1/z
				int pos3=pruning(pos1,-log2(p)-log2(z))+1;
				minimizer_substrings.push_back(make_pair(minimizer_count+(size_t)a,minimizer_count+(size_t)pos3));
				global_diff.push_back(diff);
				minimizer_count+=(size_t)n;
			}
			// removing the first letter and restoring variables
			if ( !diff.empty()){
				if(diff.front().first == a) {
					diff.pop_front();
				}
			}
			if(!isEqual(p,1)){
				p /= fP[a][amap[heap.S[heap.S.size()-1]]];
				if(p>0.7) p=1.0; //fixing p in case of precision errors (p cannot be between 0.5 and 1, hence p>0.7 means p=1)
			}else{
				pos1=a+1;
			}
			if(heap.S.size() > 0){
				sig1 = amap[heap.S[heap.S.size()-1]];
				heap.right();
			}
		}
	}
	//here the output of Lemma 13 = input of Theorem 14 is already computed
	
	//encoding minimizer solid factors as factor of heavy string plus up to log z differences
	HeavyString text=HeavyString(fP,H,alph,minimizer_substrings,global_diff,pi_prefix);

	//sorting minimizer solid factors
	minimizer_substrings.sort(HeavyString::Heavycompare(&text));

	// construction of the index
	forward_index = new PropertySuffixTree(text, minimizer_substrings);
	
}



double MinimizerIndex::naive_check(string P,int start_pos){
	if(start_pos<0) return 0;
	double res=1;
	for(int i=0;i<P.length();++i){
		res*=fP[start_pos+i][amap[P[i]]];
	}
	return res;
}

vector<int> MinimizerIndex::occurrences(string const &P, int ell, double z, ostream& result){
	int m = P.size();
	int k = ceil(4*log2(ell) / log2(alph.size()));
	string pattern = P;
	int min_index = pattern_minimizers(pattern,k);
	vector<pair<int,double>> candidates = forward_index->occurrences(P.substr(min_index));
	std::set<int> occs;
	for(int i=0;i<candidates.size();++i){
		double first_part_weight=naive_check(P.substr(0,min_index),candidates[i].first-min_index);
		if(first_part_weight*pow(2,candidates[i].second)*z>=1){
			//if(candidates[i].first!=N-pattern.length()){//TODO this only used to match the minor issue of the other programs
				occs.insert(candidates[i].first-min_index);
			//}
		}
	}
	std::vector<int> final_occs(occs.begin(), occs.end());
	return final_occs;
}
