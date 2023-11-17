/**
 *    Weighted Index
 *    Copyright (C) 2017 Carl Barton, Tomasz Kociumaka, Chang Liu, Solon P. Pissis and Jakub Radoszewski.
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <iostream>
#include <string>
#include <getopt.h>
#include "input.h"

using std::cerr;
using std::endl;

void usage ( ) {
    std::cerr << "Usage: Weighted Index <options>"  << std::endl;
    std::cerr << "Standard:" << std::endl;
    std::cerr << "	-t, --text\t<str>\tFilename for Text. Default: standard input." << std::endl;
    std::cerr << "	-o, --output\t<str>\tFilename for Output. Default: standard output." << std::endl;
    std::cerr << "	-z, --threshold\t <dbl>\tCumulative weight threshold (non-negative)."<< std::endl;
    std::cerr << "	-q, --quiet\t Do NOT print the z-estimation and the Property Suffix Tree."<< std::endl;
}

static struct option long_options[] =
{
    { "text",				required_argument,	NULL,	't' },
    { "length",				required_argument,	NULL,	'n' },
    { "output",				required_argument,	NULL,	'o' },
    { "patterns",			required_argument,	NULL,	'p' },
    { "quiet",		    	no_argument,	    NULL,	'q' },
    { "threshold",			required_argument,	NULL,	'z'	},
    { "help",				optional_argument,	NULL,	'h' },
	{ "ell",				required_argument,  NULL,	'l'	},
};

Settings::Settings(): z(-1), quiet(false) {}

Settings decode_switches (int argc, char * argv[]) {
    int opt;
    Settings st;
	char *ep;
    while ( (opt = getopt_long ( argc, argv, "t:o:p:z:l:hq", long_options, NULL ) ) != -1 )
    {
        switch ( opt )
        {
            case 't':
                st.text = std::ifstream(optarg);
                break;
			case 'n':
				st.length = strtol(optarg, &ep, 10);
				if( optarg == ep) throw 1;
				break;
			case 'l':
				st.ell = strtol(optarg, &ep, 10);
				if( optarg == ep) throw 1;
				break;
            case 'p':
                st.patterns = optarg;
                break;
            case 'o':
                st.output = std::ofstream(optarg);
                break;
            case 'z':
                st.z = strtod ( optarg, &ep );
                if ( optarg == ep) throw 1;
                break;
            case 'q':
                st.quiet = true;
            case 'h':
                usage();
                exit(1);
        }
    }
    if (st.z < 0) {
        cerr << "A non-negative parameter z must be specified!" << endl << endl;
        usage();
    }
    return st;
}
