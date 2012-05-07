#ifndef EKHEM_SITE_H
#define EKHEM_SITE_H

#include "page.h"
#include <boost/filesystem/path.hpp>
#include <boost/smart_ptr.hpp>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <tidy.h>

/** Reprezentacja obserwowanej strony.
 * Odczytuje konfiguracje strony, pobiera ją i udostępnia nowe linki
 */
class Site {
	typedef std::set<std::string> simple_links_container;
	typedef std::map<std::string, std::string> links_container;
public:
	typedef links_container::const_iterator const_links_iterator;
private:

	boost::scoped_ptr<Page>   page;
	boost::scoped_ptr<Page>   pattern;
	std::vector<std::string>  urls;
	std::string               links_hash;
	links_container           new_links;
	simple_links_container    previous_links;

	void find_all_links(TidyNode, const std::string& url);
	void load_previous_links();
	void prepare_links();

public:
	Site(const boost::filesystem::path&);

	const_links_iterator links_begin() const;
	const_links_iterator links_end() const;

	void save_links();

	bool has_pattern() const;
	const TidyNode get_pattern() const;

};

#endif
