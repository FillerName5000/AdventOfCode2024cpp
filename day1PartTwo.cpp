#include "day1PartTwo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace DayOnePartTwo
{
	namespace {
		using cmap_return_val_t = std::unordered_map<int, int>::iterator;

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
	void dayOnePartTwo() {
		std::cout << "Running program dayOne Part Two" << '\n';

		std::string line;
		std::ifstream inputFile("day1full.txt");

		if (inputFile.is_open())
		{
			while (getline(inputFile, line)) {
				handleLine(line);
			}

			std::sort(firstElements.begin(), firstElements.end()); // faster than any handwritten algorithm.

			std::unordered_map<int, int> countMap;
			for (const int& num : secondElements) {
				++countMap[num];
			}

			int diffTotal = 0;
			size_t lineCount = firstElements.size();
			int idToCheck = 0;

			for (int i = 0; i < lineCount; i++)
			{
				idToCheck = firstElements[i];
				cmap_return_val_t foundPair = countMap.find(idToCheck); // _map[key] inserts values
				if (foundPair != countMap.end()) { // end value = not found
					diffTotal += idToCheck * (*foundPair).second;
				}
			}
			std::cout << diffTotal;
		}
		else std::cout << "Unable to open file";
	}
}