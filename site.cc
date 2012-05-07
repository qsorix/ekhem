#include "site.h"
#include "entity.h"
#include "uri.h"
#include "exceptions.h"
#include "utils.h"
#include "search.h"
#include "paths.h"
#include <fstream>
#include <iostream>
#include <boost/filesystem/operations.hpp>

Site::Site(const boost::filesystem::path& path) {
	using namespace boost::filesystem;

	links_hash = ekhem::hash(path.string());

	std::ifstream config((path/"config.txt").string().c_str());
	if(!config.good())
		throw ekhem::configuration_not_found((path/"config.txt").string());

	while(true) {
		std::string url;
		config >> url;
		if(config.good())
			urls.push_back(url);
		else break;
	}

	// FIXME: w konfiguracji nie ma adresow, moze inny wyjatek rzucic.
	if(urls.empty() || urls[0].empty())
		throw ekhem::configuration_not_found((path/"config.txt").string());

	if (exists(path/"pattern.html")) {
		pattern.reset(new Page(path/"pattern.html"));
	}

	prepare_links();
}

void Site::find_all_links(TidyNode parent, const std::string& url) {
	TidyNode child;

	for ( child = tidyGetChild(parent); child; child = tidyGetNext(child) ) {
		if(tidyNodeIsA(child)) {
			TidyAttr href = tidyAttrGetHREF(child);
			if(!href) continue;
			if(!tidyAttrValue(href)) continue;

			std::string link = ekhem::build_uri(tidyAttrValue(href), url);
			if(link.find("http://") != 0)
				continue;

			std::string title = ekhem::get_node_text(page->document(), child);
			if(previous_links.count(link)==0) {
				if(new_links[link].empty()) {
					new_links[link] = title;
				}
			}
		}
		find_all_links( child, url );
	}
}

void Site::prepare_links() {
	load_previous_links();

	for(std::vector<std::string>::const_iterator i = urls.begin(); i!=urls.end(); ++i) {
		try {
			Entity ent(*i);
			page.reset(new Page(ent));
			find_all_links(page->root(), ent.get_url());
		} catch (const std::runtime_error&e) {
			// FIXME: zrobic to jak bozia przykazala
			// skip wrong entries
		}
	}
}

void Site::load_previous_links() {
	using namespace boost::filesystem;
	path links(ekhem::path::data);
	links/=links_hash;

	std::cout << links.string() << std::endl;

	if(!exists(links))
		return;


	std::ifstream file(links.string().c_str());
	while(file.good()) {
		std::string link;
		file >> link;
		if(!link.empty())
			previous_links.insert(link);
	}
}

Site::const_links_iterator Site::links_begin() const {
	return new_links.begin();
}

Site::const_links_iterator Site::links_end() const {
	return new_links.end();
}

bool Site::has_pattern() const {
	return pattern;
}

const TidyNode Site::get_pattern() const {
	assert(pattern);
	return pattern->root();
}

void Site::save_links() {
	// FIXME: funkcja nie jest exception safe
	using namespace boost::filesystem;
	path links(ekhem::path::data);
	links/=links_hash;

	std::ofstream file(links.string().c_str());

	for(const_links_iterator i=new_links.begin(); i!=new_links.end(); ++i) {
		previous_links.insert(i->first);
	}

	for(simple_links_container::const_iterator i=previous_links.begin(); i!=previous_links.end(); ++i) {
		file << *i << std::endl;
	}
}
