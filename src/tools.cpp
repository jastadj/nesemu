#include "tools.h"

#include <sstream>
#include <filesystem>

std::vector<std::string> Tools::split(std::string str, char delimiter, bool allow_empty, bool encapsulate_quotes)
{
	std::vector<std::string> words;

	size_t quote_start_pos = str.find_first_of('"');
	size_t quote_next_pos = str.find_first_of('"', quote_start_pos + 1);
	size_t pos = str.find_first_of(delimiter);

	if (pos > quote_start_pos && pos < quote_next_pos)
	{
		pos = str.find_first_of(delimiter, quote_next_pos);
	}

	if (str.empty())
	{
		return words;
	}

	// While no more delimiter found
	while (pos != std::string::npos)
	{
		std::string new_word = str.substr(0, pos);

		if (!allow_empty)
		{
			if (!new_word.empty())
			{
				words.push_back(new_word);
			}
		}
		else
		{
			words.push_back(new_word);
		}
		str.erase(0, pos + 1);

		pos = str.find_first_of(delimiter);
		quote_start_pos = str.find_first_of('"');
		quote_next_pos = str.find_first_of('"', quote_start_pos + 1);
		if (pos > quote_start_pos && pos < quote_next_pos)
		{
			pos = str.find_first_of(delimiter, quote_next_pos);
		}
	}

	// add word to word list
	words.push_back(str);

	// strip quotes off the words beg/end
	for (int i = 0; i < words.size(); i++)
	{
		if (words[i].size() >= 2)
		{
			if (words[i].front() == '"' && words[i].back() == '"')
			{
				std::string sq = words[i];
				sq.erase(sq.size() - 1, 1);
				sq.erase(0, 1);
				words[i] = sq;
			}
		}
	}

	if (!allow_empty)
	{
		for (int i = words.size() - 1; i >= 0; i--)
		{
			if (words[i].empty())
			{
				words.erase(words.begin() + i);
			}
		}
	}

	return words;
}

std::string Tools::toLower(std::string str)
{
	for (auto& c : str)
	{
		c = std::tolower(c);
	}
	return str;
}

std::string Tools::toUpper(std::string str)
{
	for (auto& c : str)
	{
		c = std::toupper(c);
	}
	return str;
}

std::string Tools::getYesNo(bool y)
{
	return (y ? "Yes" : "No");
}

int Tools::toInt(std::string str)
{
	int val = 0;
	str = toLower(str);
	if (str.find("0x") == 0)
	{
		str = str.substr(2);
		sscanf_s(str.c_str(), "%x", &val);
	}
	else
	{
		val = atoi(str.c_str());
	}
	return val;
}

unsigned long long Tools::toUnsignedLongLong(std::string str)
{
	unsigned long long val = 0;
	str = toLower(str);
	if (str.find("0x") == 0)
	{
		unsigned int intval = 0;
		str = str.substr(2);
		sscanf_s(str.c_str(), "%x", &intval);
		return static_cast<unsigned long long>(intval);
	}
	else
	{
		char* end;
		val = strtoull(str.c_str(), &end, 10);
	}
	return val;
}

std::vector<std::string> Tools::getFilesAtPath(std::string path)
{
	std::vector<std::string> files;
	if (std::filesystem::is_directory(path))
	{
		std::filesystem::path p(path);
		std::filesystem::directory_iterator it(p);
		for (auto& file : it)
		{
			if (file.is_regular_file())
			{
				files.push_back(file.path().filename().string());
			}
		}
	}
	return files;
}

bool Tools::fileExists(std::string filepath)
{
	return std::filesystem::exists(filepath);
}

std::string Tools::getFilename(std::string filepath)
{
	std::filesystem::path fp(filepath);
	return fp.filename().string();
}

std::string Tools::getFileExtension(std::string filepath)
{
	std::filesystem::path fp(filepath);
	return fp.filename().extension().string();
}

std::string Tools::getFileDir(std::string filepath)
{
	if (std::filesystem::is_regular_file(filepath))
	{
		std::filesystem::path p(filepath);
		return p.parent_path().string();
	}
	return "";
}
