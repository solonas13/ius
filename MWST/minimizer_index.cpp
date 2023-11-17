#include <cmath>
#include <chrono>
#include <list>
#include <cstdlib>
#include <ctime>
#include <set>
#include <deque>
#include <unordered_map>
#include <sys/resource.h>
#include <algorithm>
#include <unordered_map>

#include "estimation.h"
#include "krfp.h"
#include "minimizers.h"
#include "minimizer_index.h"

using std::endl;
using std::cerr;
using get_time = std::chrono::steady_clock;

bool sort_sa(const pair<int,int> &a,const pair<int,int> &b)
{
       return a.first<b.first;
}

std::istream & operator >> (std::istream& input, MinimizerIndex &M) {
    input >> M.N;
    input >> M.alph;
    int A = M.alph.size();
    for (int i = 0; i < M.N; ++i) {
        double sum = 0;
        std::vector<double> symbol(A, 0);
        for (int j = 0; j < A; ++j) {
            input >> symbol[j];
            sum += symbol[j];
        }
        if (std::abs(sum-1) > EPS) {
            std::cerr << "Probabilities at position " << i << " do not sum up to 1" << std::endl;
            throw 1;
        }
        M.fP.emplace_back(symbol);
    }
 	return input;
}

void extention ( vector<vector<double>>& text, string& s, string& alph, vector<int>& le, vector<int>& re, double z ){
	int n = text.size();
	int N = s.size();
	unordered_map<char, int> A;
	for(int i = 0; i < alph.size(); i++){
		A[alph[i]] = i;
	}
	
	vector<double> _pi;
	for(int i = 0; i < N; i++){
		_pi.push_back( text[i%n][A[s[i]]]);
	}	
	
	int l = 0;
	double cum_pi = 1;
	
	re.assign(N,0);	
	for (auto j = 0; j < z; j++ ){
		for(auto i = 0; i < n; i++){
			int si = i + j*n;
			if( i == 0 || re[si-1] < 1 ){
				l = 0;
				cum_pi = 1;
				for( l = 0; l < n-i-1; l++){
					if(cum_pi * _pi[si+l] * z >= 1){
						cum_pi *= _pi[si+l];
					}else{
						break;
					}
				}
				re[si] = l-1;
			}else{
				l = re[si-1];
				cum_pi /= _pi[si-1];
				if ( cum_pi * z < 1 ){
					re[si] = l -1;
				}else{
					for ( l = l; l < n-i-1; l++ ){
						if(cum_pi * _pi[si+l] * z >= 1){
							cum_pi *= _pi[si+l];
						}else{
							break;
						}
					}
					re[si] = l-1;
				}
			}
		}
	}
	
	le.assign(N,0);
	reverse(_pi.begin(), _pi.end());
	for (auto j = 0; j < z; j++ ){
		for(auto i = 0; i < n; i++){
			int si = i + j*n;
			if( i == 0 || le[si-1] < 1 ){
				l = 0;
				cum_pi = 1;
				for( l = 0; l < n-i; l++){
					if(cum_pi * _pi[si+l] * z >= 1){
						cum_pi *= _pi[si+l];
					}else{
						break;
					}
				}
				le[si] = l-1;
			}else{
				l = le[si-1];
				cum_pi /= _pi[si-1];
				if ( cum_pi < 1/z )
					le[si] = l -1;
				else
				{
					for ( l = l; l < n-i; l++ )
					{
						if(cum_pi * _pi[si+l] * z >= 1){
							cum_pi *= _pi[si+l];
						}else{
							break;
						}
					}
					le[si] = l-1;
				}
			}
		}	
	}
	reverse(le.begin(), le.end());
}

void MinimizerIndex::build_index(double z, int ell){
	vector<vector<double>> rP(fP.rbegin(), fP.rend());
	Estimation fS(fP,alph,z);
	PropertyString fT;
	std::vector<int> f_mini_pos;
	std::vector<int> r_mini_pos;
	int i = 0;
	int k = ceil(4 * log2(ell) / log2(alph.size()));
	int w = ell - k + 1;
	
	for(PropertyString const & s : fS.strings()){
		fT += s;
		std::unordered_set<uint64_t> M;
		string temp_s = s.string();
		compute_minimizers(temp_s, w, k, M);
		for(auto it : M){
			f_mini_pos.emplace_back(it + i*N);
		}
		i++;
	}	
	string zstrs = fT.string();
	string rev_zstrs(zstrs.rbegin(), zstrs.rend());
	vector<int> le;
	vector<int> re;

	extention(fP, zstrs, alph, le, re, z);
	vector<int> le_r(le.rbegin(), le.rend());
	vector<int> re_r(re.rbegin(), re.rend());
	
	f_mini_pos.erase(remove_if(f_mini_pos.begin(), f_mini_pos.end(), [&](int i) { return re[i] < k; }), f_mini_pos.end());

	for(auto i : f_mini_pos){
		r_mini_pos.push_back(zstrs.size() - i);
	}
	HeavyString fH(fP, zstrs, alph, f_mini_pos, le, re, true);
	HeavyString rH(rP, rev_zstrs, alph, r_mini_pos, le_r, re_r, false);
	Nz = zstrs.size();

	forward_index = new PropertySuffixTree(re, fH,f_mini_pos);
	
	reverse_index = new PropertySuffixTree(le_r, rH, r_mini_pos);
		
	fS.clear();
	fT.clear();	
	vector<vector<double>>().swap(fP);
}

std::vector<int> MinimizerIndex::occurrences(std::string const &P, int ell, double z, std::ostream& result) const{
	int m = P.size();
	int k = ceil(4 * log2(ell) / log2(alph.size()));
	string pattern = P;
	int min_index = pattern_minimizers(pattern,k);
	bool flag = false;
	
	std::set<int> occs;
	int l = min_index;
	int r = m - min_index;
	if(min_index <= ceil(m/2) ){
		std::string min_suf = P.substr(min_index);

		for(int o : forward_index->occurrences(min_suf)){
			int begin = o - l;
			if(begin < 0) continue;
			if(begin + m >= Nz) continue;
			if(occs.count(begin%N)) continue;
			double rpi = forward_index->get_pi(o, o, r);
			double lpi = forward_index->naive_check(P, 0, begin, l, o);
			if( rpi * lpi *z >=1 ){
				occs.insert( begin%N );
			}
		}
	}else{
		std::string rP = P;
		std::reverse(rP.begin(), rP.end());
		int r_index = m - min_index;
		std::string min_suf = rP.substr(r_index);
		for(int o : reverse_index->occurrences(min_suf)){
			int begin = Nz - ( o + min_suf.size() - 1 ) - 1;
			if(begin < 0) continue;
			if(begin + m >= Nz) continue;
			if(occs.count(begin%N)) continue;
			int c = Nz - o;
			double lpi = forward_index->get_pi(c,begin,l);
			double rpi = forward_index->naive_check(P, l, begin+l, r, c);	
			if( rpi * lpi *z >=1 ){
				occs.insert( begin%N );
			}
		}
	}
	std::vector<int> final_occs(occs.begin(), occs.end());
	return final_occs;
}

bool  MinimizerIndex::is_valid(string const& p, int pos, double z) const{
		unordered_map<char, int> mapping;
		pos = pos%N;
		if(pos + p.size() > N) return false;
		for(int i = 0; i < alph.size(); i++){
			mapping[alph[i]] = i;
		}
		double cum_prob = 1;
		for(int i = 0; i < p.size(); i++){
			cum_prob *= fP[pos+i][mapping[p[i]]];
		}
		return (cum_prob*z >= 1)?true:false;
}