// COMMENTED OUT CODE DISCLAIMER
// To ensure compilation for the other files with the BigInt.hpp or any other external header files, this entire file is commented out. 
// In the current structure that only uses one file at a time, header files are not perfectly organised.
// Having many files in a project you have no intention of using is not a normal use case, therefore organising this perfectly is not a priority.
// To test this file, get the BigInt hpp file from github and add it to the Header Files, in my case in a "libs" filter. 
// Comment out any other cpp files using it. Only the .cpp files should need to be edited.
// /COMMENTED OUT CODE DISCLAIMER
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <array>
#include <vector>
#include "BigInt.hpp" // https://github.com/faheel/BigInt

namespace DayFourteen
{
	// [0,0]
	//       [xLen-1,yLen-1]
	namespace {
		using coord_t = std::pair<BigInt, BigInt>;
		void handleLine(std::string line);
		void coutVectorCoordsAsGrid(const std::vector<coord_t>& coords, BigInt xLen, BigInt yLen);

		struct StartState {
			BigInt px, py, vx, vy;
		};

		const bool toLog = false; // true   false
		const BigInt kXLenFull = 101; // Using Bigint for everything to avoid any conversion errors in case it is used with other BigInts
		const BigInt kXLenTest = 11; // performance gains from using smaller int value types on most modern system is negligable
		const BigInt kYLenFull = 103;
		const BigInt kYLenTest = 7;
		const BigInt kIterations = 100; // part two is going to be a quadrillion iterations
		const size_t kDivisions = 4;

		std::vector<StartState> startStates;
		std::vector<coord_t> endCoords;
		std::array<BigInt, kDivisions> quadrantsCount = { 0, 0, 0, 0 };

		BigInt calcTotalSafetyFactorFromQuadrantsCount()
		{
			BigInt total = 1;

			for (const auto& count : quadrantsCount)
			{
				total *= count;
			}

			return total;
		}

		void incrementQuadrantsCount(coord_t endCoord, BigInt xLen, BigInt yLen)
		{
			if (endCoord.second < yLen / 2) {
				if (endCoord.first < xLen / 2)
					quadrantsCount[0]++; // top left
				else
					quadrantsCount[1]++; // top right
			}
			else {
				if (endCoord.first < xLen / 2)
					quadrantsCount[2]++; // bottom left
				else
					quadrantsCount[3]++; // bottom right
			}
		}

		bool isValidEndCoord(coord_t endCoord, BigInt xLen, BigInt yLen)
		{
			if (xLen % 2 == 0 || yLen % 2 == 0) {
				throw std::runtime_error("Length cannot be even");
			}

			BigInt xHalf = (xLen - 1) / 2;
			BigInt yHalf = (yLen - 1) / 2;

			return endCoord.first != xHalf && endCoord.second != yHalf;
		}

		BigInt calcEndDirCoord(BigInt pDir, BigInt vDir, BigInt dirLen)
		{
			BigInt endState = pDir + (vDir * kIterations);
			return ((endState % dirLen) + dirLen) % dirLen;
		}

		coord_t calcEndCoord(StartState startState, BigInt xLen, BigInt yLen)
		{
			BigInt endXCoord = calcEndDirCoord(startState.px, startState.vx, xLen);
			BigInt endYCoord = calcEndDirCoord(startState.py, startState.vy, yLen);

			return std::make_pair(endXCoord, endYCoord);
		}

		BigInt getTotalSafetyFactor(BigInt xLen, BigInt yLen) {
			for (const auto& startState : startStates)
			{
				coord_t endCoord = calcEndCoord(startState, xLen, yLen);
				endCoords.push_back(endCoord);
			}

			if (toLog) std::cout << "size: " << endCoords.size() << '\n';

			for (const auto& endCoord : endCoords)
			{
				if (toLog) {
					std::cout << "End coord: [" << endCoord.first << "," << endCoord.second << "]";
				}
				if (isValidEndCoord(endCoord, xLen, yLen)) {
					incrementQuadrantsCount(endCoord, xLen, yLen);
					if (toLog) std::cout << " is valid\n";
				}
				else {
					if (toLog) std::cout << " is not valid\n";
				}
			}
			if (toLog) coutVectorCoordsAsGrid(endCoords, xLen, yLen);

			return calcTotalSafetyFactorFromQuadrantsCount();
		}

		void handleFile(std::ifstream& inputFile, BigInt xLen, BigInt yLen)
		{
			if (inputFile.is_open()) {
				std::string line;
				while (getline(inputFile, line)) {
					handleLine(line);
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				BigInt total = getTotalSafetyFactor(xLen, yLen);

				std::cout << "Total: " << total << '\n';

				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		void handleLine(std::string line)
		{
			BigInt px, py, vx, vy;

			std::regex pattern(R"(p=(-?\d{1,3}),(-?\d{1,3})\s+v=(-?\d{1,3}),(-?\d{1,3}))");
			std::smatch matches;

			if (std::regex_match(line, matches, pattern)) {
				px = std::stoi(matches[1]);
				py = std::stoi(matches[2]);
				vx = std::stoi(matches[3]);
				vy = std::stoi(matches[4]);

				startStates.push_back({ px, py, vx, vy });
			}
			else {
				std::cout << line << '\n';
				throw std::runtime_error("Failed to parse");
			}
		}

		void coutVectorCoordsAsGrid(const std::vector<coord_t>& coords, BigInt xLen, BigInt yLen)
		{
			std::vector<std::vector<int>> grid(yLen.to_int(), std::vector<int>(xLen.to_int(), 0));

			for (const auto& coord : coords)
			{
				grid[coord.second.to_int()][coord.first.to_int()]++;
			}

			for (int y = 0; y < yLen.to_int(); y++)
			{
				for (int x = 0; x < xLen.to_int(); x++)
				{
					if (grid[y][x] == 0)
						std::cout << '.';
					else
						std::cout << grid[y][x];
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
	}
	void dayFourteen() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DayFourteen" << '\n';
		const bool isFullFile = true; // true   false
		const BigInt kXLen = isFullFile ? kXLenFull : kXLenTest;
		const BigInt kYLen = isFullFile ? kYLenFull : kYLenTest;

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("dayFourteenFull.txt") : inputFile.open("dayFourteenTest.txt");
		handleFile(inputFile, kXLen, kYLen);
	}
}
