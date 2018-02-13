#ifndef FILE_H
#define FILE_H

#include <string>
#include <memory>
#include <cstdlib>

class File {
	std::string filename;
	std::shared_ptr<struct stat> file_status;
public:
	File();
	File(const std::string &filename);

	// APIs
	std::string file_suffix();
	std::string readall();

	size_t size();

	bool is_exists();
	bool is_file();
	bool is_directory();
	bool is_executable();

	const std::string &fullpath();
	std::string realpath();
};

using FilePtr = std::shared_ptr<File>;


class FileManager {
public:
	static bool is_start_with_directory(const std::string &filename);
	static FilePtr search_file(const std::string &filename);
};


#endif
