/**
    Weighted Index
    Copyright (C) 2017 Carl Barton, Tomasz Kociumaka, Chang Liu, Solon P. Pissis and Jakub Radoszewski.
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
#include "property_string.h"

using std::endl;

PropertyString & PropertyString::operator+=(const PropertyString& other) {
    S += other.S;
    _pi.insert(_pi.end(), other._pi.begin(), other._pi.end());
    return *this;
}

std::ostream& operator << (std::ostream& out, PropertyString const &S) {
    for (char c : S.S) {
        //out << c << " ";
        out << c;
    }
    out << endl;
	for (int c : S._pi) {
        out << c << " ";
    }
    out << endl;
    return out;
}
