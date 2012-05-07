#ifndef EKHEM_PAGE
#define EKHEM_PAGE

#include <tidy.h>

namespace boost {
	namespace filesystem {
		class path;
	}
}

class Entity;

/** Strona HTML.
 * Pobiera strone i udostępnia jej drzewo HTML.
 */
class Page {
	TidyDoc tree;
public:
	explicit Page(const Entity& entity);
	explicit Page(const boost::filesystem::path& path);
	~Page();

	const TidyNode root() const;
	TidyDoc& document();
};

#endif
