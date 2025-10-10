// Locations listed by location ID
// Mismatched lists
// Smallest-smallest;second-second;...
// intrapair distance
// optimal sorting algo
#include "day1.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

namespace dayOne
{
	namespace {
		std::vector<int> firstElements;
		std::vector<int> secondElements;

		void handleLine(std::string& line) { // operates on the read value, destructive function
			std::stringstream ss(line);

			std::string stringPart;
			std::string delim = "   ";
			size_t delimLen = delim.length();

			size_t pos = line.find(delim); // unsigned long long

			while (pos != std::string::npos)
			{
				firstElements.push_back(std::stoi(line.substr(0, pos)));
				(void)line.erase(0, pos + delimLen);
				pos = line.find(delim);
			}
			secondElements.push_back(std::stoi(line));
		}
	}
	void dayOne() {
		std::cout << "Running program dayOne" << '\n';

		std::string line;
		std::ifstream inputFile("day1full.txt");

		if (inputFile.is_open()) 
		{
			while (getline(inputFile, line)) {
				handleLine(line);
			}

			std::sort(firstElements.begin(), firstElements.end()); // faster than any handwritten algorithm.

			std::sort(secondElements.begin(), secondElements.end()); 

			int diffTotal = 0;
			int lineCount = firstElements.size();

			for (int i = 0; i < lineCount; i++)
			{
				diffTotal += abs(firstElements[i] - secondElements[i]);
			}
			std::cout << diffTotal;
		}
		else std::cout << "Unable to open file";
	}
}