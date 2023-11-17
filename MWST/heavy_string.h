#ifndef HEAVY_STRING_H
#define HEAVY_STRING_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <cstddef>
#include <cmath>
#include <stdexcept> 
#include <iostream>

class HeavyString{
	std::string H;
	std::unordered_map<size_t, char> _alt;
	std::unordered_map<int, double> delta_pi;
	std::unordered_map<int, std::vector<int>> alt_pos;
	std::unordered_map<int, std::pair<int, int>> alt_ext;
	std::vector<double> pi_suf;
	size_t n;
	size_t N;

	public:
	HeavyString(){};
	HeavyString(std::vector<std::vector<double>>& P, std::string const& S, std::string& A) 
		: n(P.size()), N(S.size()) {
			if (n == 0 || N == 0) {
				throw std::invalid_argument("P and S cannot be empty.");
			}

			for(size_t i = 0; i < n; i++){
				size_t which_max = max_element(P[i].begin(), P[i].end()) - P[i].begin();
				H+=(A[which_max]);
			}		
			for(size_t i = 0; i < N; i++){
				if(S[i] != H[i%n]){
					if (A.find(S[i]) == std::string::npos) {
						throw std::invalid_argument("S contains a character not in A.");
					}
					_alt[i] = S[i];
				}
			}
		}
	
	HeavyString(std::string const& S): n(S.size()), N(S.size()){
			H = S;
	}
	
	HeavyString(std::vector<std::vector<double>>& P,  std::string const& S, std::string& A, std::vector<int> min_pos, std::vector<int> le, std::vector<int> re, bool create_pi){
		n = P.size();
		N = S.size();
		if (n == 0 || N == 0) {
			throw std::invalid_argument("P and S cannot be empty.");
		}
		
		std::vector<double> pi_arr;
		
		for(size_t i = 0; i < n; i++){
			int which_max = max_element(P[i].begin(), P[i].end()) - P[i].begin();
			H+=(A[which_max]);
			double pi = log2(P[i][which_max]);
			pi_arr.push_back(pi);
		}		
		
		if(create_pi){				
			pi_suf.assign(pi_arr.begin(), pi_arr.end());
			for(int i = n-2; i >= 0; i--){
				pi_suf[i] += pi_suf[i+1];
			}
		}
		
		for(int m : min_pos){
			int begin = m - le[m];
			int end = m + re[m] + 1;
			alt_ext[m].first = le[m];
			alt_ext[m].second = re[m];
			for(int i = begin; i < end; i++){
				int h = i%n;
				if(H[h] != S[i]){
					double this_pi = log2(P[h][A.find(S[i])]);
					_alt[i] = S[i];
					alt_pos[m].push_back(i);
					delta_pi[i] =  this_pi - pi_arr[h];
				}
			}
		}
	}

	HeavyString(const HeavyString& other): H(other.H), _alt(other._alt), n(other.n), N(other.N) {}

	HeavyString& operator=(const HeavyString& other) {
		if (this != &other) {
			H = other.H;
			n = other.n;
			N = other.N;
			pi_suf.assign(other.pi_suf.begin(), other.pi_suf.end());
			std::unordered_map<size_t, char>temp1(other._alt);
			std::unordered_map<int, double>temp2(other.delta_pi);
			std::unordered_map<int, std::vector<int>> temp3(other.alt_pos);
			std::unordered_map<int, std::pair<int, int>> temp4(other.alt_ext);
			std::swap(_alt, temp1);
			std::swap(delta_pi, temp2);
			std::swap(alt_pos, temp3);
			std::swap(alt_ext, temp4);
		}
		return *this;
	}

	char& operator[](size_t i) { 
		if (i >= N) {
			throw std::out_of_range("Index out of range.");
		}

		if(_alt.count(i)){
			return _alt.at(i);
		}else{
			return H[i%n];
		}
	}
	
	char& at(size_t i) { 
		if (i >= N) {
			throw std::out_of_range("Index out of range.");
		}

		if(_alt.count(i)){
			return _alt.at(i);
		}else{
			return H[i%n];
		}
	}

	std::string substr(size_t pos, size_t len){
		if (pos >= N || len == 0) {
			return "";
		}
		std::string substring = H.substr(pos%n, len);

		for(size_t i = 0; i < len; i++){
			if(_alt.count(pos+i)){
				substring[i] = _alt.at(pos+i);
			}
		}

		return substring;
	}
	
	double get_pi(int i, int begin, int length){
		if(begin%n > i%n)			return 0;
		if(begin%n + length > n)	return 0;
		if( i - alt_ext[i].first > begin ) return 0;
		if( i + alt_ext[i].second < begin + length - 1 ) return 0;
				
		int end = begin + length;
		
		double cum_pi = pi_suf[begin%n] - pi_suf[end%n];
		
		std::vector<int>& v = alt_pos[i];
		if(v.empty()){
			return pow(2,cum_pi);
		}else{
			for(auto j : v){
				if(j >= begin && j < end){					
					cum_pi += delta_pi[j];
				}
			}
			return pow(2,cum_pi);
		}
	}
	
	double check_pi(std::string& pat, size_t pat_begin, size_t txt_begin, size_t length, size_t min_pos){
		for(auto i = 0; i < length; i++){
			if(pat[pat_begin + i] != this->at(txt_begin+i)){
				return 0;
			}
		}
		return this->get_pi(min_pos,txt_begin, length);
	}
	
	size_t le(size_t i){
		return alt_ext[i].first;
	}
	
	size_t re(size_t i){
		return alt_ext[i].second;
	}
	
	size_t length() const {return N;}
	size_t heavy_length() const {return n;}

	class const_iterator : public std::iterator<std::random_access_iterator_tag, char> {
    private:
        size_t index;
        HeavyString* hs;
    public:
        // constructor
		const_iterator(size_t i=-1, HeavyString* h=nullptr) : index(i), hs(h) {}
		
        // dereference operator
        char operator*() const  {
            return (*hs)[index];
        }

        // increment operators
        const_iterator& operator++() {
            index++;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator temp = *this;
            ++(*this);
            return temp;
        }

        // decrement operators
        const_iterator& operator--() {
            index--;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator temp = *this;
            --(*this);
            return temp;
        }

		const_iterator operator+(size_t n) const {
			return const_iterator(index + n, hs);
		}

		friend const_iterator operator+(size_t n, const const_iterator& it) {
			return it + n;
		}

		const_iterator& operator+=(size_t n) {
			index += n;
			return *this;
		}

		const_iterator operator-(size_t n) const {
			return (*this) + (-n);
		}

		friend ptrdiff_t operator-(const const_iterator& lhs, const const_iterator& rhs) {
			return lhs.index - rhs.index; 
		}

		const_iterator& operator-=(size_t n) {
			(*this) += (-n);
			return *this;
		}

        // comparison operators
        bool operator==(const const_iterator& other) const  {
            return (index == other.index && hs == other.hs);
        }

       bool operator==(const std::string::const_iterator& other) const  {
            return ((*hs)[index] == *other);
        }
		
        bool operator!=(const const_iterator& other) {
            return !(*this == other);
        }
	};

	const_iterator begin() {return const_iterator(0, this);}
	const_iterator end() {return const_iterator(N, this);}
};

std::ostream& operator<<(std::ostream& os, HeavyString& hs);
#endif