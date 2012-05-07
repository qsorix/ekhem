#include "paths.h"
#include <boost/filesystem/operations.hpp>

#ifdef WIN32
#include <shlobj.h>
// #include <direct.h> <-- to tez?

static std::string get_folder_path(DWORD id) {
	char path[MAX_PATH] = { 0 };
	
	SHGetFolderPath(
		NULL,      // hwnd
		id,        // CSIDL_FOO
		NULL,      // access token,
		0,         // current location
		path );    // buffer

	return path;
};
#endif

boost::filesystem::path base_path() {
#ifdef WIN32
	boost::filesystem::path base(get_folder_path(CSIDL_APPDATA));
	return base;
#else
	return boost::filesystem::path();
#endif
}

const boost::filesystem::path ekhem::path::sites = base_path()/"sites";
const boost::filesystem::path ekhem::path::output = base_path()/"output";
const boost::filesystem::path ekhem::path::data = base_path()/"data";

void ekhem::path::create_directories() {
	namespace bf = boost::filesystem;

	if(!bf::exists(sites));
		bf::create_directory(sites);

	if(!bf::exists(output));
		bf::create_directory(output);

	if(!bf::exists(data));
		bf::create_directory(data);
}

