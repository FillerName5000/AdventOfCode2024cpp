#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

namespace DayTwo
{
	namespace {
		using std::string;

		const int kMinDiff = 1; // google's constant naming convention is kPascalCase
		const int kMaxDiff = 3;
		const char kDelim = ' ';

		bool twoLevelsAreSafe(const int& parsedInt, const int& previousLevel, const bool& comparisonIsLargerThan)
		{
			int diff = parsedInt - previousLevel;
			if (abs(diff) < kMinDiff or abs(diff) > kMaxDiff)
			{
				// std::cout << "distance: " << previousLevel << '-' << parsedInt << std::endl;
				return false;
			}
			if (comparisonIsLargerThan)
			{
				if (diff < 0) {
					// std::cout << "comparison: " << previousLevel << '-' << parsedInt << std::endl;
					return false;
				};
			}
			else
			{
				if (diff > 0) {
					// std::cout << "comparison: " << previousLevel << '-' << parsedInt << std::endl;
					return false;
				}
			}
			return true;
		}

		bool handleLine(const std::string& line) { // actual source value is passed but cannot be edited
			std::stringstream ss(line);
			bool comparisonIsLargerThan = true;

			string stringPart;
			if (getline(ss, stringPart, kDelim)) {
				int previousLevel = std::stoi(stringPart);
				if (getline(ss, stringPart, kDelim)) 
				{
					int parsedInt = std::stoi(stringPart);
					comparisonIsLargerThan = (parsedInt - previousLevel) > 0;
					if (!twoLevelsAreSafe(parsedInt, previousLevel, comparisonIsLargerThan)) return false;
					previousLevel = parsedInt;
				}
				while (getline(ss, stringPart, kDelim)) {
					int parsedInt = std::stoi(stringPart);
					if (!twoLevelsAreSafe(parsedInt, previousLevel, comparisonIsLargerThan)) return false;
					previousLevel = parsedInt;
				}
			}
			return true;
		}
	}
	void dayTwo() {
		std::cout << "Running program dayTwo" << '\n';

		std::string line;
		std::ifstream inputFile("dayTwoFull.txt");

		if (inputFile.is_open())
		{
			int safeLines = 0;
			while (getline(inputFile, line)) {
				if (handleLine(line)) {
					safeLines++;
			//		std::cout << line << '\n';
				};
			}

			std::cout << "Safe lines:" << safeLines << '\n';

			std::cout << "done reading file\n";
		}
		else std::cout << "Unable to open file";
	}
}