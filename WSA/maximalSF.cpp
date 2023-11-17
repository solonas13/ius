/**
    Weighted Suffix Array
    Copyright (C) 2017 Panagiotis.Charalampopoulos, Costas.Iliopoulos, Chang.Liu, Solon.Pissis
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <map>
#include <iostream>
#include <string>

#include "defs.h"

using namespace std;

int i = 0;
int j = 0;

double lp ( vector<vector<double>>& text, int position, char letter )
{
	unsigned int c;
	switch(letter)
	{
		case 'A':
			c = 0;
			break;
		case 'C':
			c = 1;
			break;
		case 'G':
			c = 2;
			break;
		case 'T':
			c = 3;
			break;
		default:
			c = 5;
			break;
	}
	if ( c == 5 )
		return 0;
	else
	{
		return text[position][c];
	}
}

int minimal ( int a, int b, int c )
{
	if ( a<=b && a<= c )
		return a;
	if ( b<=a && b<= c )
		return b;
	else
		return c;
}

void maximalSF ( vector<vector<double>>& text, string & sq, int N, int n, double z, int * ME )
{
	// compute valid SA using slide window:
	double p = 1.0;
	int valid_length = 0;
	int k = 0;

	for ( j = 0; j < z; j++ )
	{
		for ( i = 0; i < n; i++ )
		{
			if ( i == 0 || ME[i+j*(n)-1] <= 1 )
			{
				k = 0;
				p = 1.0;
				for ( k = 0; k < n-i; k++ )
				{
					if ( p * lp( text, i+k, sq[i+j*n+k] ) >= 1/z )
						p *= lp( text, i+k, sq[i+k+j*n]);
					else
						break;
				}
				ME[i+j*(n)] = k;
			}
			else
			{
				k = ME[i+j*(n)-1];
				p /= lp ( text, i-1, sq[i-1+j*(n)] );
				if ( p < 1/z )
					ME[i+j*(n)] = k -1;
				else
				{
					for ( k = k-1; k < n-i; k++ )
					{
						if ( p * lp( text, i+k, sq[i+k+j*(n)] ) >= 1/z )
							p *= lp( text, i+k, sq[i+k+j*(n)] );
						else
							break;
					}
				}
				ME[i+j*(n)] = k;
			}
		}
	}
}
