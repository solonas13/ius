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
#pragma once

#include<string>
#include<vector>
#include<ostream>


class PropertyString {
    std::string S;
    std::vector<int> _pi;
    
    friend std::ostream& operator << (std::ostream& out, PropertyString const &S);

public:
    PropertyString(int N=0): S(N,'\0'), _pi(N, 0) {}
    
    size_t length() const { return S.length(); }
    char& operator[](size_t i) { return S[i]; }
    int& pi(size_t i) { return _pi[i]; }
    std::string const& string() const { return S; }
    std::vector<int> const& property() const { return _pi; }
    
    PropertyString& operator += (PropertyString const & other);
};
