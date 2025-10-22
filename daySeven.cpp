#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "BigInt.hpp" // https://github.com/faheel/BigInt
// GMP would be applicable for production-ready applications. This one-file solution works fine for these leetcode-style problems

namespace DaySeven
{
	namespace {
		void coutVectorContents(std::string prefix, std::vector<BigInt> vector);

		const bool toLog = false; // true   false

		bool recursiveOperationOrder(const std::vector<BigInt>& numbers, const BigInt operationRes, const size_t index, BigInt currentTotal, const size_t size, const bool isMultiplication) {
			if (index == size) {
				return currentTotal == operationRes;
			}

			BigInt nextValue = numbers[index];
			if (recursiveOperationOrder(numbers, operationRes, index + 1, currentTotal + nextValue, size, false)) {
				return true;
			}
			if (recursiveOperationOrder(numbers, operationRes, index + 1, currentTotal * nextValue, size, true)) {
				return true;
			}

			return false;
		}

		bool vectorVersionHasValidOperationOrder(const std::vector<BigInt>& numbers, const BigInt operationRes) {
			size_t size = numbers.size();
			return recursiveOperationOrder(numbers, operationRes, 1, numbers[0], size, false);
		}

		bool hasValidOperationOrder(const BigInt operationRes, const std::vector<BigInt>& numbers)
		{ // possible complicated optimization: start with all multiplication and quick sort to the possible combination
			// There's likely an existing paper/phd thesis/decades of research put into an existing library for this
			return vectorVersionHasValidOperationOrder(numbers, operationRes);

			return false;
		}

		BigInt getValidResult(std::string& line)
		{
			size_t operationResPos = line.find(':');
			BigInt operationRes = BigInt(line.substr(0, operationResPos));
			std::vector<BigInt> numbers;
			size_t start = operationResPos + 1;

			size_t lineLen = line.size();
			while (start < lineLen) {
				size_t end = line.find(' ', start);
				if (end == std::string::npos) end = lineLen;
				if (end > start) {
					numbers.push_back(BigInt(line.substr(start, end - start)));
				}
				start = end + 1;
			}
			if (toLog) std::cout << "Testing: " << operationRes;
			if (toLog) coutVectorContents("", numbers);
			if (hasValidOperationOrder(operationRes, numbers)) return operationRes;
		//	if (toLog) std::cout << "No valid operation order found\n";
			return 0; // invalid result
		}

		void handleFile(std::ifstream& inputFile)
		{
			if (inputFile.is_open()) {
				std::string line;
				BigInt total = 0;

				while (getline(inputFile, line)) {
					total += getValidResult(line);
					if (toLog) std::cout << "Total: " << total << "\n";
					if (!toLog) std::cout << '.';
				}

				std::cout << "\nTotal: " << total << '\n';

				std::cout << "Finished running program\n";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		void coutVectorContents(std::string prefix, std::vector<BigInt> vector)
		{
			std::cout << prefix;
			for (const auto& num : vector) {
				std::cout << num << ' ';
			}
			std::cout << '\n';
		}

	}
	void daySeven() {
		std::system("cls"); // clear terminal pre-printing
		std::cout << "Running program Day Seven" << '\n';
		const bool isFullFile = true; // true   false

		std::string line;
		std::ifstream inputFile;
		(isFullFile) ? inputFile.open("daySevenFull.txt") : inputFile.open("daySevenTest.txt");
		handleFile(inputFile);
	}
}