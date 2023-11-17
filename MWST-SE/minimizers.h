#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

using namespace std;

class MinimizerHeap{
	uint64_t n,l,k,len,lefthash,righthash, letter_k;
	set<pair<uint64_t,uint64_t>> heap; // (kr hash, position)
	//string S;
	vector<char> S;
	
	public:
	   MinimizerHeap(uint64_t n1, uint64_t l1, uint64_t k1);
	   
	   uint64_t top();
	   void left(char a);
	   void right();
};


uint64_t compute_minimizers(std::string& text, uint64_t w, uint64_t k, std::unordered_set<uint64_t> &minimizers );
uint64_t pattern_minimizers(std::string& text,uint64_t k);
