#include <iostream>
#include <fstream>
#include <string>
#include <regex>

namespace DayThree
{
	namespace {
		using std::string;

		const bool toLog = false;

		string removeInvalidChars(const std::string& fullText)
		{
			const std::regex allowed("[mul(),0-9]");
			string filtered;
			for (char c : fullText) {
				if (std::regex_match(string(1, c), allowed)) {
					filtered += c;
				}
			}
			return filtered;
		}

		long handleLine(std::string fullText) { // operates on the read value, destructive function
			//string filteredText = removeInvalidChars(fullText);
			if (toLog) std::cout << fullText << "\n\n";

			std::regex mulPattern(R"(mul\((\d{1,3}),(\d{1,3})\))");
			long total = 0;

			for (std::sregex_iterator match(fullText.begin(), fullText.end(), mulPattern), end; match != end; ++match) {
				int num1 = std::stoi((*match)[1].str()); // Capturing group ((\d{1,3})
				int num2 = std::stoi((*match)[2].str());

				total += num1 * num2;
				std::cout << "match: " << (*match)[0].str() << '\n';
			}

			std::cout << total << "\n\n\n\n";
			return total;
		}
	}
	void dayThree() {
		std::cout << "Running program dayThree" << '\n';

		std::string line;
		std::ifstream inputFile("dayThreeFull.txt");

		long totalAllLines = 0;

		if (inputFile.is_open())
		{
			while (getline(inputFile, line)) {
				totalAllLines += handleLine(line);
			}

			std::cout << "Total: " << totalAllLines << '\n';
			std::cout << "Finished reading file\n";
		}
		else std::cout << "Unable to open file\n";
	}
}
