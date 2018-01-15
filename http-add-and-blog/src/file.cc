#include "file.h"
#include <cstdlib>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <fstream>


File::File() :
	filename(),
	file_status()
{
}

File::File(const std::string &filename) :
	filename(filename),
	file_status(new struct stat)
{
	if(lstat(filename.c_str(), file_status.get()) < 0)
		file_status.reset();
}

std::string File::file_suffix() {
	auto pos = filename.find_last_of("/.");
	if(pos == filename.npos || filename[pos] == '/')
		return "";
	return filename.substr(pos + 1);
}

std::string File::readall() {
	if(!is_exists() || !is_file()) return "";
	std::ifstream ifs(filename);
	std::ostringstream oss;
	while((ifs.peek(), ifs.good())) {
		oss.put(ifs.get());
	}
	return oss.str();
}

size_t File::size() {
	return file_status->st_size;
}

const std::string &File::fullpath() {
	return filename;
}

bool File::is_exists() {
	return file_status.get() != nullptr;
}

bool File::is_file() {
	// a regular file
	return is_exists() && S_ISREG(file_status->st_mode);
}

bool File::is_directory() {
	return is_exists() && S_ISDIR(file_status->st_mode);
}

bool File::is_executable() {
	return is_exists() && access(filename.c_str(), X_OK) == 0;
}


bool FileManager::is_start_with_directory(const std::string &filename) {
	for(auto ch : filename)
		if(ch == '/')
			return true;
	return false;
}

FilePtr FileManager::search_file(const std::string &filename) {
	auto fileinfo = FilePtr(new File(filename));
	if(fileinfo->is_exists())
		return fileinfo;

	// search file in path environ variable
	char *pathenv = getenv("PATH");
	std::istringstream iss(pathenv);
	
	std::string path;
	while(std::getline(iss, path, ':')) {
		std::string fullpath = path  + "/" + filename;
		auto fileinfo = FilePtr(new File(fullpath));
		if(fileinfo->is_exists())
			return fileinfo;
	}

	return FilePtr(new File());
}

