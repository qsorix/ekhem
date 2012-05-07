#include "page.h"
#include "exceptions.h"

#include "entity.h"
#include <boost/filesystem/path.hpp>
#include <buffio.h> // Tidy ma zjebane te naglowki

bool is_html(const std::string& content) {
	return (content.find("text/html") != std::string::npos);
}

Bool ignore(TidyDoc tdoc, TidyReportLevel lvl, uint line, uint col, ctmbstr mssg ) {
	return no;
}

TidyDoc tidy_create() {
	TidyDoc ret = tidyCreate();
	// Dokumentacja Tidy k#@$@sko dokladnie opisuje wartosci zwracane z tych
	// funkcji.
 	tidySetReportFilter(ret, ignore);
 	tidySetCharEncoding(ret, "raw");
	return ret;
};

Page::Page(const Entity& page) : tree(NULL) {
	if(!is_html(page.get_content_type())) {
		throw ekhem::entity_is_not_webpage(page.get_content_type() + " " + page.get_url());
	}
	tree = tidy_create();
 	tidyParseString(tree, page.get_text().c_str());
}

Page::Page(const boost::filesystem::path& path) : tree(tidy_create()) {
	tidyParseFile(tree, path.string().c_str());
}

Page::~Page() {
	if(tree)
		tidyRelease(tree);
}

const TidyNode Page::root() const {
	assert(tree);
	return tidyGetRoot(tree);
}

TidyDoc& Page::document() {
	return tree;
}
