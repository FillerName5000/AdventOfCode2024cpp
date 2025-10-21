#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

namespace DayFive
{
	namespace {
		void coutMapsContents();
		void coutVectorContents(std::vector<std::string>& update);
		using rulemap_t = std::unordered_map<std::string, std::set<std::string>>; // int parsing not needed for part one, no logic for the numbers 

		const bool toLog = false; // true   false

		const char ruleDelim = '|';
		const char updateDelim = ',';
		const int numLen = 2;

		rulemap_t rulesPrecedingPages; // Key must appear after value in set
		rulemap_t rulesEnsuingPages; // Key must appear before value in set

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

			if (toLog) coutVectorContents(update);

			size_t vectorLen = update.size();
			if (vectorLen % 2 == 0) throw std::runtime_error::runtime_error("Even amount of update entries in a line, no middle");

			for (size_t i = 0; i < vectorLen; i++)
			{
				std::string numToCheck = update.at(i);
				for (size_t j = 0; j < vectorLen; j++)
				{
					int diff = j - i;
					if (diff < 0) {
						if (rulesPrecedingPages[numToCheck].count(update.at(j)) == 1) { // set, count is 1 or 0
							if (toLog) std::cout << "i: " << update.at(i) << ", j: " << update.at(j) << '\n';
							return 0;
						}
					}
					else if (diff > 0) {
						if (rulesEnsuingPages[numToCheck].count(update.at(j)) == 1) {
							if (toLog) std::cout << "i: " << update.at(i) << ", j: " << update.at(j) << '\n';
							return 0;
						}
					}
					else {
						// do nothing
					}
				}
			}

			int parsedMiddlePageNum = std::stoi(update.at(vectorLen / 2));
			if (toLog) std::cout << parsedMiddlePageNum << '\n';
			return parsedMiddlePageNum;
		}


		void handleRuleLine(std::string line)
		{
			size_t delimPos = line.find(ruleDelim);
			std::string firstPage = line.substr(0, numLen);
			std::string secondPage = line.substr(delimPos + 1, numLen);

			if (toLog) std::cout << firstPage << '-' << secondPage << '\n';

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
				if (toLog) std::cout << "---\nRules are defined.\n---\n";
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

		void coutVectorContents(std::vector<std::string>& update)
		{
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
	void dayFive() {
		std::cout << "Running program Day Five" << '\n'; // Safe inputs, two-digit numbers only
		const bool isFullFile = false; // true   false

		std::string line;
		std::ifstream inputFile;
		(isFullFile) ? inputFile.open("dayFiveFull.txt") : inputFile.open("dayFiveTest.txt");
		handleFile(inputFile);
	}
}