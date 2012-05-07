#ifndef EKHEM_PATHS_H
#define EKHEM_PATHS_H

#include <boost/filesystem/path.hpp>

namespace ekhem {
	namespace path {
		extern const boost::filesystem::path sites;
		extern const boost::filesystem::path output;
		extern const boost::filesystem::path data;

		void create_directories();
	}
}

#endif
