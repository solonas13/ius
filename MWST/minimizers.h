#include <string>
#include <vector>
#include <utility>
#include <unordered_set>

uint64_t compute_minimizers(std::string& text, uint64_t w, uint64_t k, std::unordered_set<uint64_t> &minimizers );
uint64_t pattern_minimizers(std::string& text,uint64_t k);
