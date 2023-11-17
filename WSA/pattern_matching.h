#ifndef PM_H 
#define PM_H

#include <unordered_set>
#include <math.h>
#include <string>

#include <sdsl/rmq_support.hpp>	
#include "property_string.h"

using namespace std;
using namespace sdsl;

pair<int64_t,int64_t> pattern_matching ( string & w, PropertyString & a, int * SA, int * LCP, rmq_succinct_sct<> &rmq, int64_t n );

#endif