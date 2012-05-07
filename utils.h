#ifndef EKHEM_UTILS_H
#define EKHEM_UTILS_H

#include <string>
#include <boost/lexical_cast.hpp>

namespace ekhem {

std::string hash(const std::string& input) {
	const unsigned char* str = reinterpret_cast<const unsigned char*>(input.c_str());
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return boost::lexical_cast<std::string>(hash);
}

} // namespace

#endif
