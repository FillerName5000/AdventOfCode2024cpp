#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <utility>

namespace DayThirteen
{
	namespace {
		using longg_t = long long;
		struct ButtonData { longg_t aX; longg_t aY; longg_t bX; longg_t bY; longg_t prizeX; longg_t prizeY; };
		ButtonData parseButtonSet(const std::string& buttonA, const std::string& buttonB, const std::string& prize);
		bool isAAdjustedHypotenuseLarger();
		bool isValidComboForEndCoord(const longg_t aCount, const longg_t bCount, const longg_t prizeX, const longg_t prizeY);
		bool isSingleButtonChoiceValid(const longg_t x, const longg_t y, const longg_t prizeX, const longg_t prizeY);
		longg_t calcTruncatedStartCount(const longg_t x, const longg_t y, const longg_t prizeX, const longg_t prizeY);
		longg_t calcTokensFromCounts(std::pair<longg_t, longg_t> counts);
		bool hasOverShotEndCoord(const longg_t aCount, const longg_t bCount, const longg_t prizeX, const longg_t prizeY);

		const bool toLog = true; // true   false
		const longg_t kAButtonCost = 3;
		const longg_t kBButtonCost = 1;
		const int kMaxIter = 1'000'000;

		ButtonData singleMachine; // Using globals is not advisable, used here to reduce excessive argument passing

		std::pair<longg_t, longg_t> calcMultiButtonCounts(const bool isAMoreEfficient) {
			longg_t fastCount = isAMoreEfficient
				? calcTruncatedStartCount(singleMachine.aX, singleMachine.aY, singleMachine.prizeX, singleMachine.prizeY) + 1
				: calcTruncatedStartCount(singleMachine.bX, singleMachine.bY, singleMachine.prizeX, singleMachine.prizeY) + 1;
			if (fastCount > 100) fastCount = 100;
			longg_t slowCount = 0;

			longg_t& aCount = isAMoreEfficient ? fastCount : slowCount;
			longg_t& bCount = isAMoreEfficient ? slowCount : fastCount;

			longg_t slowCountCopy;

			for (; fastCount >= 0; --fastCount)
			{
				if (isValidComboForEndCoord(aCount, bCount, singleMachine.prizeX, singleMachine.prizeY)) break;
				++slowCount;
				if (slowCount > 100) break;

				slowCountCopy = slowCount++;
				int iter = 0;
				while (!hasOverShotEndCoord(aCount, bCount, singleMachine.prizeX, singleMachine.prizeY)) {
					++iter;
					if (iter > kMaxIter) throw std::runtime_error("max iter breached");
					if (isValidComboForEndCoord(aCount, bCount, singleMachine.prizeX, singleMachine.prizeY)) {
						return std::make_pair(aCount, bCount);
					}
					++slowCount;
					if (slowCount > 100) break;
					if (toLog && slowCount == 100 && fastCount == 100) { 
						std::cout << "\nDouble 100's !\n"; }
				}
				slowCount = slowCountCopy;
			}
			if (slowCount > 100 || fastCount < 0) {
				if (toLog) std::cout << "Failed: " << (slowCount > 100 ? "SlowCount exceeded 100" : "FastCount went negative")
					<< " (SlowCount=" << slowCount << ", FastCount=" << fastCount << ")\n";
				return std::make_pair(-1, -1);
			}
			if (isValidComboForEndCoord(aCount, bCount, singleMachine.prizeX, singleMachine.prizeY)) return std::make_pair(aCount, bCount);
			return std::make_pair(-1, -1);
		}

		std::pair<longg_t, longg_t> getDependentCountsForMachine(const bool isAMoreEfficient) {
			if (isAMoreEfficient) { // start with A
				if (isSingleButtonChoiceValid(singleMachine.aX, singleMachine.aY, singleMachine.prizeX, singleMachine.prizeY)) {
					longg_t aCount = calcTruncatedStartCount(singleMachine.aX, singleMachine.aY, singleMachine.prizeX, singleMachine.prizeY);
					if (aCount < 101) return std::make_pair(aCount, 0);
				}
				return calcMultiButtonCounts(true);
			}
			else {
				if (isSingleButtonChoiceValid(singleMachine.bX, singleMachine.bY, singleMachine.prizeX, singleMachine.prizeY)) {
					longg_t bCount = calcTruncatedStartCount(singleMachine.bX, singleMachine.bY, singleMachine.prizeX, singleMachine.prizeY);
					if (bCount < 101) return std::make_pair(bCount, 0);
				}
				return calcMultiButtonCounts(false);
			}
		}

		longg_t getTotalTokensForMachine(const ButtonData singleMachine) {
			if (toLog) std::cout << "\nMachine: A(X=" << singleMachine.aX << ",Y=" << singleMachine.aY << ") B(X=" << singleMachine.bX << ",Y=" << singleMachine.bY << ") Prize(X=" << singleMachine.prizeX << ",Y=" << singleMachine.prizeY << ")\n";

			bool isAMoreEfficient = isAAdjustedHypotenuseLarger();

			if (toLog) std::cout << "Strategy: Using " << (isAMoreEfficient ? "A" : "B") << " as primary button\n";

			std::pair<longg_t, longg_t> counts = getDependentCountsForMachine(isAMoreEfficient);
			if (counts.first < 0) return 0;

			if (toLog) {
				if (counts.first < 0 || counts.second < 0) std::cout << "Result: No solution found\n";
				else std::cout << "Result: A=" << counts.first << " B=" << counts.second << " Cost=" << calcTokensFromCounts(counts) << "\n";
			}

			return calcTokensFromCounts(counts);
		}

		void handleFile(std::ifstream& inputFile)
		{
			if (inputFile.is_open()) {
				std::string buttonA, buttonB, prize;
				std::string line;
				int lineCount = 1;
				longg_t totalTokens = 0;
				while (getline(inputFile, line)) {
					++lineCount;
					if (line.empty()) {
						continue;
					}

					buttonA = line;
					getline(inputFile, buttonB);
					++lineCount;
					getline(inputFile, prize);
					++lineCount;

					singleMachine = parseButtonSet(buttonA, buttonB, prize);

					longg_t toAdd = getTotalTokensForMachine(singleMachine);

					if (toLog) std::cout << lineCount << ": Added " << toAdd << " to: " << totalTokens << " for (" << singleMachine.prizeX << ',' << singleMachine.prizeY << ")\n";

					totalTokens += toAdd;
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				std::cout << "Total tokens: " << totalTokens << '\n';

				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		longg_t calcTokensFromCounts(std::pair<longg_t, longg_t> counts) {
			return (counts.first * kAButtonCost) + (counts.second * kBButtonCost);
		}

		bool isSingleButtonChoiceValid(const longg_t x, const longg_t y, const longg_t prizeX, const longg_t prizeY) {
			return ((prizeX * prizeX) + (prizeY * prizeY)) % ((x * x) + (y * y)) == 0;
		}

		longg_t calcTruncatedStartCount(const longg_t x, const longg_t y, const longg_t prizeX, const longg_t prizeY) {
			return ((prizeX * prizeX) + (prizeY * prizeY)) / ((x * x) + (y * y));
		}

		bool hasOverShotEndCoord(const longg_t aCount, const longg_t bCount, const longg_t prizeX, const longg_t prizeY) {
			longg_t totalXMovement = (aCount * singleMachine.aX) + (bCount * singleMachine.bX);
			longg_t totalYMovement = (aCount * singleMachine.aY) + (bCount * singleMachine.bY);
			return prizeX < totalXMovement || prizeY < totalYMovement;
		}

		bool isValidComboForEndCoord(const longg_t aCount, const longg_t bCount, const longg_t prizeX, const longg_t prizeY) {
			longg_t totalXMovement = (aCount * singleMachine.aX) + (bCount * singleMachine.bX);
			longg_t totalYMovement = (aCount * singleMachine.aY) + (bCount * singleMachine.bY);
			return prizeX == totalXMovement && prizeY == totalYMovement;
		}

		bool isAAdjustedHypotenuseLarger() {
			longg_t aHypotenuseSquared = (singleMachine.aX * singleMachine.aX) + (singleMachine.aY * singleMachine.aY);
			longg_t bHypotenuseSquared = (singleMachine.bX * singleMachine.bX) + (singleMachine.bY * singleMachine.bY);

			return aHypotenuseSquared * kBButtonCost > bHypotenuseSquared * kAButtonCost;
		}

		ButtonData parseButtonSet(const std::string& buttonA, const std::string& buttonB, const std::string& prize) {
			ButtonData data{};
			std::regex numberPattern(R"([-+]?\d+)");
			std::smatch match;

			std::string temp = buttonA;
			std::regex_search(temp, match, numberPattern);
			data.aX = std::stoi(match[0]);
			temp = match.suffix();
			std::regex_search(temp, match, numberPattern);
			data.aY = std::stoi(match[0]);

			temp = buttonB;
			std::regex_search(temp, match, numberPattern);
			data.bX = std::stoi(match[0]);
			temp = match.suffix();
			std::regex_search(temp, match, numberPattern);
			data.bY = std::stoi(match[0]);

			temp = prize;
			std::regex_search(temp, match, numberPattern);
			data.prizeX = std::stoi(match[0]);
			temp = match.suffix();
			std::regex_search(temp, match, numberPattern);
			data.prizeY = std::stoi(match[0]);

			return data;
		}
	}
	void dayThirteen() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DayThirteen" << '\n';
		const bool isFullFile = true; // true   false

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("dayThirteenFull.txt") : inputFile.open("dayThirteenTest.txt");
		handleFile(inputFile);
	}
}
// 44022 too high
// 24338 too low
// 27665 too low
// 20442 for Hypotenuse method