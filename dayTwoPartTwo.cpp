#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

// Below this code is an attempt at completing this challenge in a single read of the line without array-like structures.
// It only seemed to work for popping n elements, not n+1 or n-1.

namespace DayTwoPartTwo
{
	namespace {
		using std::string; // This program is not advanced enough to use non-standard strings. I won't use using namespace std

		const bool toLog = false;
		const int kMinDiff = 1; // google's constant naming convention is kPascalCase
		const int kMaxDiff = 3;
		const char kDelim = ' ';

		bool twoLevelsAreSafe(const int parsedEntry, const int previousLevel, const bool comparisonIsLargerThan)
		{
			int diff = parsedEntry - previousLevel;
			if (abs(diff) < kMinDiff || abs(diff) > kMaxDiff)
			{
				if (toLog) std::cout << parsedEntry << '-' << previousLevel << ": F Both distance\n";
				return false;
			}

			if (toLog) std::cout << "Comparison: " << parsedEntry << '-' << previousLevel << " for: " << comparisonIsLargerThan << "\n";
			if (comparisonIsLargerThan)
			{
				if (diff < 0) {
					if (toLog) std::cout << parsedEntry << '-' << previousLevel << ": F Both comparison < 0\n";
					return false;
				};
			}
			else
			{
				if (diff > 0) {
					if (toLog) std::cout << parsedEntry << '-' << previousLevel << ": F Both comparison > 0\n";
					return false;
				}
			}
			if (toLog) std::cout << parsedEntry << "-" << previousLevel << ": Is Valid\n";
			return true;
		}

		bool isDampenedRunValid(std::vector<int> entriesCopy, size_t i)
		{
			if (toLog) std::cout << "__Damper run for index: " << i << "\n";
			if (toLog) std::cout << "__ index: " << i << "has value: " << entriesCopy[i] << "\n";
			entriesCopy.erase(entriesCopy.begin() + (i));
			size_t size = entriesCopy.size();

			bool comparisonIsLargerThan = entriesCopy[1] - entriesCopy[0] > 0;

			for (size_t i = 1; i < size; ++i) {
				if (!twoLevelsAreSafe(entriesCopy[i], entriesCopy[i - 1], comparisonIsLargerThan)) return false;
			}
			return true;
		}

		bool isAnyDampenedRunValid(const std::vector<int> entries, size_t i)
		{
			if (entries.size() < 4) throw std::runtime_error::runtime_error("Test set contains invalid entries");

			if (i == 2) if (isDampenedRunValid(entries, 0)) return true;
			if (isDampenedRunValid(entries, i - 1)) return true;
			if (isDampenedRunValid(entries, i)) return true;
			if (i + 1 < entries.size())
			{
				if (isDampenedRunValid(entries, i + 1)) return true;
			}
			return false;
		}

		bool handleLine(const std::string& line) { // actual source value is passed but cannot be edited
			if (toLog) std::cout << "----\n>" << line << "\n";

			std::vector<int> entries;
			bool potentialInvalidEntryHasBeenFound = false;
			size_t i = 1;

			std::stringstream ss(line); // every getline removes that entryToParse based on the delim
			bool comparisonIsLargerThan;  // default value is false but not meant to be utilized pre-explicit definition
			string entryToParse;
			if (getline(ss, entryToParse, kDelim)) {
				int previousLevel = std::stoi(entryToParse);
				entries.push_back(previousLevel);
				if (getline(ss, entryToParse, kDelim))
				{
					int secondEntry = std::stoi(entryToParse);
					entries.push_back(secondEntry);

					int diff = secondEntry - previousLevel;
					comparisonIsLargerThan = diff > 0;
					if (abs(diff) < kMinDiff || abs(diff) > kMaxDiff)
					{
						if (toLog) std::cout << "Second entry invalid\n";
						potentialInvalidEntryHasBeenFound = true;
					}
					else if (toLog) std::cout << "Second entry is valid\n";

					previousLevel = secondEntry;
				}
				else throw std::runtime_error::runtime_error("Failed getline parsing");

				while (getline(ss, entryToParse, kDelim)) {
					int parsedEntry = std::stoi(entryToParse);
					entries.push_back(parsedEntry);
					if (!potentialInvalidEntryHasBeenFound) i++;
					if (!twoLevelsAreSafe(parsedEntry, previousLevel, comparisonIsLargerThan)) potentialInvalidEntryHasBeenFound = true;
					previousLevel = parsedEntry;
				}

				if (!potentialInvalidEntryHasBeenFound) return true;
				if (isAnyDampenedRunValid(entries, i)) return true;
				return false;
			}
			else throw std::runtime_error::runtime_error("Failed getline parsing"); // Program should not be fail - tolerant

			return true;
		}
	}

	void dayTwoPartTwo() {
		std::cout << "Running program dayTwoPartTwo" << '\n';

		std::string line;
		std::ifstream inputFile("dayTwoFull.txt");

		if (inputFile.is_open())
		{
			int safeLines = 0;
			while (getline(inputFile, line)) {
				if (handleLine(line)) {
					safeLines++;
					//std::cout << "safe entry " << safeLines << ": " << line << "\n";
				}
				else std::cout << line << '\n';
			}

			std::cout << "Safe lines:" << safeLines << '\n';

			std::cout << "done reading file\n";
		}
		else std::cout << "Unable to open file";
	}

	// dayTwoTest
	// dayTwoFull

	// Below code was for a self-imposed challenge: no double runthrough. 
	// Easier algo above has multiple runs per line: one to find the wrong one, pop it, then run through the full line and check for validity.
	// Does not accomodate n+1 and n-1 checking, only popping element n.

	//namespace {
	//	using std::string;

	//	const bool toLog = true;

	//	const int kMinDiff = 1; // google's constant naming convention is kPascalCase
	//	const int kMaxDiff = 3;
	//	const char kDelim = ' ';

	//	void updateVariablesForValidLevels(const int parsedEntry, int& previousLevel, int& secondLastLevel)
	//	{
	//		secondLastLevel = previousLevel;
	//		previousLevel = parsedEntry;
	//		if (toLog) std::cout << "NormlUpdate: S" << secondLastLevel << "|P" << previousLevel << "|\n\n";
	//	}

	//	bool twoLevelsAreSafe(const int parsedEntry, const int previousLevel)
	//	{
	//		int diff = parsedEntry - previousLevel;
	//		if (abs(diff) < kMinDiff || abs(diff) > kMaxDiff)
	//		{
	//			if (toLog) std::cout << parsedEntry << '-' << previousLevel << ": F Only distance\n";
	//			return false;
	//		}
	//		return true;
	//	}

	//	bool doesLoopHaveToContinue(bool* hasDampenerBeenUsed, int& parsedEntryToUndo, const int previousLevel) {
	//		if (hasDampenerBeenUsed == nullptr) throw std::runtime_error::runtime_error("hasDampenerBeenUsed NullpointerException");
	//		if (!*hasDampenerBeenUsed)
	//		{
	//			if (toLog) std::cout << "Used up dampener, " << parsedEntryToUndo << " is now " << previousLevel << "\n";
	//			parsedEntryToUndo = previousLevel;
	//			*hasDampenerBeenUsed = true;
	//			return true;
	//		}
	//		if (toLog) std::cout << "No dampener available\n";
	//		return false;
	//	}

	//	bool twoLevelsAreSafe(int& parsedEntry, const int previousLevel, const bool* comparisonIsLargerThan, bool* hasDampenerBeenUsed)
	//	{
	//		if (toLog) std::cout << "AreSafe? " << "P" << previousLevel << "|E" << parsedEntry << "|\n";
	//		int diff = parsedEntry - previousLevel;
	//		if (abs(diff) < kMinDiff || abs(diff) > kMaxDiff)
	//		{
	//			if (toLog) std::cout << parsedEntry << '-' << previousLevel << ": F Both distance\n";
	//			return doesLoopHaveToContinue(hasDampenerBeenUsed, parsedEntry, previousLevel);
	//		}
	//		if (comparisonIsLargerThan != nullptr)
	//		{
	//			if (toLog) std::cout << "Comparison: " << parsedEntry << '-' << previousLevel << " for: " << *comparisonIsLargerThan << "\n";
	//			if (*comparisonIsLargerThan)
	//			{
	//				if (diff < 0) {
	//					if (toLog) std::cout << parsedEntry << '-' << previousLevel << ": F Both comparison < 0\n";
	//					return doesLoopHaveToContinue(hasDampenerBeenUsed, parsedEntry, previousLevel);
	//				};
	//			}
	//			else
	//			{
	//				if (diff > 0) {
	//					if (toLog) std::cout << parsedEntry << '-' << previousLevel << ": F Both comparison > 0\n";
	//					return doesLoopHaveToContinue(hasDampenerBeenUsed, parsedEntry, previousLevel);
	//				}
	//			}
	//		}
	//		else throw std::runtime_error::runtime_error("comparisonIsLargerThan NullpointerException");
	//		return true;
	//	}

	//	void updateVariablesForValidFirstLevels(bool& comparisonIsLargerThan, const int parsedEntry, int& previousLevel, int& secondLastLevel)
	//	{
	//		comparisonIsLargerThan = (parsedEntry - previousLevel) > 0;
	//		secondLastLevel = previousLevel;
	//		previousLevel = parsedEntry;
	//		if (toLog) std::cout << "FirstUpdate: S" << secondLastLevel << "|P" << previousLevel << "|E" << parsedEntry << "\n";
	//	}

	//	/// <summary>
	//	/// Pre check before full loop can go
	//	/// </summary>
	//	bool areFirstEntriesValid(std::stringstream& ss, std::string& entryToParse, int& previousLevel, int& secondLastLevel,
	//		bool& comparisonIsLargerThan, bool& hasDampenerBeenUsed)
	//	{
	//		if (getline(ss, entryToParse, kDelim)) // first check with entry 1 & 2 
	//		{
	//			int parsedEntry = std::stoi(entryToParse);
	//			if (twoLevelsAreSafe(previousLevel, parsedEntry)) {
	//				updateVariablesForValidFirstLevels(comparisonIsLargerThan, parsedEntry, previousLevel, secondLastLevel);
	//				return true;
	//			}
	//			else if (getline(ss, entryToParse, kDelim)) {
	//				hasDampenerBeenUsed = true;
	//				if (toLog) std::cout << "Second entry invalid\n";

	//				secondLastLevel = previousLevel;
	//				parsedEntry = std::stoi(entryToParse);
	//				if (toLog) std::cout << "&&S" << secondLastLevel << "|P" << previousLevel << "|E" << parsedEntry << "|\n";
	//				if (twoLevelsAreSafe(secondLastLevel, parsedEntry)) { //entry 1 & 3
	//					comparisonIsLargerThan = (parsedEntry - previousLevel) > 0;
	//					return true;
	//				}
	//				else {
	//					if (toLog) std::cout << "Second & Third entry invalid\n";
	//					return false;
	//				}
	//			}
	//			else throw std::runtime_error("Failed getline parsing");
	//		}
	//		else throw std::runtime_error::runtime_error("Failed getline parsing"); // Program should not be fail-tolerant
	//	}

	//	bool handleLine(const std::string& line) { // actual source value is passed but cannot be edited
	//		if (toLog) std::cout << "----\n>" << line << "\n";

	//		std::stringstream ss(line);
	//		bool comparisonIsLargerThan; // default value is false but not meant to be utilized pre-explicit definition

	//		bool hasDampenerBeenUsed = false;
	//		int secondLastLevel = 0;

	//		string entryToParse;
	//		if (getline(ss, entryToParse, kDelim)) { // every getline removes that entryToParse based on the delim
	//			int previousLevel = std::stoi(entryToParse);
	//			if (!areFirstEntriesValid(ss, entryToParse, previousLevel, secondLastLevel, comparisonIsLargerThan, hasDampenerBeenUsed)) return false;

	//			while (getline(ss, entryToParse, kDelim)) {
	//				int parsedEntry = std::stoi(entryToParse);
	//				if (!twoLevelsAreSafe(parsedEntry, previousLevel, &comparisonIsLargerThan, &hasDampenerBeenUsed)) return false;
	//				if (toLog) std::cout << "Pre update: S" << secondLastLevel << "|P" << previousLevel << "|E" << parsedEntry << "\n";
	//				updateVariablesForValidLevels(parsedEntry, previousLevel, secondLastLevel);
	//			}
	//		}
	//		else throw std::runtime_error::runtime_error("Failed getline parsing"); // Program should not be fail-tolerant

	//		return true;
	//	}
	//}
	//void dayTwoPartTwo() {
	//	std::cout << "Running program dayTwoPartTwo" << '\n';

	//	std::string line;
	//	std::ifstream inputFile("dayTwoTest.txt");

	//	if (inputFile.is_open())
	//	{
	//		int safeLines = 0;
	//		while (getline(inputFile, line)) {
	//			if (handleLine(line)) {
	//				safeLines++;
	//				if (toLog) std::cout << "safe entry " << safeLines << ": " << line << "\n";
	//			}
	//			else std::cout << "unsafe: " << line << '\n';
	//		}

	//		std::cout << "Safe lines:" << safeLines << '\n';

	//		std::cout << "done reading file\n";
	//	}
	//	else std::cout << "Unable to open file";
	//}
}

// dayTwoTest
// dayTwoFull
