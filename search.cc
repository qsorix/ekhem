#include "search.h"
#include <iostream>
#include <buffio.h> // to jest z lib Tidy

std::string ekhem::get_node_text(TidyDoc& input_document, TidyNode input) {
	std::string ret;
	TidyNode child;

	for ( child = tidyGetChild(input); child; child = tidyGetNext(child) ) {
		if(tidyNodeIsText(child)) {
			TidyBuffer buffer = {0};
			tidyNodeGetText(input_document, child, &buffer);
			ret += reinterpret_cast<char*>(buffer.bp);
		}
		ret += get_node_text( input_document, child );
	}
	return ret;
}

void get_node_images(TidyDoc& input_document, TidyNode input, std::set<std::string>& images) {
	TidyNode child;

	for ( child = tidyGetChild(input); child; child = tidyGetNext(child) ) {
		if(tidyNodeIsIMG(child)) {
			if(tidyAttrGetSRC(child)) {
				images.insert(tidyAttrValue( tidyAttrGetSRC(child) ) );
			}
		}
		get_node_images( input_document, child, images);
	}
}

std::string find_difference(TidyDoc& input_document, TidyNode input_node, TidyNode pattern_node, std::set<std::string>& images) {
	std::string ret;

	if(!input_node || !pattern_node)
		return ret;

	TidyNode in_child = tidyGetChild(input_node);
	TidyNode pt_child = tidyGetChild(pattern_node);

	for(; in_child && pt_child; in_child = tidyGetNext(in_child), pt_child = tidyGetNext(pt_child) ) {
		if(tidyNodeIsDIV(pt_child) &&
		   tidyAttrGetTITLE(pt_child) &&
		   tidyAttrValue( tidyAttrGetTITLE(pt_child)) == std::string("ekhem-extract"))
		{
			in_child = tidyGetPrev(in_child); // FIXME: no idea why i have to do it :/
			while(in_child) {
				ret += ekhem::get_node_text(input_document, in_child);
				get_node_images(input_document, in_child, images);
				in_child = tidyGetNext(in_child);
			}
			break;
		}

		ret += find_difference(input_document, in_child, pt_child, images);
	}
	return ret;
}

std::string ekhem::extract_summary(TidyDoc& document, TidyNode pattern, std::set<std::string>& images) {
	images.clear();
	std::string ret = find_difference(document, tidyGetRoot(document), pattern, images);
	return ret;
}
