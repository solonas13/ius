#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>

using namespace std;

void LCParray( string& text, int n, int * SA, int * ME, int * LCP )
{
	int i = 0, j = 0, k = 0, l = 0;
	int * iSA = new int [n];
	for ( i = 0; i < n; i++ )
	{
		iSA[SA[i]] = i;
		LCP[i] = 0;
	}
	for ( i = 0; i < n; i++ )
	{
		if ( iSA[i] == 0)
		{
			k = 0;
			continue;
		}

		j = SA[iSA[i]-1];
		while ( i+k < n && j+l < n && text[i+k] == text[j+k] && k < ME[i] && k < ME[j] )
			k++;
		LCP[iSA[i]] = k;
		if ( k > 0 )
			k--;
	}
}




	
