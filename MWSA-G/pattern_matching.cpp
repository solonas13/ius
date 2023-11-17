#include <unordered_set>
#include <math.h>
#include <string>

#include <sdsl/bit_vectors.hpp>                                   // include header for bit vectors
#include <sdsl/rmq_support.hpp>	
#include <sdsl/io.hpp>

#include "pattern_matching.h"

#include <boost/functional/hash.hpp>
using namespace boost;
using namespace std;
using namespace sdsl;

extern string zstrs;

int64_t  find_minimizer_index(string s, int64_t  k) {
  int64_t  minimizer_index = 0;
  for (int64_t  i = 1; i <= s.length() - k; i++) {
    if (s.substr(i, k) < s.substr(minimizer_index, k)) {
      minimizer_index = i;
    }
  }

  return minimizer_index;
}


/* Computes the length of lcp of two suffixes of two strings */
int64_t  lcp ( HeavyString & x, int64_t  M, string & y, int64_t  l, int64_t c)
{
	int64_t  xx = c + x.re(c);
	if ( M > xx ) return 0;
	int64_t  yy = y.size();
	if ( l >= yy ) return 0;

	int64_t  i = 0;
	while ( ( M + i < xx + 1) && ( l + i < yy ) )
	{
		if ( x[M+i] != y[l+i] )	break;
		i++;
	}
	return i;
}

/* Searching a list of strings using LCP from "Algorithms on Strings" by Crochemore et al. Algorithm takes O(m + log n), where n is the list size and m the length of pattern */
pair<int64_t,int64_t> pattern_matching ( string & w, HeavyString & a, int * SA, int * LCP, rmq_succinct_sct<> &rmq, int64_t n )
{
	int64_t m = w.size(); //length of pattern
	int64_t N = a.size(); //length of string
	int64_t d = -1;
	int64_t ld = 0;
	int64_t f = n;
	int64_t lf = 0;

	pair<int64_t,int64_t> interval;

	while ( d + 1 < f )
	{
		
		int64_t i = (d + f)/2;
		
		/* lcp(i,f) */
		int64_t lcpif;
		
		if( f == n )
			lcpif = 0;
		else lcpif = LCP[rmq ( i + 1, f ) ];
			
		/* lcp(d,i) */
		int64_t lcpdi;
		
		if( i == n )
			lcpdi = 0;
		else lcpdi = LCP[rmq ( d + 1, i ) ];
		
		if ( ( ld <= lcpif ) && ( lcpif < lf ) )
		{
			d = i;
			ld = lcpif;
		}
		else if ( ( ld <= lf ) && ( lf < lcpif ) ) 	f = i;
		else if ( ( lf <= lcpdi ) && ( lcpdi < ld ) )
		{
			f = i;
			lf = lcpdi;
		}
		else if ( ( lf <= ld ) && ( ld < lcpdi ) )	d = i;
		else
		{
			int64_t l = std::max (ld, lf);
			l = l + lcp ( a, SA[i] + l, w, l, SA[i] );
			if ( l == m ) //lower bound is found, let's find the upper bound
		        {
				int64_t e = i;
				while ( d + 1 < e )
				{
					int64_t j = (d + e)/2;

					/* lcp(j,e) */
					int64_t lcpje;
				
					if( e == n )
						lcpje = 0;
					else lcpje = LCP[rmq ( j + 1, e ) ];
					
					if ( lcpje < m ) 	d = j;
					else 			e = j;
				}

				/* lcp(d,e) */
				int64_t lcpde;
				
				if( e == n )
					lcpde = 0;
				else lcpde = LCP[rmq ( d + 1, e ) ];
				
				if ( lcpde >= m )	d = std::max (d-1,( int64_t ) -1 );

				e = i;
				while ( e + 1 < f )
				{
					int64_t j = (e + f)/2;

					/* lcp(e,j) */
					int64_t lcpej;
					
					if( j == n )
						lcpej = 0;
					else lcpej = LCP[rmq ( e + 1, j ) ];
					
					if ( lcpej < m ) 	f = j;
					else 			e = j;
				}

				/* lcp(e,f) */
				int64_t lcpef;
				
				if( f == n )
					lcpef = 0;
				else lcpef = LCP[rmq ( e + 1, f ) ];
				
				if ( lcpef >= m )	f = std::min (f+1,n);

				interval.first = d + 1;
				interval.second = f - 1;
				return interval;


			}
			else if ( ( l == N - SA[i] ) || ( ( SA[i] + l < N ) && ( l != m ) && ( (a[SA[i]+l] < w[l]) || (a.re(SA[i]) < l) ) ) )
			{
				d = i;
				ld = l;
			}
			else
			{
				f = i;
				lf = l;
			}
		}
	}
	

	interval.first = d + 1;
	interval.second = f - 1;
	return interval;
}

/* Computes the length of lcs of two suffixes of two strings */
int64_t  lcs ( HeavyString & x, int64_t  M, string & y, int64_t  l, int c)
{
	int64_t xx = c - x.le(c);
	// cout << "left ext: " << x.le(c) << endl;
	if ( M < xx ) return 0;
	int64_t  yy = y.size();
	if ( l >= yy ) return 0;

	int64_t  i = 0;
	while ( ( M - i >= xx ) && ( l + i < yy ) )
	{
		// cout << "letter compare: " << x[M-i] << " " << y[l+i] << endl;
		if ( x[M-i] != y[l+i] )	break;
		i++;
	}
	return i;
}

pair<int64_t,int64_t> rev_pattern_matching ( string & w, HeavyString & a, int * SA, int * LCP, rmq_succinct_sct<> &rmq, int64_t n )
{
	
	
	int64_t m = w.size(); //length of pattern
	int64_t N = a.size(); //length of string
	int64_t d = -1;
	int64_t ld = 0;
	int64_t f = n;
	int64_t lf = 0;

	pair<int64_t,int64_t> interval;
	

	while ( d + 1 < f )
	{
		int64_t i = (d + f)/2;
		int64_t revSA = N - 1 - SA[i];
		//std::unordered_map<pair<int64_t,int64_t>, int64_t, boost::hash<pair<int64_t,int64_t> >>::iterator it;

		/* lcp(i,f) */
		int64_t lcpif;
		
		if( f == n )
			lcpif = 0;
		else lcpif = LCP[rmq ( i + 1, f ) ];
		
		/* lcp(d,i) */
		int64_t lcpdi;
		//it = rmq.find(make_pair(d+1, i));
		
		if( i == n )
			lcpdi = 0;
		else lcpdi = LCP[rmq ( d + 1, i ) ];
		
		// if(d != -1 && f!=n){
			// cout << i << " d=" << d << " f=" << f << " ld=" << ld << " lf=" << lf << " lcpdi=" << lcpdi << " lcpif=" << lcpif << endl;
			// string si = zstrs.substr(  N-1-SA[i]-max(24, (int) a.le(N-1-SA[i])), max(25, (int) a.le(N-1-SA[i])+1));
			// string sd = zstrs.substr(  N-1-SA[d]-a.le(N-1-SA[d]), a.le(N-1-SA[d]) + 1);
			// string sf = zstrs.substr(  N-1-SA[f]-max(24, (int) a.le(N-1-SA[f])), max(25, (int) a.le(N-1-SA[f])+1));
			// reverse(si.begin(), si.end());
			// reverse(sd.begin(), sd.end());
			// reverse(sf.begin(), sf.end());
			// cout << sd << " " << a.le(N-1-SA[d])+1 << endl;
			// cout << si << " " << a.le(N-1-SA[i])+1 << endl;
			// cout << sf << endl;
		// }
	
		if ( ( ld <= lcpif ) && ( lcpif < lf ) )
		{
			d = i;
			ld = lcpif;
		}
		else if ( ( ld <= lf ) && ( lf < lcpif ) ) 	f = i;
		else if ( ( lf <= lcpdi ) && ( lcpdi < ld ) )
		{
			f = i;
			lf = lcpdi;
		}
		else if ( ( lf <= ld ) && ( ld < lcpdi ) )	d = i;
		else
		{
			int64_t l = std::max (ld, lf);
			
			// avoid the function call if revSA-1<0 or l>=w.size() by changing lcs?
			l = l + lcs ( a, revSA - l, w, l, revSA );
			// cout << "l: " << l << endl;
			if ( l == m ) //lower bound is found, let's find the upper bound
		    	{
				int64_t e = i;
				while ( d + 1 < e )
				{
					int64_t j = (d + e)/2;

					/* lcp(j,e) */
					int64_t lcpje;
					
					if( e == n )
						lcpje = 0;
					else lcpje = LCP[rmq ( j + 1, e ) ];
					
					if ( lcpje < m ) 	d = j;
					else 			e = j;
				}

				/* lcp(d,e) */
				int64_t lcpde;
				
				
				if( e == n )
					lcpde = 0;
				else lcpde = LCP[rmq ( d + 1, e ) ];
				
				if ( lcpde >= m )	d = std::max (d-1,( int64_t ) -1 );
			
				e = i;
				while ( e + 1 < f )
				{
					int64_t j = (e + f)/2;

					/* lcp(e,j) */
					int64_t lcpej;
					
					
					if( j == n )
						lcpej = 0;
					else lcpej = LCP[rmq ( e + 1, j ) ];
					
					if ( lcpej < m ) 	f = j;
					else 			e = j;
				}

				/* lcp(e,f) */
				int64_t lcpef;
				
				if( f == n )
					lcpef = 0;
				else lcpef = LCP[rmq ( e + 1, f ) ];
				
				if ( lcpef >= m )	f = std::min (f+1,n);

				interval.first = d + 1;
				interval.second = f - 1;
				return interval;


			}
			else if ( ( l == N - SA[i] ) || ( ( revSA - l >= 0 ) && ( l != m ) && ( ( a[revSA - l] < w[l] ) || a.le(revSA) < l ) ))
			{
				// cout << a[revSA-l] << " " << w[l] << endl;
				d = i;
				ld = l;
				// cout << "go f side" << endl;
			}
			else
			{
				// cout << a[revSA-l] << " " << w[l] << endl;
				f = i;
				lf = l;
				// cout << "go d side" << endl;
			}

		}
	}
	

	interval.first = d + 1;
	interval.second = f - 1;
	
	return interval;
}