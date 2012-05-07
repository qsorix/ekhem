#ifndef EKHEM_SEARCH_H
#define EKHEM_SEARCH_H

#include <string>
#include <set>
#include <tidy.h> // to jest z lib Tidy

namespace ekhem {

std::string get_node_text(TidyDoc& input_document, TidyNode input);
std::string extract_summary(TidyDoc& document, TidyNode pattern, std::set<std::string>& images);

} // namespace

#endif
