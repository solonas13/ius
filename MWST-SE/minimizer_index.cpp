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
    //cout<< M.alph<<endl;
    //cout << M.H<<endl;
 	return input;
}


int MinimizerIndex::find_minimzer_index (string& text,int k){
	int n = text.length();
	int fp = 0;
	int smallest_fp = fp;
	
	int * FP = ( int * ) malloc( ( n - k + 1  ) *  sizeof( int ) );
	
	for(int j = 0; j<k; j++)
		fp =  karp_rabin_hashing::concat( fp, text[j] , 1 );
		
	FP[0] = fp;
	int pos = 1;
	//cout<<fp<<endl;
	int min_fp = fp;
	int minimizers = 0;
	
	// find all fps for all k substrings
	for(int j = 1; j<=n-k; j++)
	{
		fp = karp_rabin_hashing::concat( fp, text[j+k-1] , 1 );
		fp = karp_rabin_hashing::subtract( fp, text[j-1] , k );
		
		//cout<<fp<<endl;
		FP[pos] = fp;
		pos++;
	}	
	
		
	// minimum fp in first window
   	for (int j = 0; j <= n - k ; j++) 
   	{
		if(FP[j] < min_fp){
			min_fp = FP[j];
			minimizers = j;
		}
 	}
	
	free( FP );
	return minimizers;
}

int MinimizerIndex::pruning(int first_pos, double p1){ // p1= - log p - log z // finding the longest solid string starting in a minimizer (the part on the heavy path) using binary search
	//cout<<"p1: "<<p1<<endl;
	int n=H.size();
	double base;
	if(first_pos==0){
		base=p1;
	}else{
		base=p1+pi_prefix[first_pos-1];
	}
	//cout<<"base: "<<base<<endl;
	//if pi_prefix[x]>=base then x belongs to the solid factor starting in the minimizer
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
	//auto begin = get_time::now();
	int k = ceil(4*log2(l) / log2(alph.size()));
	int n = fP.size();
	list<pair<size_t,size_t>> minimizer_substrings;
	size_t minimizer_count=0;
	
	MinimizerHeap heap(n,l,k);
	
	//cout<< pi_prefix[n-1]<<endl;
	for(int i = 0; i < alph.size(); i++){
		amap[alph[i]] = i;
	}
	
	//string S;
	vector<char> S;
	double p = 1.0;
	int a = n-1;
	unordered_set<int> minimizers;
	list<pair<int, char>> diff;
	list<list<pair<int, char>>> global_diff;
	int sig1 = -1;
	int pos1 = n;
	//cout << "Heavy string: " << H << endl;
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
			//S.insert(0, 1, alph[sig]);
			S.push_back(alph[sig]);
			heap.left(alph[sig]);
			if(H[a] != alph[sig]){
				diff.push_front(make_pair(a, alph[sig]));
			}
			if(S.size() >= l){
				double pi_cum = 1;
				if( pos1 <= 0 ){	
					pi_cum = pow(2,pi_prefix[l]);	//if full length S is the heavy string and reach the beginning, directly use pi prefix
				}else{
					pi_cum = p * pow(2, pi_prefix[a+l-1] - pi_prefix[pos1-1]);
				}
				if(pi_cum * z >= 1){
					//string prefixS = S.substr(0,l);
					minimizers.insert(heap.top());
					//minimizers.insert(a+ pattern_minimizers(prefixS, k));
					//if(heap.top()!=a+ pattern_minimizers(prefixS, k)){
					//	cout << heap.top() <<" "<< a+ pattern_minimizers(prefixS, k)<<endl;
					//}
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
				p /= fP[a][amap[S[S.size()-1]]];
				if(p>0.7) p=1.0; //fixing p in case of precision errors (p cannot be between 0.5 and 1, hence p>0.7 means p=1)
			}else{
				pos1=a+1;
			}
			if(S.size() > 0){
				sig1 = amap[S[S.size()-1]];
				//S = S.substr(1);
				S.pop_back();
				heap.right();
			}
		}
	}
	//cout<< "the set of minimizer solid factors computed"<<endl;
	
	//cout << minimizer_strings_before_pruning.size() << " minimizers"<<endl;
	//for(list<pair<int,list<pair<int, char>>>>::iterator minit = minimizer_strings_before_pruning.begin(); minit!=minimizer_strings_before_pruning.end();++minit){
		//cout << minit->first << ":";
		//for(list<pair<int,char>>::iterator el = minit->second.begin(); el!=minit->second.end();++el){
		//	cout << "("<< el->first << "," << el->second<< ")";
		//}
		//cout << endl;
	//}
	
	
	/*
	//list of minimizer nodes with diffrences lists
	cout << minimizer_substrings.size() << " minimizers"<<endl;
	list<list<pair<int,char>>>::iterator diffit=global_diff.begin();
	for(list<pair<size_t,size_t>>::iterator minit = minimizer_substrings.begin(); minit!=minimizer_substrings.end();++minit){
		cout << minit->first <<".."<<minit->second<< ":";
		for(list<pair<int,char>>::iterator el = diffit->begin(); el!=diffit->end();++el){
			cout << "("<< el->first << "," << el->second<< ")";
		}
		cout << endl;
		diffit++;
	}
	*/
	//cout << "starting to build the heavy tree"<<endl;
	//auto end = get_time::now();
	//auto diff2 = end - begin;
	//cout << "Lemma 13 time "<< chrono::duration_cast<chrono::milliseconds>(diff2).count()<<endl;
	//begin = get_time::now();
	
	HeavyString text=HeavyString(fP,H,alph,minimizer_substrings,global_diff,pi_prefix);
	global_diff.clear();
	//end = get_time::now();
	//diff2 = end - begin;
	//cout << "building heavy string time "<< chrono::duration_cast<chrono::milliseconds>(diff2).count()<<endl;
	//at this point minimizer_substrings stores the beginnings and endings of the strings that should be included in the reversed trie, while text stores their contents.
	
	//cout<<"heavy string constructed"<<endl;
	//begin = get_time::now();
	minimizer_substrings.sort(HeavyString::Heavycompare(&text));
	//end = get_time::now();
	//diff2 = end - begin;
	//cout << "sorting time "<< chrono::duration_cast<chrono::milliseconds>(diff2).count()<<endl;
	//at this point the minimizer_substrings are sorted alphabetically.
		
	
	
	/*
	// sorted minimizer strings and lcp between next two such strings
	list<pair<size_t,size_t>>::iterator iter2=minimizer_substrings.begin();
	++iter2;
	for(list<pair<size_t,size_t>>::iterator iter1=minimizer_substrings.begin();iter1!=minimizer_substrings.end();++iter1){
		cout<< iter1->first<< ".."<<iter1->second<<" len="<< iter1->second-iter1->first+1<<endl;
		cout << text.substr(iter1->first,iter1->second-iter1->first+1)<< " "<<iter1->first<<endl;
		 if(iter2!=minimizer_substrings.end()){
			cout<< text.LCP(*iter1,*iter2)<<endl;
			++iter2;
		}
	}
	*/
	
	//cout<< "start building tree"<<endl;
	//begin = get_time::now();
	forward_index = new PropertySuffixTree(text, minimizer_substrings);
	//end = get_time::now();
	//diff2 = end - begin;
	//cout << "buiding time "<< chrono::duration_cast<chrono::milliseconds>(diff2).count()<<endl;
	//cout<<"finish building tree"<<endl;
	//forward_index->dfs();
	
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
	//cout << "pattern "<<P<<" split: " <<P.substr(0,min_index)<<"|"<<P.substr(min_index)<<endl;
	vector<pair<int,double>> candidates = forward_index->occurrences(P.substr(min_index));
	//cout<< candidates.size()<<" candidates:"<<endl;
	std::set<int> occs;
	for(int i=0;i<candidates.size();++i){
		//cout<< candidates[i].first-min_index<<":";
		double first_part_weight=naive_check(P.substr(0,min_index),candidates[i].first-min_index);
		//cout<< "first part: " <<first_part_weight<<" second part "<< pow(2,candidates[i].second)<<endl;
		if(first_part_weight*pow(2,candidates[i].second)*z>=1){
			//if(candidates[i].first!=N-pattern.length()){//TODO this only to match the error in other programs
				occs.insert(candidates[i].first-min_index);
			//}
		}
	}
	std::vector<int> final_occs(occs.begin(), occs.end());
	return final_occs;
}
