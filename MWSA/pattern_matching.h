#ifndef PM_H 
#define PM_H

#include <unordered_set>
#include <math.h>
#include <string>

#include <sdsl/bit_vectors.hpp>   
#include <sdsl/rmq_support.hpp>	
#include <sdsl/io.hpp>
#include <boost/functional/hash.hpp>

#include "heavy_string.h"

using namespace boost;
using namespace std;
using namespace sdsl;

int64_t find_minimizer_index(string s, int64_t k);
pair<int64_t,int64_t> pattern_matching ( string & w, HeavyString & a, int * SA, int * LCP, rmq_succinct_sct<> &rmq, int64_t n );
pair<int64_t,int64_t> rev_pattern_matching ( string & w, HeavyString & a, int * SA, int * LCP, rmq_succinct_sct<> &rmq, int64_t n );

#endif