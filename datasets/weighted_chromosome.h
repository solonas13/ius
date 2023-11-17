#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <cmath>

#include "zlib.h"

using namespace std;

class weighted_chromosome{
	vector<vector<double>> wchr;
	size_t start_pos;
	size_t end_pos;
	
public:	
	weighted_chromosome(){
		start_pos = 0;
		end_pos = 0;
	}
	
	void readFasta(string& faFile){
		//read fasta file
		gzFile file = gzopen(faFile.c_str(), "r");
		if (!file) {
			std::cerr << "Failed to open file\n";
			return;
		}
		
		string sequence;
		char buffer[1000000];
		while (gzgets(file, buffer, 1000000)) {
			string line(buffer);
			line.erase(std::remove(line.begin(), line.end(), '\n'), line.cend());
			if(line[0] == '>') continue;
			else{
				sequence += line;
			}
		}
		
		//remove multiply 'N' at the beginning and end
		start_pos = sequence.find_first_not_of('N');
		sequence = sequence.substr(start_pos);
		end_pos = sequence.find_last_not_of('N');
		sequence = sequence.substr(0, end_pos + 1);
		
		//map to possibality matrix
		for(size_t i = 0; i < sequence.size(); i++){
			vector<double> row(4,0.0);
			switch (sequence[i]) {
				case 'A':
					row[0] = 1.0;
					break;
				case 'C':
					row[1] = 1.0;
					break;
				case 'G':
					row[2] = 1.0;
					break;
				case 'T':
					row[3] = 1.0;
					break;
				case 'N':
					row[0] = 0.25;
					row[1] = 0.25;
					row[2] = 0.25;
					row[3] = 0.25;
					break;
			}
			wchr.push_back(row);
		}
	}
	
	bool readVCFs(string& vcf_file){
		gzFile file = gzopen(vcf_file.c_str(), "r");
		if (!file) {
			std::cerr << "Failed to open file\n";
			return false;
		}
		char buffer[1000000];

		unordered_map<char, int> AL;
		AL['A'] = 0;
		AL['C'] = 1;
		AL['G'] = 2;
		AL['T'] = 3;
		
		int i = 0;
		while (gzgets(file, buffer, 1000000)) {
			string line(buffer);
			if (line[0] == '#') continue; // skip header lines
			vector<string> fields;
			size_t start = 0, end = 0;
			while ((end = line.find('\t', start)) != string::npos){
				fields.push_back(line.substr(start, end - start));
				start = end + 1;
			}
			fields.push_back(line.substr(start));
			string ref_s = fields[3];   
			string alt_s = fields[4];
			if (alt_s.length() != 1 || ref_s.length() != 1) continue; // skip lines with non-singleton RFF/ALT
			size_t pos = stoi(fields[1]);
			if(pos < start_pos) continue;
			pos = pos - start_pos;		
			int alt = AL[alt_s[0]];
			string af = fields[7].substr(fields[7].find("AF=") + 3);
			af = af.substr(0, af.find(";"));
			double vaf = stod(af);
			vaf = round(vaf * 1000000) / 1000000;
			i++;
		
			if(wchr[pos][alt] != 1){
				auto max = max_element(wchr[pos].begin(), wchr[pos].end());
				*max -= vaf;
				wchr[pos][alt] += vaf;
			}
		}
		return true;
	}
	
	vector<vector<double>> & getMatrix(){	return wchr;}
	
	friend std::ostream& operator << (std::ostream& out, weighted_chromosome const &S){
		for(size_t i = 0; i < S.wchr.size(); i++){
			out << S.wchr[i][0] << " " << S.wchr[i][1] << " " << S.wchr[i][2] << " " << S.wchr[i][3] << endl;
		}
		return out;
	}
			
};