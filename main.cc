#include <iostream>
#include <fstream>
#include "entity.h"
#include "uri.h"
#include "page.h"
#include "site.h"
#include "search.h"
#include "exceptions.h"
#include "paths.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/replace.hpp>

class Output {
	std::ofstream out;

public:
	explicit Output(const boost::filesystem::path& outdir) : out((outdir/"output.html").string().c_str()) {
		out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"" << std::endl;
		out << "  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << std::endl;
		out << "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"pl\" xml:lang=\"pl\">" << std::endl;
		out << "<head>" << std::endl;
		out << "<meta name=\"robots\" content=\"noindex, nofollow\" />" << std::endl;
		out << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>" << std::endl;
		out << "<link rel=\"stylesheet\" href=\"style.css\" type=\"text/css\"/>" << std::endl;
		out << "</head>" << std::endl;
		out << "<body>" << std::endl;
	}

	~Output() {
		out << "</body>" << std::endl;
		out << "</html>" << std::endl;
	}

	void site_begin(const std::string& site_name) {
		out << "<h1>" << site_name << "</h1>" << std::endl;
	}


	void site_end() {}

	void site_no_config() {
		out << "<p><em>Nie znaleziono pliku konfiguracji.</em></p>" << std::endl;
	}

	void not_a_webpage() {
		out << "<p><em>Ten link nie wskazuje na strone o zawartosci tex/html.</em></p>" << std::endl;
	}

	void site_link(const std::string& url, const std::string& title=std::string()) {
		out << "<h2><a href=\"" << url << "\">";
		if(title.empty())
			out << url;
		else
			out << title;
		out << "</a></h2>" << std::endl;
	}

	void site_text(const std::string& text) {
		std::string f(text);
		boost::algorithm::replace_all(f, "\n", "<br/>");
		out << "<p>" << f << "</p>" << std::endl;
	}

	void site_images(const std::set<std::string>& images) {
		if(images.empty())
			return;
		out << "<h3>Obrazy</h3>" << std::endl;
		for(std::set<std::string>::const_iterator i=images.begin(); i!=images.end(); ++i) {
			out << "<a href=\"" << *i << "\">" << *i << "</a>" << std::endl;
		}
	}
};

void process_site(const Site& site, const std::string& url, const std::string& label, Output& output) {
	std::cout << "  nowy link: " << url << std::endl;
	output.site_link(url, label);

	if(site.has_pattern()) {
		try {
			Entity entity(url);
			Page linked(entity);
			std::set<std::string> images1, images;

			std::string summary = ekhem::extract_summary(linked.document(), site.get_pattern(), images1);
			for(std::set<std::string>::const_iterator img=images1.begin(); img!=images1.end(); ++img) {
				images.insert(ekhem::build_uri(*img, entity.get_url()));
			}
			std::cout << "  tresc: " << summary;
			output.site_text(summary);
			std::cout << std::endl;

			std::cout << "  obrazki: " << std::endl;
			std::copy(images.begin(), images.end(), std::ostream_iterator<std::string>(std::cout, "\n  "));
			output.site_images(images);
		} catch (const ekhem::entity_is_not_webpage& e) {
			output.not_a_webpage();
			std::cout << "  to nie jest text/html" << std::endl;
		}
	}
}

// brzydki-brzydki main
int main() try {
	namespace bf = boost::filesystem;

	ekhem::path::create_directories();

	Output output(ekhem::path::output);

	bf::directory_iterator end;
	for(bf::directory_iterator i(ekhem::path::sites); i!=end; ++i) {
		// FIXME: hidden files under windows
		if(i->leaf()[0]=='.') continue;

		std::cout << "Przetwarzanie '" << i->leaf() << "'..." << std::endl;
		output.site_begin(i->leaf());

		try {
			Site site(*i);
			for(Site::const_links_iterator i = site.links_begin(); i!=site.links_end(); ++i) {
				process_site(site, i->first, i->second, output);
			}

			site.save_links();

		} catch(const ekhem::configuration_not_found& e) {
			output.site_no_config();
			std::cout << "  nie znaleziono pliku konfiguracji '" << e.what() << "'" << std::endl;
		}

		output.site_end();
		std::cout << std::endl;
	}
} catch (const std::exception& e) {
	std::cout << "Wystapil blad, dzialanie programu zostanie zakonczone: " << std::endl;
	std::cout << "   " << e.what() << std::endl;
}
