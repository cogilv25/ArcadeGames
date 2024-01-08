#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>


	//Returns a shared_ptr to a char array containing the contents of the file at the path provided.
	//If no file is found or there is another error returns 0 and reports failure through std::cerr.
	std::shared_ptr<char> loadFileAsCharArray(const char* path)
	{
		std::string str;
		std::ifstream file;
		try
		{
			file.open(path);
			if (!file.good())
			{
				std::cerr << "Unable to find the requested file: " << path << "\n";
				return 0;
			}
			std::stringstream stream;
			stream << file.rdbuf();
			file.close();
			str = stream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cerr << "An error occured reading file: " << path << "\n";
			return 0;
		}

		std::shared_ptr<char> fileData(new char[str.size()+1]);
		memcpy(fileData.get(), str.c_str(), str.size());
		fileData.get()[str.size()] = 0;
		return fileData;
	}
