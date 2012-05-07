#include "uri.h"
#include <libxml/uri.h>

std::string ekhem::build_uri(const std::string& uri, const std::string& base) {
	xmlChar* r = xmlBuildURI(reinterpret_cast<const unsigned char*>(uri.c_str()), reinterpret_cast<const unsigned char*>(base.c_str()));
	std::string ret(reinterpret_cast<const char*>(r));
	xmlFree(r);
	return ret;
}
