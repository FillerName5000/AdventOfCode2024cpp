#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

namespace DayFivePartTwo
{
	namespace {
		void coutMapsContents();
		void coutVectorContents(std::string prefix, std::vector<std::string>& update);
		using rulemap_t = std::unordered_map<std::string, std::set<std::string>>; // int parsing not needed for part one, no logic for the numbers 

		const bool toLog = false; // true   false

		const char ruleDelim = '|';
		const char updateDelim = ',';
		const int numLen = 2;

		rulemap_t rulesPrecedingPages; // Key must appear after value in set
		rulemap_t rulesEnsuingPages; // Key must appear before value in set

		int getMiddlePageForIncorrectUpdate(std::vector<std::string> updateCopy)
		{
			size_t vectorLen = updateCopy.size();
			for (size_t i = 1; i < vectorLen; ++i) { // insertion sort
				std::string key = updateCopy[i];
				size_t j = i;
				while (j > 0 && rulesPrecedingPages[key].count(updateCopy[j - 1]) == 1) {
					updateCopy[j] = updateCopy[j - 1];
					--j;
				}
				updateCopy[j] = key;
			}

			for (size_t i = 0; i < vectorLen; ++i) {
				std::string numToCheck = updateCopy[i];
				for (size_t j = 0; j < vectorLen; ++j) {
					long long diff = static_cast<long long>(j) - static_cast<long long>(i); // unsigned underflow
					if (diff < 0) {
						if (rulesPrecedingPages[numToCheck].count(updateCopy.at(j)) == 1) { // set, count is 1 or 0
							coutVectorContents("Invalid sort: ", updateCopy);
							throw std::runtime_error::runtime_error("Sorting not succesful");
						}
					}
					else if (diff > 0) {
						if (rulesEnsuingPages[numToCheck].count(updateCopy.at(j)) == 1) {
							coutVectorContents("Invalid sort: ", updateCopy);
							throw std::runtime_error::runtime_error("Sorting not succesful");
						}
					}
					else {
						// do nothing
					}
				}
				if (toLog) coutVectorContents("Current State: ", updateCopy);
			}

			if (toLog) coutVectorContents("Sorted: ", updateCopy);

			return std::stoi(updateCopy[updateCopy.size() / 2]);
		}

		int getMiddlePageValue(std::string line)
		{
			const int numDist = numLen + 1;

			std::vector<std::string> update;
			size_t lineLen = line.length();

			for (size_t i = 0; i < lineLen; i += numDist) {
				if (i + 1 < lineLen) {
					update.push_back(line.substr(i, numLen));
				}
			}

			if (toLog) coutVectorContents("Testing: ", update);

			size_t vectorLen = update.size();
			if (vectorLen % 2 == 0) throw std::runtime_error::runtime_error("Even amount of update entries in a line, no middle");

			for (size_t i = 0; i < vectorLen; i++)
			{
				std::string numToCheck = update.at(i);
				for (size_t j = 0; j < vectorLen; j++)
				{
					long long diff = static_cast<long long>(j) - static_cast<long long>(i); // unsigned underflow
					if (diff < 0) {
						if (rulesPrecedingPages[numToCheck].count(update.at(j)) == 1) { // set, count is 1 or 0
							return getMiddlePageForIncorrectUpdate(update);
						}
					}
					else if (diff > 0) {
						if (rulesEnsuingPages[numToCheck].count(update.at(j)) == 1) {
							return getMiddlePageForIncorrectUpdate(update);
						}
					}
					else {
						// do nothing
					}
				}
			}

			return 0;
		}


		void handleRuleLine(std::string line)
		{
			size_t delimPos = line.find(ruleDelim);
			std::string firstPage = line.substr(0, numLen);
			std::string secondPage = line.substr(delimPos + 1, numLen);

		//	if (toLog) std::cout << firstPage << '-' << secondPage << '\n';

			rulesEnsuingPages[secondPage].insert(firstPage);
			rulesPrecedingPages[firstPage].insert(secondPage);
		}

		void handleAnyLine(std::string& line, int& middlePageTotal)
		{
			if (line.empty() || line[0] == '\n') {
				if (toLog) {
					coutMapsContents();
					std::cout << "---\nRules are defined.\n---\n";
				}
			}
			else {
				switch (line[2]) {
				case ruleDelim:
					handleRuleLine(line);
					break;
				case updateDelim:
					middlePageTotal += getMiddlePageValue(line);
					break;
				default:
					throw std::runtime_error::runtime_error("Invalid line parsing");
				}
			}
		}

		void handleFile(std::ifstream& inputFile)
		{
			int middlePageTotal = 0;
			if (inputFile.is_open()) {
				std::string line;
				while (getline(inputFile, line)) {
					handleAnyLine(line, middlePageTotal);
				}

				std::cout << "Total: " << middlePageTotal << '\n';

				std::cout << "Finished running program\n";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		void coutVectorContents(std::string prefix, std::vector<std::string>& update)
		{
			std::cout << prefix;
			for (const auto& num : update) {
				std::cout << num << ' ';
			}
			std::cout << '\n';
		}

		void coutMapsContents()
		{
			std::cout << "Preceding:\n";
			for (const auto& pair : rulesPrecedingPages) {
				std::cout << pair.first << ": ";
				for (const auto& value : pair.second) {
					std::cout << value << ' ';
				}
				std::cout << '\n';
			}

			std::cout << "Ensuing:\n";
			for (const auto& pair : rulesEnsuingPages) {
				std::cout << pair.first << ": ";
				for (const auto& value : pair.second) {
					std::cout << value << ' ';
				}
				std::cout << '\n';
			}
		}
	}
	void dayFivePartTwo() {
		std::cout << "Running program Day Five Part Two" << '\n'; // Safe inputs, two-digit numbers only
		const bool isFullFile = true; // true   false

		std::string line;
		std::ifstream inputFile;
		(isFullFile) ? inputFile.open("dayFiveFull.txt") : inputFile.open("dayFiveTest.txt");
		handleFile(inputFile);
	}
}