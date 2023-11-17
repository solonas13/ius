#include "heavy_string.h"

std::ostream& operator<<(std::ostream& os, HeavyString& hs) {
	for (size_t i = 0; i < hs.length(); i++) {
		os << hs[i];
	}
	return os;
}
