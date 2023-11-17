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
	std::map<size_t, char> _alt;
	std::unordered_map<size_t, double> delta_pi;
	std::unordered_map<size_t, std::vector<int>> alt_pos;
	std::unordered_map<size_t, std::pair<int, int>> alt_ext;
	std::vector<double> pi_prefix;

	public:
	size_t n;
	size_t N;
	HeavyString(){};
	

	HeavyString(std::vector<std::vector<double>>& P, std::string const& S,std::string& A, list<pair<size_t,size_t>>& min_list,list<list<pair<int,char>>>& diffs,vector<double>& pi_pref){
			H = S;
			n=S.size();
			N= n* min_list.size();
			//cout<< "trying to copy pi pref"<<endl;
			pi_prefix=pi_pref;
			//cout <<"copy made"<<endl;
			//cout <<endl;
			list<pair<int,char>> diff;
			vector<double> pi_arr;
			//cout<< "array created"<<endl;
			pi_arr.push_back(pi_prefix[0]);
			//cout <<"pi_prefix range "<<pi_prefix[n-1]<<endl;
			for(int i=1;i<n;++i){
				//cout <<"pi arr computation "<< i <<endl;
				pi_arr.push_back(pi_prefix[i]-pi_prefix[i-1]);
			}
			//cout<< "pi array built"<<endl;
			size_t i=0;
			list<pair<size_t,size_t>>::iterator minit=min_list.begin();
			for(list<pair<size_t,size_t>>::iterator minit=min_list.begin(); minit!=min_list.end();++minit){
				//cout <<"storing minimizer "<< i <<endl;
				diff=diffs.front();
				diffs.pop_front();
				for(list<pair<int,char>>::iterator el=diff.begin();el!=diff.end();++el){
					double this_pi = log2(P[el->first][A.find(el->second)]);
					_alt[i*n+(size_t)el->first]=el->second;
					alt_pos[minit->first].push_back(i*n+(size_t)el->first);
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
		//for(size_t i = 0; i < len; i++){
		//	if(_alt.count(pos+i)){
				//substring[i] = _alt.at(pos+i);
		//	}
		//}

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
	
	int LCP(pair<size_t,size_t> min_str1,pair<size_t,size_t> min_str2){ //TODO maybe we can make faster LCP provided an LCP on the Heavy string  - here it is done in time linear to the string lengths
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
		//cout<<"base weight: " <<weight<<endl;
		for(size_t i = 0; i < len; i++){
			if(_alt.count(pos+i)){
				weight+= delta_pi.at(pos+i);
				//cout<< "modified at pos "<< pos+i<< " by "<<delta_pi.at(pos+i)<<endl;
			}
		}
		return weight;
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
