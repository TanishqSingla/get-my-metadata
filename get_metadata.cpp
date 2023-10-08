#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

struct kv {
	std::string key;
	std::string value;

	kv() {
		this->key = std::string("");
		this->value = std::string("");
	}
	kv(std::string k, std::string v) {
		this->value = v;
		
		this->key = "\"" + k + "\"";
	}
};

static std::vector<kv> pairs;

std::string trim(std::string& s) {
	std::string::iterator b_it = s.begin();
	std::string::iterator e_it = s.end();

	while(*b_it == ' ') {
		++b_it;
	}

	while(*e_it == ' ') {
		--e_it;
	}

	return std::string(b_it, e_it);
}

void parseLine(std::string& line) {
	std::string::iterator it = line.begin();

	while(it != line.end()) {
		if(*it == ':') {
			break;
		}
		++it;
	}
	auto key = std::string(line.begin(), it);
	++it;
	auto value = std::string(it, line.end());

	kv *result = new kv(trim(key), trim(value));

	pairs.push_back(*result);
}

const char* dumpJSON() {
	char json[1024] = "{";

	int it = 1;

	std::string entry;
	for(int i = 0; i < pairs.size(); i++) {
		entry = pairs[i].key + ":" + pairs[i].value;
		if(i < pairs.size() - 1) {
			entry += ",";
		}

		std::strcat(json, entry.c_str());
	}

	std::strcat(json, "}");

	json[strlen(json) + 1] = '\0';
	char* result = (char*)malloc(strlen(json));
	std::strcpy(result, json);
	return result;
}

extern "C" {
	const char* get_metadata(const char *filename) {
		if(!fs::exists(filename)) {
			char message[1024];
			std::sprintf(message, "Error: file does not exist %s", filename);

			const char* result = message;
			return result;
		}

		std::ifstream in(filename);
		std::string s;

		char flag = 0;

		while(getline(in, s)) {
			if(s == "---") {
				if(flag)
					break;

				flag = 1;
				continue;
			}

			parseLine(s);
		}

		return dumpJSON();
	}
}
