/*
 * Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>
#include "stdint.h"
#include "AlloyCommon.h"
#include "AlloyFileUtil.h"
#if defined(WIN32) || defined(_WIN32)

#include <windows.h>
#include <codecvt>
#define WINDOWS
#else
#include <dirent.h>
#endif


#include <stdexcept>
using namespace std;

namespace aly {
std::string CodePointToUTF8(int cp) {
	int n = 0;
	if (cp < 0x80)
		n = 1;
	else if (cp < 0x800)
		n = 2;
	else if (cp < 0x10000)
		n = 3;
	else if (cp < 0x200000)
		n = 4;
	else if (cp < 0x4000000)
		n = 5;
	else if (cp <= 0x7fffffff)
		n = 6;
	char str[7];
	str[n] = '\0';
	switch (n) {
	case 6:
		str[5] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0x4000000;
	case 5:
		str[4] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0x200000;
	case 4:
		str[3] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0x10000;
	case 3:
		str[2] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0x800;
	case 2:
		str[1] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0xc0;
	case 1:
		str[0] = cp;
	}
	return std::string(str);
}

std::string ReadTextFile(const std::string& str) {
	std::ifstream myfile;
	myfile.open(str);
	if (myfile.is_open()) {
		std::stringstream buffer;
		std::string line;
		while (getline(myfile, line)) {
			buffer << line << std::endl;
		}
		return buffer.str();
	} else {
		throw runtime_error(MakeString() << "Could not open " << str);
	}
}
std::vector<char> ReadBinaryFile(const std::string& str) {
	streampos size;
	ifstream file(str, ios::in | ios::binary | ios::ate);
	if (file.is_open()) {
		size = file.tellg();
		std::vector<char> memblock(size);
		file.seekg(0, ios::beg);
		file.read(memblock.data(), size);
		file.close();
		return memblock;
	} else
		throw runtime_error(MakeString() << "Could not open " << str);
}
bool FileExists(const std::string& name) {
	std::ifstream file(name, ios::in | ios::binary);
	if (file.is_open()) {
		file.close();
		return true;
	}
	return false;
}

std::string GetFileExtension(const std::string& fileName) {
	if (fileName.find_last_of(".") != string::npos)
		return fileName.substr(fileName.find_last_of(".") + 1);
	return string("");
}

std::string GetFileName(const std::string& fileName) {
	if (fileName.find_last_of(PATH_SEPARATOR) != string::npos)return fileName.substr(fileName.find_last_of(PATH_SEPARATOR) + 1);
	return fileName;
}
std::string RemoveTrailingSlash(const std::string& file) {
	if (file.find_last_of(PATH_SEPARATOR) != string::npos) {
		size_t end =file.find_last_of(PATH_SEPARATOR);
		return file.substr(0, end);
	}
	return file;
}
std::string ConcatPath(const std::string& dir, const std::string& file) {
	return RemoveTrailingSlash(dir) + PATH_SEPARATOR+file;
}
std::string GetFileNameWithoutExtension(const std::string& fileName) {
	if (fileName.find_last_of(".") != string::npos
			&& fileName.find_last_of(PATH_SEPARATOR) != string::npos) {
		size_t start = fileName.find_last_of(PATH_SEPARATOR) + 1;
		size_t end = fileName.find_last_of(".");
		return fileName.substr(start, end - start);
	}
	return fileName;
}

std::string GetFileWithoutExtension(const std::string& fileName) {
	if (fileName.find_last_of(".") != string::npos) {
		size_t end = fileName.find_last_of(".");
		return fileName.substr(0, end);
	}
	return fileName;
}
std::string ReplaceFileExtension(const std::string& file,
		const std::string& ext) {
	return GetFileWithoutExtension(file) + "." + ext;
}
std::string GetFileDirectoryPath(const std::string& fileName) {
	if (fileName.find_last_of(PATH_SEPARATOR) != string::npos) {
		size_t end = fileName.find_last_of(PATH_SEPARATOR);
		return fileName.substr(0, end);
	}
	return fileName;
}

#ifndef WINDOWS
//Only works on Linux for NOW!
std::vector<std::string> GetDirectoryListing(const std::string& dirName,
		const std::string& ext, const std::string& mask) {
	std::vector<std::string> files;
	dirent* dp;
	std::string cleanPath = RemoveTrailingSlash(dirName) + PATH_SEPARATOR;
	DIR* dirp = opendir(cleanPath.c_str());
	while ((dp = readdir(dirp)) != NULL) {
		string fileName(dp->d_name);
		if (dp->d_type == DT_REG) {
			if (ext.length() == 0 || GetFileExtension(fileName) == ext) {
				if (mask.length() == 0
						|| (mask.length() > 0
								&& fileName.find(mask) < fileName.length()))
					files.push_back(cleanPath + fileName);
			}
		}
	}
	closedir(dirp);
	std::sort(files.begin(), files.end());
	return files;
}
#else 

std::wstring ToWString(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(str);
	return wide;
}

std::string ToString(const std::wstring& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string narrow = converter.to_bytes(str);
	return narrow;
}

std::vector<std::string> GetDirectoryListing(const std::string& dirName,const std::string& ext, const std::string& mask) {
	WIN32_FIND_DATAW fd;
	std::string path = RemoveTrailingSlash(dirName);
	std::wstring query = ToWString(path + PATH_SEPARATOR+string("*"));
	HANDLE h = FindFirstFileW(query.c_str(), &fd);
	if (h == INVALID_HANDLE_VALUE) {
		return std::vector<std::string>();
	}
	std::vector<std::string> list;
	do {
		std::string fileName = ToString(fd.cFileName);
		if (fileName != "." && fileName != "..")
		{
			if (ext.length() == 0 || GetFileExtension(fileName) == ext) {
				if (mask.length() == 0|| (mask.length() > 0&& fileName.find(mask) < fileName.length())) {
					if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) list.push_back(path + PATH_SEPARATOR + fileName);
				}
			}
		}
	}while (FindNextFile(h, &fd));
	FindClose(h);
	return list;
}
#endif


}

