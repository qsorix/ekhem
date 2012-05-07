#ifndef EKHEM_EXCEPTIONS_H
#define EKHEM_EXCEPTIONS_H

#include <stdexcept>

namespace ekhem {

class entity_is_not_webpage : public std::runtime_error {
public:
	entity_is_not_webpage(const std::string& what) : std::runtime_error(what) {}
};

class configuration_not_found : public std::runtime_error {
public:
	configuration_not_found(const std::string& what) : std::runtime_error(what) {}
};

} // namespace

#endif
