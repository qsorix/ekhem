#include "entity.h"
#include <stdexcept>
#include <cassert>
#include <boost/lexical_cast.hpp>

/** inicjalizacja curl'a.
 * @note nie wolam nigdzie curl_global_cleanup, bo koncze program po zabawie
 * z curlem
 */
int Entity::init_lib_curl = curl_global_init(CURL_GLOBAL_WIN32);

void Entity::throw_on_error(const std::string& where, CURLcode code) const {
	if(code != CURLE_OK) {
		if(code == CURLE_FAILED_INIT)
			throw std::runtime_error(where + ", code: CURL_ERROR_FAILED_INIT");
		throw std::runtime_error(where + ", code: " + boost::lexical_cast<std::string>(code)+"\n"+error_buffer);
	}
}

Entity::Entity(const std::string& url_): handle(NULL), url(url_) {
	error_buffer[0]='\x0';

	try {
		handle = curl_easy_init();
		if(!handle)
			throw std::runtime_error("Unable to create CURL handle");
		curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, error_buffer);
		
		throw_on_error("CURLOPT_WRITEFUNCTION", curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &Entity::write_function));
		throw_on_error("CURLOPT_WRITEDATA",     curl_easy_setopt(handle, CURLOPT_WRITEDATA, this));

		throw_on_error("CURLOPT_NOPROGRESS",    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1));
		throw_on_error("CURLOPT_AUTOREFERER",   curl_easy_setopt(handle, CURLOPT_AUTOREFERER, 1));
		throw_on_error("CURLOPT_FOLLOWLOCATION",curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1));
		throw_on_error("CURLOPT_MAXREDIRS",     curl_easy_setopt(handle, CURLOPT_MAXREDIRS, curl_max_redirs));

		throw_on_error("CURLOPT_URL",           curl_easy_setopt(handle, CURLOPT_URL, url.c_str()));

		// download data from url
		throw_on_error("curl_easy_perform",     curl_easy_perform(handle));

	} catch(...) {
		dealloc();
		throw;
	}
}

void Entity::dealloc() {
	if(handle) {
		curl_easy_cleanup(handle);
		handle=NULL;
	}
}

Entity::~Entity() {
	dealloc();
}

size_t Entity::write_function(void* ptr, size_t size, size_t nmemb, void* data) {
	assert(data && "Pointer to an Entity object was not given");

	Entity& entity = *static_cast<Entity*>(data);

	std::vector<char>& buffer = entity.input_buffer;
	char* in = static_cast<char*>(ptr);

	buffer.insert(buffer.end(), in, in+size*nmemb);
	return size*nmemb;
}

std::string Entity::get_text() const {
	return std::string(input_buffer.begin(), input_buffer.end());
}

std::string Entity::get_url() const {
	assert(handle);
	char* effective_url=NULL;
	throw_on_error( "CURLINFO_EFFECTIVE_URL", curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &effective_url) );
	return std::string(effective_url);
}

std::string Entity::get_content_type() const {
	assert(handle);
	char* content=NULL;
	throw_on_error( "CURLINFO_CONTENT_TYPE", curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &content) );
	return std::string(content);
}


const std::vector<char>& Entity::get_data() const {
	return input_buffer;
}
