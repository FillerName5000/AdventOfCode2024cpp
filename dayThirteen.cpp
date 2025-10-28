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
		bool isAMovementMoreEfficientThanB(const longg_t aMovement, const longg_t bMovement);
		longg_t calcDirTokensSingleButton(const longg_t movement, const longg_t prize, const longg_t cost);

		const bool toLog = false; // true   false
		const longg_t kAButtonCost = 3;
		const longg_t kBButtonCost = 1;
		const int kMaxIter = 1'000'000;

		bool doesMovementWork(const longg_t otherOrthoAMove, const longg_t otherOrthoACount,
			const longg_t otherOrthoBMove, const longg_t otherOrthoBCount, const longg_t otherPrizeCoord) {
			return (otherOrthoAMove * otherOrthoACount) + (otherOrthoBMove * otherOrthoBCount) == otherPrizeCoord;
		}

		std::pair<longg_t, longg_t> getMovementsCount(const longg_t fastMovement, const longg_t slowMovement, const longg_t prize,
			const longg_t otherAMovement, const longg_t otherBMovement, const longg_t otherPrize, const bool isADirMoreEfficient) 
		{
			longg_t countFast = (prize / fastMovement) + 1;
			if (countFast > 100) {
				countFast = 101;
			}
			longg_t countSlow = 0;

			longg_t& aCount = isADirMoreEfficient ? countFast : countSlow;
			longg_t& bCount = isADirMoreEfficient ? countSlow : countFast;

			if (toLog) std::cout << "Starting calc: Fast movement: " << fastMovement << ", Slow movement: " << slowMovement << ", Target prize : " << prize << ", Initial countFast: " << countFast << "\n";

			for (countFast; countFast >= 0; countFast--) // quick sort or a similar algo would be faster. This project is focused on solving not hyperoptimizing for high throughput.
			{
				++countSlow;
				if (countSlow > 100) return std::make_pair(-1,-1);
				longg_t prizeAttempt = (fastMovement * countFast) + (slowMovement * countSlow);
					if (toLog) std::cout << "\nFast: " << countFast << ", Slow: " << countSlow << ", Attempt: " << prizeAttempt << ", Target: " << prize << ", Diff: " << (prizeAttempt - prize) << '\n';
				if (prizeAttempt == prize) {
					if (doesMovementWork(otherAMovement, aCount, otherBMovement, bCount, otherPrize)) break;
				}

				longg_t slowCountCopy = countSlow;
				longg_t diff = prizeAttempt - prize;
				slowCountCopy += (std::abs(diff) / slowMovement) - 1;
				int iter = 0;
				while (prizeAttempt < prize) {
					if (iter > kMaxIter) throw std::runtime_error("max iter breached");
					++slowCountCopy;
					prizeAttempt = (fastMovement * countFast) + (slowMovement * slowCountCopy);
					//	if (toLog) std::cout << "  " << slowCountCopy << "|" << prizeAttempt << "|" << (prizeAttempt - prize) << " ";
					++iter;
				}

				if (prizeAttempt == prize) {
					countSlow = slowCountCopy;
					if (doesMovementWork(otherAMovement, aCount, otherBMovement, bCount, otherPrize)) break;
				}
			}
			if (countFast == -1) countFast = 0; // post for loop offset, zero loop still needed
			if (countFast == 0 && ((countSlow * slowMovement) != prize)) return std::make_pair(-1, -1); // no valid combo
			return std::make_pair(countFast, countSlow);
		}

		longg_t calcTotalTokensForBothButtons() {

		}

		longg_t getDirectionTokensForMachine(const longg_t aMovement, const longg_t bMovement, const longg_t prizeForMovement, const bool isADirMoreEfficient,
			const longg_t otherAMovement, const longg_t otherBMovement, const longg_t otherPrize) {
			if (isADirMoreEfficient) { // start with A
				if (prizeForMovement % aMovement == 0) {
					return calcDirTokensSingleButton(aMovement, prizeForMovement, kAButtonCost);
				}
				auto movementsCount = getMovementsCount(aMovement, bMovement, prizeForMovement, otherAMovement, otherBMovement, otherPrize, isADirMoreEfficient);
				if (toLog) std::cout << "\n\nDir: A->B, Fast count: " << movementsCount.first << ", Slow count: " << movementsCount.second << ", Prize: " << prizeForMovement << '\n';
				if (movementsCount.first < 0) return 0;
				return (movementsCount.first * kAButtonCost) + (movementsCount.second * kBButtonCost);
			}
			else {
				if (prizeForMovement % bMovement == 0) {
					return calcDirTokensSingleButton(bMovement, prizeForMovement, kBButtonCost);
				}
				auto movementsCount = getMovementsCount(bMovement, aMovement, prizeForMovement, otherAMovement, otherBMovement, otherPrize, isADirMoreEfficient);
				if (toLog) std::cout << "\n\nDir: B->A, Fast count: " << movementsCount.first << ", Slow count: " << movementsCount.second << ", Prize: " << prizeForMovement << '\n';
				if (movementsCount.first < 0) return 0;
				return (movementsCount.first * kBButtonCost) + (movementsCount.second * kAButtonCost);
			}
		}

		longg_t calcalateTotalTokensForMachine(const ButtonData singleMachineData) {
			bool isAXMoreEfficient = isAMovementMoreEfficientThanB(singleMachineData.aX, singleMachineData.bX);
			bool isAYMoreEfficient = isAMovementMoreEfficientThanB(singleMachineData.aY, singleMachineData.bY);

			return getDirectionTokensForMachine(singleMachineData.aX, singleMachineData.bX, singleMachineData.prizeX,
				isAXMoreEfficient, singleMachineData.aY, singleMachineData.bY, singleMachineData.prizeY);
		}

		void handleFile(std::ifstream& inputFile)
		{
			if (inputFile.is_open()) {
				std::string buttonA, buttonB, prize;
				std::string line;
				longg_t totalTokens = 0;
				while (getline(inputFile, line)) {
					if (line.empty()) {
						continue;
					}

					buttonA = line;
					getline(inputFile, buttonB);
					getline(inputFile, prize);

					ButtonData singleMachineData = parseButtonSet(buttonA, buttonB, prize);

					longg_t toAdd = calcalateTotalTokensForMachine(singleMachineData);

					if (toLog) std::cout << "Added " << toAdd << " to: " << totalTokens << '\n';

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

		longg_t calcDirTokensSingleButton(const longg_t movement, const longg_t prize, const longg_t cost) {
			if (prize % movement != 0) throw std::runtime_error("Expecting single button");
			longg_t count = prize / movement;
			return count * cost;
		}

		bool isAMovementMoreEfficientThanB(const longg_t aMovement, const longg_t bMovement) {
			return aMovement * kBButtonCost > bMovement * kAButtonCost; // dodges inaccuracies from division conversions in case of very large numbers
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