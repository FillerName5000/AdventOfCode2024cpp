#include <iostream>
#include <fstream>
#include <string>
#include <regex>

namespace DayThreePartTwo
{
	namespace {
		using std::string;

		const bool toLog = true;


		long handleLine(std::string line) { // operates on the read value, destructive function
			const std::regex kMulPattern(R"(mul\((\d{1,3}),(\d{1,3})\))");
			const int kMaxMulPatternLen = 12;

			const std::regex kDoPattern(R"(do\(\))");
			const std::regex kDontPattern(R"(don't\(\))");
			const int kLongestDPatternLen = 7;
			
			bool isMulEnabled = true;
			long total = 0;

			size_t pos = 0;
			while (pos < line.size()) {
				size_t mPos = line.find('m', pos);
				size_t dPos = line.find('d', pos);

				size_t nextPos = std::min(mPos, dPos);
				if (nextPos == std::string::npos) break;

				char foundChar = line[nextPos];
				switch (foundChar) {
				case 'm': {
					if (!isMulEnabled) break;
					std::string subLine = line.substr(nextPos, kMaxMulPatternLen);
					std::smatch match;
					if (std::regex_search(subLine, match, kMulPattern)) {
						if (toLog) std::cout << match[0].str();
						if (match.position(0) == 0) {
							int num1 = std::stol(match[1].str());
							int num2 = std::stol(match[2].str());

							total += num1 * num2;
							if (toLog) std::cout << ";" << total << ';';
						}
						else std::cout << "!!!!!!!!!!";
					}
					break;
				}
				case 'd': {
					std::string subLine = line.substr(nextPos, kLongestDPatternLen);
					std::smatch match;

					if (std::regex_search(subLine, match, kDoPattern) && match.position(0) == 0) {
						if (toLog) std::cout << "DO[]";
						isMulEnabled = true;
					}
					else if (std::regex_search(subLine, match, kDontPattern) && match.position(0) == 0) {
						if (toLog) std::cout << "DON'T[]";
						isMulEnabled = false;
					}
					break;
				}
				}
				pos = nextPos + 1;
			}
			std::cout << '\n' << "total: " << total << '\n';
			return total;
		}
	}
	void dayThreePartTwo() {
		std::cout << "Running program dayThree Part Two" << '\n';

		std::string line;
		std::ifstream inputFile("dayThreeFull.txt");

		long totalAllLines = 0;

		if (inputFile.is_open())
		{
			while (getline(inputFile, line)) {
				if (toLog) std::cout << '\n' << line << "\n\n";
				totalAllLines += handleLine(line);
			}

			std::cout << "Total: " << totalAllLines << '\n';
			std::cout << "Finished reading file\n";
		}
		else std::cout << "Unable to open file\n";
	}
}
// 79037122 too high
// 75532820 too low
// Solution: remove newlines and treat the dataset as a full line
// dayThreeTest
// dayThreeFull