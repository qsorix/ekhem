#ifndef EKHEM_ENTITY_H
#define EKHEM_ENTITY_H

#include <curl/curl.h>
#include <string>
#include <vector>

/** Obiekt pobierany przez HTTP. */
class Entity {
	static const int curl_max_redirs = 5;
	static int init_lib_curl;

	CURL*             handle;
	const std::string url;

	char error_buffer[CURL_ERROR_SIZE];

	std::vector<char> input_buffer;

	static size_t write_function(void* ptr, size_t size, size_t nmemb, void* data);

	void dealloc();
	void throw_on_error(const std::string& where, CURLcode code) const;

public:
	Entity(const std::string& url);
	~Entity();

	std::string get_content_type() const;
	std::string get_url() const;
	std::string get_text() const;
	const std::vector<char>& get_data() const;
};

#endif
