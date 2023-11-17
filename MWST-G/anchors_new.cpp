/*
Compile:
cd /scratch/users/k1635844/OLD_LUSTRE_2019-12-05_15:24:35/k1635844/anchors
module add compilers/gcc/7.3.0
make -f Makefile.anchors_new.gcc


*/
#define DEBUG false
#include <algorithm>
#include <chrono>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <sys/time.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <deque>
#include <list>

using namespace std;

/* Computes the minimizers of a string using Karp & Rabin fingerprints -- Algorithm 1 from Jain et al. */
void minimizers_with_kr(string const &whole_string, vector<int> &M, int w, int k)
{   
	deque<pair<int, string> > Q;
	uint64_t hash_string_in_win;
	int min_pos;
	int whole_string_len = whole_string.size();
	int start_pos = 0;
	string cur_str;

	for(int i = 0; i < (whole_string_len-k); ++i)	   
	{      	 	
		cur_str = whole_string.substr(i,k);		 			
		
		while(!Q.empty() && Q.back().second > cur_str){
			Q.pop_back();
		}

		pair<int, string> p(i,cur_str);
		Q.push_back(p);
		
		if(Q.front().first <=i-w)
		{
			while(Q.front().first <= i-w)	Q.pop_front();
		}

		if(i >= w-1)
		{
			if(M.empty())
			{
				pair<int, string> p(Q.front().first,Q.front().second);
				M.push_back(p.first);
			}
			else if(M.back() != Q.front().first)
			{
				pair<int, string> p(Q.front().first,Q.front().second);
				M.push_back(p.first);
			}
		}

		//FurtherSample to resolve ties 
		while(Q.size()>1 && i>=w) //&& (Q.front().second == (++it)->second))
		{
			auto it = Q.begin(); 	 	 
			it++;
			if(it->second==Q.front().second)
			{
				Q.pop_front();	
				if(M.back()!=Q.front().first)
				{
					pair<int, string> p(Q.front().first,Q.front().second);
					M.push_back(p.first);		 
				}
			}
			else
			{
				break;
			}
		}
	}
}
