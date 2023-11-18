#ifndef HEAVY_STRING_H
#define HEAVY_STRING_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <map>
#include <utility>
#include <string>
#include <algorithm>
#include <cstddef>
#include <cmath>
#include <stdexcept> 
#include <iostream>

using namespace std;

class HeavyString{
	std::string H;
	std::map<size_t, char> _alt; //here we use map, as elements when multiple elements are accessed they are often close in the structure - the difference in time is noticable
	std::unordered_map<size_t, double> delta_pi;
	std::vector<double> pi_prefix;

	public:
	size_t n;
	size_t N;
	HeavyString(){};
	

	HeavyString(std::vector<std::vector<double>>& P, std::string const& S,std::string& A, list<pair<size_t,size_t>>& min_list,list<list<pair<int,char>>>& diffs,vector<double>& pi_pref){
			H = S;
			n=S.size();
			N= n* min_list.size();
			pi_prefix=pi_pref;
			list<pair<int,char>> diff;
			vector<double> pi_arr;
			pi_arr.push_back(pi_prefix[0]);
			for(int i=1;i<n;++i){
				pi_arr.push_back(pi_prefix[i]-pi_prefix[i-1]);
			}
			size_t i=0;
			list<pair<size_t,size_t>>::iterator minit=min_list.begin();
			for(list<pair<size_t,size_t>>::iterator minit=min_list.begin(); minit!=min_list.end();++minit){
				diff=diffs.front();
				diffs.pop_front();
				for(list<pair<int,char>>::iterator el=diff.begin();el!=diff.end();++el){
					double this_pi = log2(P[el->first][A.find(el->second)]);
					_alt[i*n+(size_t)el->first]=el->second;
					delta_pi[i*n+(size_t)el->first] =  this_pi - pi_arr[el->first];	
				}
				++i;
			}
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

		map<size_t,char>::iterator alt_iter = _alt.lower_bound(pos);
		while((alt_iter!=_alt.end()) && (alt_iter->first<pos+len)){
			substring[alt_iter->first-pos]=alt_iter->second;
			++alt_iter;
		}
		return substring;
	}
	
	struct Heavycompare : std::binary_function<pair<size_t,size_t>, pair<size_t,size_t>,bool>
	{
		Heavycompare(HeavyString *h) : _Heavy1(h) {}
		bool operator() (const pair<size_t,size_t> &min_str1, const pair<size_t,size_t> &min_str2){
			string str1=_Heavy1->substr(min_str1.first,min_str1.second-min_str1.first);
			string str2=_Heavy1->substr(min_str2.first,min_str2.second-min_str2.first);
			return (str2.compare(str1)>0);
		}
		    HeavyString * _Heavy1;
	};
	
	int LCP(pair<size_t,size_t> min_str1,pair<size_t,size_t> min_str2){ //In theory using a blackbox LCP structure for the heavy string H improves the running time, but in practice the linear time comparison of strings is more efficient
		string str1=substr(min_str1.first,min_str1.second-min_str1.first);
		string str2=substr(min_str2.first,min_str2.second-min_str2.first);
		int len=min(str1.length(),str2.length());
		int i=0;
		while((i<len)&&(str1[i]==str2[i])) ++i;
		return i;
	}
	
	
	
	double substr_weight(size_t pos, size_t len){
		if (pos >= N || len == 0) {
			return 1;
		}
		double weight;
		if(pos%n!=0){
			weight = pi_prefix[pos%n+len-1]-pi_prefix[pos%n-1];
		}else{
			weight=pi_prefix[pos%n+len-1];
		}
	
		for(size_t i = 0; i < len; i++){
			if(delta_pi.count(pos+i)){
				weight+= delta_pi.at(pos+i);
			}
		}		
		
		return weight;
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
